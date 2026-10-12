#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_tostring.hpp"
#include "Tre/IFFReader.h"

#include <memory>
#include <sstream>

const std::uint8_t bytes[] = {
    // FORM TEST — payload size 92
    'F','O','R','M', 0x00,0x00,0x00,0x5C, 'T','E','S','T',

    // DATA — 4 raw bytes
    'D','A','T','A', 0x00,0x00,0x00,0x04,
    0x11,0x22,0x33,0x44,

    // FORM AAAA — payload size 36
    'F','O','R','M', 0x00,0x00,0x00,0x24, 'A','A','A','A',

        // VALU — 4 raw bytes
        'V','A','L','U', 0x00,0x00,0x00,0x04,
        0x55,0x66,0x77,0x88,

        // FORM BBBB — payload size 12
        'F','O','R','M', 0x00,0x00,0x00,0x0C, 'B','B','B','B',

            // Empty leaf chunk
            'L','E','A','F', 0x00,0x00,0x00,0x00,

    // FORM CCCC — payload size 16
    'F','O','R','M', 0x00,0x00,0x00,0x10, 'C','C','C','C',

        // TEXT — 4 raw bytes
        'T','E','X','T', 0x00,0x00,0x00,0x04,
        'a','b','c','d',

    // Empty final chunk
    'T','A','I','L', 0x00,0x00,0x00,0x00,
};


namespace Catch {
    using namespace SWGEmuStructureBuilder::Tre;
    template<>
    struct StringMaker<IFFTag> {
        static std::string convert(const IFFTag& tag) {
            return "IFFTag(" + std::to_string(tag.value) + ", " + tag.ToString() + ")";
        }
    };

    template<>
    struct StringMaker<IFFChunk> {
        static std::string convert(const IFFChunk& chunk) {
            std::string result = "IFFChunk(tag=" + chunk.tag.ToString();
            if (chunk.tag == "FORM") {
                result += ", type=" + chunk.type.ToString();
            }
            result += ", dataOffset=" + std::to_string(chunk.dataOffset);
            result += ", dataSize=" + std::to_string(chunk.dataSize) + ")";
            return result;
        }
    };
}


TEST_CASE("IFFReader reads the root form", "[IFFReader]")
{
    using namespace SWGEmuStructureBuilder::Tre;

    std::istringstream input(
    std::string(reinterpret_cast<const char*>(bytes), sizeof(bytes)),
    std::ios::in | std::ios::binary);

    IFFReader reader(input);
    
    std::vector<IFFChunk> chunks = reader.ReadChunks();

    REQUIRE(chunks.size() == 1);
    
    
    {
        IFFChunk& chunk = chunks[0];
        REQUIRE(chunk.tag == "FORM");
        REQUIRE(chunk.type == "TEST");
        REQUIRE(chunk.dataOffset == 12);
        REQUIRE(chunk.dataSize == 88);
    }
    
}

TEST_CASE("IFFReader reads direct children and their payloads", "[IFFReader]")
{
    using namespace SWGEmuStructureBuilder::Tre;
    std::istringstream input(
        std::string(reinterpret_cast<const char*>(bytes), sizeof(bytes)),
        std::ios::in | std::ios::binary);
    IFFReader reader(input);
    const auto roots = reader.ReadChunks();
    REQUIRE(roots.size() == 1);
    CAPTURE(roots[0].type);
    const auto children = reader.ReadChildren(roots[0]);
    REQUIRE(children.size() == 4);

    SECTION("Leaf sizes and form bounds match the fixture")
    {
        const IFFChunk expected[] = {
            {IFFTag("DATA"), IFFTag(), 20, 4},
            {IFFTag("FORM"), IFFTag("AAAA"), 36, 32},
            {IFFTag("FORM"), IFFTag("CCCC"), 80, 12},
            {IFFTag("TAIL"), IFFTag(), 100, 0},
        };
        for (std::size_t i = 0; i < children.size(); ++i) {
            CAPTURE(children[i].tag);
            CAPTURE(i);
            CHECK(children[i].tag == expected[i].tag);
            CHECK(children[i].type == expected[i].type);
            CHECK(children[i].dataOffset == expected[i].dataOffset);
            CHECK(children[i].dataSize == expected[i].dataSize);
        }
    }

    SECTION("Nested forms expose their own direct children")
    {
        const auto nested = reader.ReadChildren(children[1]);
        REQUIRE(nested.size() == 2);
        CAPTURE(nested[0].tag);
        CHECK(nested[0].tag == "VALU");
        CHECK(nested[0].dataOffset == 44);
        CHECK(nested[0].dataSize == 4);
        CAPTURE(nested[1].tag);
        CHECK(nested[1].tag == "FORM");
        CHECK(nested[1].type == "BBBB");
        CHECK(nested[1].dataOffset == 60);
        CHECK(nested[1].dataSize == 8);

        const auto leaves = reader.ReadChildren(nested[1]);
        REQUIRE(leaves.size() == 1);
        CAPTURE(leaves[0].tag);
        CHECK(leaves[0].tag == "LEAF");
        CHECK(leaves[0].dataOffset == 68);
        CHECK(leaves[0].dataSize == 0);
        CHECK(reader.ReadChildren(leaves[0]).empty());
    }

    SECTION("Reading the same parent again does not depend on stream position")
    {
        reader.ReadChildren(children[1]);
        const auto again = reader.ReadChildren(roots[0]);
        REQUIRE(again.size() == 4);
        CHECK(again[0].tag == "DATA");
        CHECK(again[3].tag == "TAIL");
    }

    SECTION("Chunk data is limited to its payload")
    {
        const auto text = reader.ReadChildren(children[2]);
        REQUIRE(text.size() == 1);
        REQUIRE(text[0].tag == "TEXT");
        std::unique_ptr<std::streambuf> buffer(reader.GetChunkData(text[0]));
        REQUIRE(buffer != nullptr);
        std::istream payload(buffer.get());
        char result[5]{};
        payload.read(result, sizeof(result));
        CHECK(payload.gcount() == 4);
        CHECK(std::string(result, 4) == "abcd");
        CHECK(payload.eof());
    }

    SECTION("Empty chunk data immediately reaches EOF")
    {
        std::unique_ptr<std::streambuf> buffer(reader.GetChunkData(children[3]));
        REQUIRE(buffer != nullptr);
        std::istream payload(buffer.get());
        CHECK(payload.get() == std::char_traits<char>::eof());
    }
}

TEST_CASE("IFFReader searches forms and leaf chunks", "[IFFReader]")
{
    using namespace SWGEmuStructureBuilder::Tre;
    std::istringstream input(
        std::string(reinterpret_cast<const char*>(bytes), sizeof(bytes)),
        std::ios::in | std::ios::binary);
    IFFReader reader(input);
    const auto roots = reader.ReadChunks();
    REQUIRE(roots.size() == 1);
    const auto& root = roots[0];

    SECTION("Recursive search finds the root, nested forms, and a later sibling")
    {
        const IFFChunk expected[] = {
            {IFFTag("FORM"), IFFTag("TEST"), 12, 88},
            {IFFTag("FORM"), IFFTag("AAAA"), 36, 32},
            {IFFTag("FORM"), IFFTag("BBBB"), 60, 8},
            {IFFTag("FORM"), IFFTag("CCCC"), 80, 12},
        };
        for (const auto& wanted : expected) {
            CAPTURE(wanted.type);
            IFFChunk found;
            REQUIRE(reader.FindForm(wanted.type, found));
            CHECK(found.tag == wanted.tag);
            CHECK(found.type == wanted.type);
            CHECK(found.dataOffset == wanted.dataOffset);
            CHECK(found.dataSize == wanted.dataSize);
        }
    }

    SECTION("Missing forms and leaf tags are not found as forms")
    {
        IFFChunk found;
        CHECK_FALSE(reader.FindForm(IFFTag("NONE"), found));
        CHECK_FALSE(reader.FindForm(IFFTag("DATA"), found));
        CHECK(reader.FindForm(IFFTag("CCCC"), found));
    }

    SECTION("Child form search matches the type and stays at one level")
    {
        IFFChunk found;
        REQUIRE(reader.FindChildForm(root, IFFTag("AAAA"), found));
        CHECK(found.type == "AAAA");
        CHECK(found.dataOffset == 36);
        CHECK_FALSE(reader.FindChildForm(root, IFFTag("BBBB"), found));
        CHECK_FALSE(reader.FindChildForm(root, IFFTag("DATA"), found));
        CHECK_FALSE(reader.FindChildForm(root, IFFTag("NONE"), found));
    }

    SECTION("Listing child forms excludes leaves and deeper descendants")
    {
        const auto forms = reader.FindChildForms(root);
        REQUIRE(forms.size() == 2);
        CHECK(forms[0].type == "AAAA");
        CHECK(forms[1].type == "CCCC");
    }

    SECTION("Leaf search excludes forms and deeper descendants")
    {
        const auto data = reader.FindAllChildChunks(root, IFFTag("DATA"));
        REQUIRE(data.size() == 1);
        CHECK(data[0].dataOffset == 20);
        CHECK(data[0].dataSize == 4);
        CHECK(reader.FindAllChildChunks(root, IFFTag("VALU")).empty());
        CHECK(reader.FindAllChildChunks(root, IFFTag("FORM")).empty());
        CHECK(reader.FindAllChildChunks(root, IFFTag("NONE")).empty());
    }
}

TEST_CASE("IFFReader handles empty and incomplete input", "[IFFReader]")
{
    using namespace SWGEmuStructureBuilder::Tre;
    SECTION("An empty stream has no chunks or forms")
    {
        std::istringstream input;
        IFFReader reader(input);
        CHECK(reader.ReadChunks().empty());
        IFFChunk found;
        CHECK_FALSE(reader.FindForm(IFFTag("TEST"), found));
    }

    SECTION("An incomplete chunk header is not read")
    {
        std::istringstream input(std::string("FORM\0\0\0", 7));
        IFFReader reader(input);
        CHECK(reader.ReadChunks().empty());
    }

    SECTION("A chunk extending past the input is rejected")
    {
        std::istringstream input(
            std::string(reinterpret_cast<const char*>(bytes), sizeof(bytes) - 1));
        IFFReader reader(input);
        CHECK(reader.ReadChunks().empty());
        IFFChunk found;
        CHECK_FALSE(reader.FindForm(IFFTag("TEST"), found));
    }
}

#include "Tre/Helpers/ReadHelpers.h"

namespace SWGEmuStructureBuilder::Tre::Private
{
    bool CompareVecToString(const std::vector<char>& vec, const std::string_view str)
    {
        if(vec.size() != str.size()) {
            return false;
        }

        for(size_t i = 0; i < vec.size(); ++i) {
            if(vec[i] != str[i]) {
                return false;
            }
        }

        return true;
    }

    bool CompareVecToChars(const std::vector<char>& vec, std::initializer_list<char> chars)
    {
        if(vec.size() != chars.size()) {
            return false;
        }

        for(size_t i = 0; i < vec.size(); ++i) {
            if(vec[i] != chars.begin()[i]) {
                return false;
            }
        }

        return true;
    }
}
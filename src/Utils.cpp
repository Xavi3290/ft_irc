#include "../inc/Utils.hpp"

std::string toLower(const std::string& str) {
    std::string result;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        result += std::tolower(*it);
    }
    return result;
}
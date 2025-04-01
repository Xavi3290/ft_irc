#include "../../inc/NumericReplies.hpp"
#include <sstream>
#include <iomanip>

// std::string NumericReplies::reply(int code, const std::string &nick, const std::string &msg) {
//     std::ostringstream oss;

//     oss << ":irc.42.localhost ";
//     oss << std::setw(3) << std::setfill('0') << code << " ";
//     oss << (nick.empty() ? "*" : nick) << " :";
//     oss << msg << "\r\n";

//     return oss.str();
// }

std::string NumericReplies::reply(int code, const std::string& nick, const std::string& param)
{
    std::stringstream ss;
    ss << ":irc.42.localhost ";

    // Asegura que el código siempre tenga tres dígitos
    ss << std::setw(3) << std::setfill('0') << code << " ";
    ss << (nick.empty() ? "*" : nick) << " :";
    ss << param << "\r\n";

    switch(code)
    {
        case 001:
            ss << ":Welcome to the IRC Network, " << nick << "!";
            break;
        case 002:
            ss << ":Your host is irc.42.localhost running version 1.0";
            break;
        case 331:
            ss << param << ":No topic is set";
            break;
        case 332:
            ss << param << ":<topic>"; // <--- TO DO
            break;
        case 366:
            ss << param << ":End of NAMES list";
            break;
        case 433:
            ss << ":Nickname is already in use";
            break;
        case 401:
            ss << param << " :No such nick/channel";
            break;
        case 451:
            ss << ":You have not registered";
            break;
        // ...
        default:
            ss << ":Unknown reply";
            break;
    }

    ss << "\r\n";
    return ss.str();
}
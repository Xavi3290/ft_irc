#include "../../inc/NumericReplies.hpp"
#include <sstream>
#include <iomanip>

std::string NumericReplies::reply(int code, const std::string &nick, const std::string &params, const std::string &message)
{
    std::ostringstream oss;

    oss << ":irc.42.localhost ";

    oss << std::setw(3) << std::setfill('0') << code << " ";

    oss << (nick.empty() ? "*" : nick);

    if (!params.empty())
        oss << " " << params;

    if (!message.empty())
        oss << " :" << message;

    oss << "\r\n";

    return oss.str();
}

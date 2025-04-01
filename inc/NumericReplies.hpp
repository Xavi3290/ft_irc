#ifndef NUMERIC_REPLIES_HPP
#define NUMERIC_REPLIES_HPP

#include <string>

class NumericReplies
{
    public:
        static std::string reply(int code, const std::string &nick, const std::string &msg = "");
};

// Códigos de respuesta
#define RPL_WELCOME             001
#define RPL_YOURHOST            002
#define RPL_NOTOPIC             331
#define RPL_TOPIC               332
#define RPL_ENDOFNAMES          366
#define ERR_NOSUCHNICK          401
#define ERR_NICKNAMEINUSE       433
#define ERR_NONICKNAMEGIVEN     431
#define ERR_NOTREGISTERED       451
#define ERR_NEEDMOREPARAMS      461
#define ERR_NOSUCHCHANNEL       403
#define ERR_CANNOTSENDTOCHAN    404

#endif

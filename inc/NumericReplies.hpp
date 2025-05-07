#ifndef NUMERIC_REPLIES_HPP
#define NUMERIC_REPLIES_HPP

#include <string>

class NumericReplies
{
    public:
        static std::string reply(int code, const std::string &nick, const std::string &params = "", const std::string &message = "");
};

// Códigos de respuesta numéricos
#define RPL_WELCOME             001
#define RPL_YOURHOST            002
#define RPL_CREATED             003
#define RPL_MYINFO              004
#define RPL_UNAWAY              305
#define RPL_NOWAWAY             306
#define RPL_TOPIC               332
#define RPL_NOTOPIC             331
#define RPL_NAMREPLY            353
#define RPL_ENDOFNAMES          366
#define RPL_INVITING            341

#define ERR_NOSUCHNICK          401
#define ERR_NOSUCHCHANNEL       403
#define ERR_CANNOTSENDTOCHAN    404
#define ERR_NOORIGIN            409
#define ERR_USERNOTINCHANNEL    441
#define ERR_NOTONCHANNEL        442
#define ERR_USERONCHANNEL       443
#define ERR_NOTREGISTERED       451
#define ERR_NEEDMOREPARAMS      461
#define ERR_ALREADYREGISTRED    462
#define ERR_PASSWDMISMATCH      464
#define ERR_CHANNELISFULL       471
#define ERR_UNKNOWNMODE         472
#define ERR_INVITEONLYCHAN      473
#define ERR_BADCHANNELKEY       475
#define ERR_CHANOPRIVSNEEDED    482
#define ERR_NICKNAMEINUSE       433
#define ERR_NONICKNAMEGIVEN     431
#define ERR_ERRONEUSNICKNAME    432

#endif

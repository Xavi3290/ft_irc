#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <string>
#include <sstream> 

void Server::handlePing(Client *client, std::istringstream &iss)
{
    std::string parameter;
    if (!(iss >> parameter)) {
        sendReplyTo(client, ERR_NOORIGIN, "", "No origin specified");
        return;
    }
    std::string serverName = "server";
	std::string pongReply = ":" + serverName + " PONG " + serverName + " :" + parameter + "\r\n";
	send(client->getFd(), pongReply.c_str(), pongReply.size(), 0);
}
#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handlePing(Client *client, std::istringstream &iss)
{
    std::string parameter;
    if (!(iss >> parameter)) {
        std::string errorMsg = ":server 409 * :No origin specified for PING\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::string serverName = "server";
	std::string pongReply = ":" + serverName + " PONG " + serverName + " :" + parameter + "\r\n";
	send(client->getFd(), pongReply.c_str(), pongReply.size(), 0);
}
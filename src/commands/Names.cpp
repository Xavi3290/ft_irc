#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleNames(Client *client, std::istringstream &iss)
{
    std::string target;
    if (!(iss >> target)) {
        std::string errorMsg = ":server 461 NAMES :Not enough parameters\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    Channel *channel = getChannelByName(target);
    if (channel) {
        const std::vector<Client *> &clients = channel->getClients();
        std::string nameList;
        for (size_t i = 0; i < clients.size(); i++){
            if (channel->isOperator(clients[i]))
                nameList += "@";
            nameList += clients[i]->getNickname() + " ";
        }
        std::string namesReply = ":server 353 " + client->getNickname() + " = " + target + " :" + nameList + "\r\n";
        send(client->getFd(), namesReply.c_str(), namesReply.size(), 0);
        std::string endMsg = ":server 366 " + client->getNickname() + " " + target + " :End of /NAMES list\r\n";
        send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
    }
    else {
        std::string errorMsg = ":server 402 " + target + " :No such nick/channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
    }
}
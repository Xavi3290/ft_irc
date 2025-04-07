#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handlePart(Client *client, std::istringstream &iss)
{
    if (!client->isRegistered()) {
        std::string errorMsg = "451 :You have not registered\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::string channelName;
    iss >> channelName;
    if (channelName.empty()) {
        std::string errorMsg = "461 PART :Not enough parameters\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    Channel *channel = getChannelByName(channelName);
    if (channel && channel->hasClient(client)) {
        channel->removeClient(client);
        std::string partMsg = "You have left channel " + channelName + "\r\n";
        send(client->getFd(), partMsg.c_str(), partMsg.size(), 0);
        std::cout << "Client " << client->getFd() << " left channel " << channelName << std::endl;
        std::string broadcast = ":" + client->getNickname() + " has left channel " + channelName + "\r\n";
        channel->broadcastMessage(broadcast, client);
    } else {
        std::string errorMsg = "Error: you are not in channel " + channelName + "\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
    }
}
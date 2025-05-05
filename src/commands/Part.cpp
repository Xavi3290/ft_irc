#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handlePart(Client *client, std::istringstream &iss)
{
    if (!client->isRegistered()) {
        sendReplyTo(client, ERR_NOTREGISTERED, "", "You have not registered");
        return;
    }
    std::string channelName;
    iss >> channelName;
    if (channelName.empty()) {
        sendReplyTo(client, ERR_NEEDMOREPARAMS, "PART", "Not enough parameters");
        return;
    }
    Channel *channel = getChannelByName(channelName);
    if (!channel) {
        sendReplyTo(client, ERR_NOSUCHCHANNEL, channel->getOriginalName(), "No such channel");
        return;
    }
    else if (channel->hasClient(client)) {
 		std::string partMsg = ":" + client->getPrefix() + " PART " + channel->getOriginalName() + "\r\n";
        send(client->getFd(), partMsg.c_str(), partMsg.size(), 0);
		channel->broadcastMessage(partMsg, client);
		removeClientChannel(client->getFd());
    } else {
        sendReplyTo(client, ERR_NOTONCHANNEL, channelName, "You're not on that channel");
    }
}
#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>


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
        sendReplyTo(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
        return;
    }
    else if (channel->hasClient(client)) {
		if (channel->isOperator(client)) 
			channel->removeOperator(client);
 		std::string partMsg = ":" + client->getPrefix() + " PART " + channel->getOriginalName() + "\r\n";
        send(client->getFd(), partMsg.c_str(), partMsg.size(), 0);
		channel->broadcastMessage(partMsg, client);
		channel->removeClient(client);
		if (channel->getClients().empty()) {
			delete channel;
			_channels.erase(std::remove(_channels.begin(), _channels.end(), channel), _channels.end());
		}
		else
			channel->broadcastMessage(":" + client->getNickname() + " PART " + channel->getOriginalName() + "\r\n", client);
	}
	else
        sendReplyTo(client, ERR_NOTONCHANNEL, channelName, "You're not on that channel");
}
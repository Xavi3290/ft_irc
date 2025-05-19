#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>



void Server::handleTopic(Client *client, std::istringstream &iss)
{
    std::string channelName, newTopic;
    if (!(iss >> channelName)) {
        std::string errorMsg = ":server 461 TOPIC :Not enough parameters\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    Channel *channel = getChannelByName(channelName);
    if (!channel) {
        std::string errorMsg = ":server 403 " + channelName + " :No such nick/channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    std::getline(iss, newTopic);
	newTopic.erase(std::remove(newTopic.begin(), newTopic.end(), '\r'), newTopic.end());
	if (newTopic[0] == ' ')
		newTopic = newTopic.substr(1);
	if (!newTopic.empty() && newTopic[0] == ':')
		newTopic = newTopic.substr(1);
	if (newTopic.empty() && !channel->getTopic().empty()) {
		std::string topicMsg = ":server 332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
		send(client->getFd(), topicMsg.c_str(), topicMsg.size(), 0);
		return;
	}
    else if (newTopic.empty() && channel->getTopic().empty()) {
		sendReplyTo(client, RPL_NOTOPIC, channelName, "No topic is set");
		return;
	}
    if (!channel->isOperator(client) && channel->isTopicRestricted()) {
        std::string errorMsg = ":server 482 " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    if (!channel->hasClient(client)) {
        std::string errorMsg = ":server 442 " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
	channel->setTopic(newTopic);
	std::string topicMsg = ":server 332 " + client->getNickname() + " " + channelName + " :" + newTopic + "\r\n";
	send(client->getFd(), topicMsg.c_str(), topicMsg.size(), 0);
	std::string broadcast = ":" + client->getNickname() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
	channel->broadcastMessage(broadcast, client);
}

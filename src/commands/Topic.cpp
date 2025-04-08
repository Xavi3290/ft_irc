#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleTopic(Client *client, std::istringstream &iss)
{
    std::string channelName, newTopic;
    if (!(iss >> channelName)) {
        std::string errorMsg = ":server 461 TOPIC :Not enough parameters\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::getline(iss, newTopic);
    if(!newTopic.empty() && newTopic[0] == ' ')
        newTopic = newTopic.substr(1);
    else
        newTopic = "No topic";
    Channel *channel = getChannelByName(channelName);
    if (!channel) {
        std::string errorMsg = ":server 403 " + channelName + " :No such nick/channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    if (!channel->isOperator(client)) {
        std::string errorMsg = ":server 482 " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    if (!channel->hasClient(client)) {
        std::string errorMsg = ":server 442 " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    if (newTopic == "No topic")
        sendReplyTo(client, RPL_NOTOPIC, channelName, "No topic is set");
    else {
        std::string topicMsg = ":server 332 " + client->getNickname() + " " + channelName + " :" + newTopic + "\r\n";
        send(client->getFd(), topicMsg.c_str(), topicMsg.size(), 0);
    }
    std::string broadcast = ":" + client->getNickname() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    channel->setTopic(newTopic);
    channel->broadcastMessage(broadcast, client);
}
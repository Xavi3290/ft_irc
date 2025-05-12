#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleKick(Client *client, std::istringstream &iss)
{
    std::string channelName, targetNick, reason;
    if (!(iss >> channelName >> targetNick)) {
        std::string errorMsg = ":server 461 KICK :Not enough parameters\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::getline(iss, reason);
    if(!reason.empty() && reason[0] == ' ')
        reason = reason.substr(1);
    else
        reason = "No reason";
    Channel *channel = getChannelByName(channelName);
    if (!channel->isOperator(client)) {
        std::string errorMsg = ":server 482 " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    if (!channel) {
        std::string errorMsg = ":server 403 " + client->getNickname() + " " + channelName + " :No such channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    Client *target = findClientByNick(targetNick);
    if (!target) {
        std::string errorMsg = ":server 401 " + client->getNickname() + " " + targetNick + " :No such nick\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    if (!channel->hasClient(client)) {
        std::string errorMsg = ":server 442 " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    if (!channel->hasClient(target)) {
        std::string errorMsg = ":server 441 " + client->getNickname() + " " + targetNick + " :They're not on that channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    channel->removeClient(target);
    std::string kickMsg = ":server KICK " + channelName + " " + targetNick + " :You have been kicked from the channel\r\n";
    send(target->getFd(), kickMsg.c_str(), kickMsg.size(), 0);
    std::string broadcast = ":" + client->getNickname() + " KICK " + channelName + " " + targetNick + " :has been kicked from the channel\r\n";
	send(client->getFd(), broadcast.c_str(), broadcast.size(), 0);
    channel->broadcastMessage(broadcast, client);
}
#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <string>
#include <sstream> 

void Server::handleInvite(Client *client, std::istringstream &iss) {
		std::string targetNick, channelName;
		if (!(iss >> targetNick >> channelName)) {
			sendReplyTo(client, ERR_NEEDMOREPARAMS, "INVITE", "Not enough parameters");
			return;
		}
		Client *target = findClientByNick(targetNick);
		if (!target) {
			sendReplyTo(client, ERR_NOSUCHNICK, targetNick, "No such nick");
			return;
		}
		Channel *channel = getChannelByName(channelName);
		if (!channel) {
			sendReplyTo(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
			return;
		}
		if (!channel->hasClient(client)) {
			sendReplyTo(client, ERR_NOTONCHANNEL, channelName, "You're not on the channel");
			return;
		}
		if (channel->isInviteOnly() && !channel->isOperator(client)) {
			sendReplyTo(client, ERR_CHANOPRIVSNEEDED, channelName, "You're not channel operator");
			return;
		}
		if (channel->isInvited(target)) {
			sendReplyTo(client, ERR_USERONCHANNEL, targetNick + " " + channelName, "User is already on the channel");
			return;
		}
		channel->addInvited(target);
		sendReplyTo(client, RPL_INVITING, targetNick, channelName);
		std::string inviteMsg = ":" + client->getPrefix() + " INVITE " + targetNick + " :" + channelName + "\r\n";
		send(target->getFd(), inviteMsg.c_str(), inviteMsg.size(), 0);

		std::cout << "Client " << client->getFd() << " invited user " << target->getFd() << " to channel " << channelName << std::endl;
	}
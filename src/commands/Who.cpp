#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleWho(Client *client, std::istringstream &iss) {
		std::string channelname;
		if (!(iss >> channelname)) {
			sendReplyTo(client, ERR_NEEDMOREPARAMS, "WHO", "Not enough parameters");
			return;
		}
		Channel *channel = getChannelByName(channelname);
		if (!channel) {
			sendReplyTo(client, ERR_NOSUCHCHANNEL, channelname, "No such channel");
			return;
		}
		if (!channel->hasClient(client)) {
			sendReplyTo(client, ERR_NOTONCHANNEL, channelname, "You're not on that channel");
			return;
		}
		const std::vector<Client *> &clients = channel->getClients();
		for (size_t i = 0; i < clients.size(); i++) {
			Client *target = clients[i];
			std::string status;
			if (clients[i]->isAway())
				status = "G";
			else
				status = "H";
			if (channel->isOperator(clients[i]))
				status += "@";
			std::string whoReply = ":server 352 " + client->getNickname() + " " +
			channelname + " " + target->getUsername() + " host server " +
			target->getNickname() + " " + status + " :0 " + target->getRealname() + "\r\n";
			send(client->getFd(), whoReply.c_str(), whoReply.size(), 0);
		}
		std::string endMsg = ":server 315 " + client->getNickname() + " " + channelname + " :End of /WHO list\r\n";
		send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
	}
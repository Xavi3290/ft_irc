#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 


void Server::handleWho(Client *client, std::istringstream &iss) {
		std::string target;
		if (!(iss >> target)) {
			sendReplyTo(client, ERR_NEEDMOREPARAMS, "WHO", "Not enough parameters");
			return;
		}
		Channel *channel = getChannelByName(target);
		if (channel && channel->hasClient(client)) {
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
				channel->getOriginalName() + " " + target->getUsername() + " host server " +
				target->getNickname() + " " + status + " :0 " + target->getRealname() + "\r\n";
				send(client->getFd(), whoReply.c_str(), whoReply.size(), 0);
			}
		}
		else {
			Client *targetClient = findClientByNick(target);
			if (targetClient) {
				std::string status;
				if (targetClient->isAway())
					status = "G";
				else
					status = "H";
				std::string whoReply = ":server 352 " + client->getNickname() + " " +
				target + " " + targetClient->getUsername() + " host server " +
				targetClient->getNickname() + " " + status + " :0 " + targetClient->getRealname() + "\r\n";
				send(client->getFd(), whoReply.c_str(), whoReply.size(), 0);
			}
		}
		std::string endMsg = ":server 315 " + client->getNickname() + " " + target + " :End of /WHO list\r\n";
		send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
	}	
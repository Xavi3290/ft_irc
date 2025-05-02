#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream>
#include <algorithm>

void Server::handleAway(Client *client, std::istringstream &iss) {
		std::string awayMsg;
		getline(iss, awayMsg);
		awayMsg.erase(std::remove(awayMsg.begin(), awayMsg.end(), '\r'), awayMsg.end());
		if (awayMsg.empty()) {
			client->setAway(false);
			std::string reply = ":server 305 " + client->getNickname() + " :You are no longer marked as being away\r\n";
			send(client->getFd(), reply.c_str(), reply.size(), 0);
			std::cout << "Client " << client->getFd() << " is no longer away" << std::endl;
		} else {
			client->setAway(true);
			client->setAwayMessage(awayMsg);
			std::string reply = ":server 306 " + client->getNickname() + " :You have been marked as being away\r\n";
			send(client->getFd(), reply.c_str(), reply.size(), 0);
			std::cout << "Client " << client->getFd() << " is now away: " << awayMsg << std::endl;
		}
	}
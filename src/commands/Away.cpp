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
			sendReplyTo(client, RPL_UNAWAY, "", "You are no longer marked as being away");
			std::cout << "Client " << client->getFd() << " is no longer away" << std::endl;
		} else {
			client->setAway(true);
			client->setAwayMessage(awayMsg);
			sendReplyTo(client, RPL_NOWAWAY, "", "You have been marked as being away");
			std::cout << "Client " << client->getFd() << " is now away: " << awayMsg << std::endl;
		}
	}
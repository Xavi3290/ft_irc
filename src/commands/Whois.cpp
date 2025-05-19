#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <string>
#include <sstream> 


void Server::handleWhois(Client *client, std::istringstream &iss) {
	std::string targetNick;
	if (!(iss >> targetNick)) {
		sendReplyTo(client, ERR_NEEDMOREPARAMS, "WHOIS", "Not enough parameters");
		return;
	}
	Client *target = findClientByNick(targetNick);
	if (!target) {
		sendReplyTo(client, ERR_NOSUCHNICK, targetNick, "No such nick/channel");
		return;
	}

	std::string r311 = ":server 311 " + client->getNickname() + " " +
		target->getNickname() + " " + target->getUsername() + " " +
		target->getIP() + " * :" + target->getRealname() + "\r\n";
	send(client->getFd(), r311.c_str(), r311.size(), 0);

	std::string channelList;
	for (size_t i = 0; i < _channels.size(); i++) {
		if (_channels[i]->hasClient(target) && _channels[i]->hasClient(client)) {
			if (_channels[i]->isOperator(target))
				channelList += "@";
			channelList += _channels[i]->getOriginalName() + " ";
		}
	}
	if (!channelList.empty()) {
		std::string r319 = ":server 319 " + client->getNickname() + " " + target->getNickname() + " :" + channelList + "\r\n";
		send(client->getFd(), r319.c_str(), r319.size(), 0);
	}

	std::string r312 = ":server 312 " + client->getNickname() + " " +
		target->getNickname() + " server :IRC42\r\n";
	send(client->getFd(), r312.c_str(), r312.size(), 0);

	std::string r378 = ":server 378 " + client->getNickname() + " " +
		target->getNickname() + " :" + target->getIP() + " :actually using host\r\n";
	send(client->getFd(), r378.c_str(), r378.size(), 0);

	std::string r318 = ":server 318 " + client->getNickname() + " " +
		target->getNickname() + " :End of /WHOIS list\r\n";
	send(client->getFd(), r318.c_str(), r318.size(), 0);
}

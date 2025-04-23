#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
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
	std::string whoisReply = ":server 311 " + client->getNickname() + " " + target->getNickname() + " " +
							 target->getUsername() + " host :0 :" + target->getRealname() + "\r\n";
	send(client->getFd(), whoisReply.c_str(), whoisReply.size(), 0);
	std::string endMsg = ":server 318 " + client->getNickname() + " " + target->getNickname() + " :End of /WHOIS list\r\n";
	send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
}
#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <algorithm>

void Server::handlePrivMsg(Client *client, std::istringstream &iss)
{
    if (!client->isRegistered()) {
        sendReplyTo(client, ERR_NOTREGISTERED, "", "You have not registered");
        return;
    }
    std::string target;
    if (!(iss >> target)) {
        sendReplyTo(client, ERR_NEEDMOREPARAMS, "PRIVMSG", "Not enough parameters");
        return;
    }
    std::string msg;
    getline(iss, msg);

	msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
    if (msg.empty()) {
		sendReplyTo(client, ERR_NOTEXTTOSEND, "", "No text to send");
		return;
	}

    if (msg.find("!bot") != std::string::npos) {
        handleBotCommand(target, msg);
        return;
    }
    
    Channel *channel = getChannelByName(target);
    if (channel && channel->hasClient(client) && target[0] == '#') {
        sendToChannel(client, target, msg);
        std::cout << "Broadcast message from client " << client->getFd() << " to channel " << target << std::endl;
    } else if(findClientByNick(target)) {
		if (findClientByNick(target)->isAway()) {
			std::string awayMsg = ":server 301 " + client->getNickname() + " " +
			target + findClientByNick(target)->getAwayMessage() + "\r\n";
			send(client->getFd(), awayMsg.c_str(), awayMsg.size(), 0);
		}
 		sendToUser(client, target, msg);
        std::cout << "Message from client " << client->getFd() << " to user " << target << std::endl;
    } else {
        std::string errorMsg = "Error: you are not in channel " + target + "\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
    }
}
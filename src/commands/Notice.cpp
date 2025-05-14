#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream>
#include <stdlib.h> 
#include <algorithm>

void Server::handleNotice(Client *client, std::istringstream &iss)
{
	std::string target;
    std::string msg;
	std::cout << "msg: " << msg << std::endl;
	std::cout << "target: " << target << std::endl;
    if (!(iss >> target)){
        return;
	}
	getline(iss, msg);
	msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
	if (msg.empty()) {
		return;
	}
    Channel *channel = getChannelByName(target);
    if (channel && channel->hasClient(client) && target[0] == '#') {
        sendToChannel(client, target, msg);
        std::cout << "Broadcast message from client " << client->getFd() << " to channel " << target << std::endl;
    } else if (findClientByNick(target)) {
 		std::string fullMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost NOTICE " + target + " " + msg + "\r\n";
    	send(findClientByNick(target)->getFd(), fullMsg.c_str(), fullMsg.size(), 0);
        std::cout << "Message from client " << client->getFd() << " to user " << target << std::endl;
    }
}
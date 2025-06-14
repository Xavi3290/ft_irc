#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream> 

void Server::handleQuit(Client *client, std::istringstream &iss)
{
    std::string quitMsg;
    getline(iss, quitMsg);
    std::string quitMsg2 = ":" + client->getNickname() + " QUIT :" + quitMsg + "\r\n";
    std::cout << "Client " << client->getFd() << " quit" << std::endl;
    for (size_t i = 0; i < _pollFds.size(); i++) {
        if (_pollFds[i].fd == client->getFd()) {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
    removeClient(client->getFd());
}
#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <unistd.h>  // Para close(), read()
#include <string>
#include <sstream> 

void Server::handleQuit(Client *client, std::istringstream &iss)
{
    std::string quitMsg;
    getline(iss, quitMsg);
    std::string quitMsg2 = ":" + client->getNickname() + " QUIT :" + quitMsg + "\r\n";
    for (size_t i = 0; i < _channels.size(); i++) {
        Channel *channel = _channels[i];
        if (channel->hasClient(client)) {
            channel->broadcastMessage(quitMsg2, client);
        }
    }
    std::cout << "Client " << client->getFd() << " quit" << std::endl;
    for (size_t i = 0; i < _pollFds.size(); i++) {
        if (_pollFds[i].fd == client->getFd()) {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
    close(client->getFd());
    removeClient(client->getFd());
}
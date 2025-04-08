#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleNick(Client *client, std::istringstream &iss)
{
    std::string newNick;
    if (!(iss >> newNick)) {
        std::string errorMsg = ":server 431 * :No nickname given\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    client->setNickname(newNick);
    std::cout << "Client " << client->getFd() << " set nickname to " << newNick << std::endl;
    if (!client->getNickname().empty() && !client->getUsername().empty() && client->hasProvidedPass() && !client->isRegistered()) {
        std::string welcome = ":server 001 " + client->getNickname() + " :Welcome to the IRC server!\r\n";
        send(client->getFd(), welcome.c_str(), welcome.size(), 0);
        client->setRegistered(true);
        std::cout << "Client " << client->getFd() << " registered." << std::endl;
    }
}
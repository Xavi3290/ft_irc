#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream> 

void Server::handlePass(Client *client, std::istringstream &iss)
{
    std::string pass;

    if (client->isRegistered()) {
        sendReplyTo(client, ERR_ALREADYREGISTRED, "", "You may not reregister");
        return;
    }
    if (!(iss >> pass)) {
        sendReplyTo(client, ERR_NEEDMOREPARAMS, "PASS", "Not enough parameters");
        return;
    }
    if (pass != _password) {
        sendReplyTo(client, ERR_PASSWDMISMATCH, "", "Password incorrect");
        std::cout << "Client " << client->getFd() << " provided invalid password." << std::endl;
        close(client->getFd());
        removeClient(client->getFd());
        return;
    } else {
        client->send("Provided correct password, set USER and NICK");
        client->setPassProvided(true);

        std::cout << "Client " << client->getFd() << " provided valid password" << std::endl;
    }
}
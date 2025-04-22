#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

bool isValidUsername(const std::string &username) {
    for (size_t i = 0; i < username.size(); ++i) {
        if (!std::isalnum(username[i]) && username[i] != '_' && username[i] != '-')
            return false;
    }
    return true;
}

void Server::handleUser(Client *client, std::istringstream &iss)
{
    std::string username, userMode, hostname, realname;

    if (client->isRegistered()) {
        sendReplyTo(client, ERR_ALREADYREGISTRED, "", "You may not reregister");
        return;
    }
    if (!(iss >> username) || !(iss >> userMode) || !(iss >> hostname) || !(iss >> realname)) {
        sendReplyTo(client, ERR_NEEDMOREPARAMS, "USER", "Not enough parameters");
        return;
    }
    if (!isValidUsername(username)) {
        client->send(username + ": Invalid username try with alphanumeric and '-' or '_'");
        return;
    }
    client->setUsername(username);
    client->setRealname(realname.substr(1));
    std::cout << "Client " << client->getFd() << " set username to " << username << std::endl;
    if (!client->getNickname().empty() && !client->getUsername().empty() && client->hasProvidedPass() && !client->isRegistered()) {
        sendReplyTo(client, RPL_WELCOME, "", ("Welcome to the 42 IRC Server Network " + client->getPrefix()));
        client->setRegistered(true);
        std::cout << "Client " << client->getFd() << " registered." << std::endl;
    }
}
#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream>
#include <cctype>   // para std::tolower
#include <set>

std::string toLower(const std::string& str) {
    std::string result;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        result += std::tolower(*it);
    }
    return result;
}

bool isValidNick(const std::string &nick) {
    if (nick.length() > 15) // puedes ajustar el límite
        return false;

    char first = nick[0];
    if (std::isdigit(first) || first == '#' || first == ':' || first == ' ')
        return false;

    for (size_t i = 0; i < nick.size(); ++i) {
        char c = nick[i];
        if (!(std::isalnum(c) || c == '-' || c == '_' ||
              c == '[' || c == ']' || c == '{' || c == '}' ||
              c == '\\' || c == '|'))
            return false;
    }
    return true;
}

bool isNickInUse(std::string &nickName, std::vector<Client*> &clients) {
    std::string lowerNick = toLower(nickName);

    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client *client = *it;
        if (toLower(client->getNickname()) == lowerNick)
            return true;
    }
    return false;
}

void Server::handleNick(Client *client, std::istringstream &iss)
{
    std::string nickName;
    if (!(iss >> nickName)) {
        sendReplyTo(client, ERR_NONICKNAMEGIVEN, "", "No nickname given");
        return;
    }

    if(!isValidNick(nickName)) {
        sendReplyTo(client, ERR_ERRONEUSNICKNAME, nickName, "Erroneus nickname");
        return;
    }

    if (isNickInUse(nickName, _clients)) {
        sendReplyTo(client, ERR_NICKNAMEINUSE, nickName, "Nickname is already in use");
        return;
    }

    if (client->isRegistered()){
        std::set<Client*> notified;

        std::string msg = ":" + client->getPrefix() + " NICK :" + nickName + "\r\n";

        for (std::vector<Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it) {
            Channel *channel = *it;

            if (!channel->hasClient(client))
                continue;

            const std::vector<Client*> &members = channel->getClients();
            for (std::vector<Client*>::const_iterator mit = members.begin(); mit != members.end(); ++mit) {
                Client *member = *mit;
                if (notified.find(member) == notified.end()) {
                    member->send(msg);
                    notified.insert(member);
                }
            }
        }
    }

    client->setNickname(nickName);
    std::cout << "Client " << client->getFd() << " set nickname to " << nickName << std::endl;
    if (!client->getNickname().empty() && !client->getUsername().empty() && client->hasProvidedPass() && !client->isRegistered()) {
        std::string welcome = ":server 001 " + client->getNickname() + " :Welcome to the IRC server!\r\n";
        send(client->getFd(), welcome.c_str(), welcome.size(), 0);
        client->setRegistered(true);
        std::cout << "Client " << client->getFd() << " registered." << std::endl;
    }
}
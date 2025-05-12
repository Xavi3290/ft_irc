#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleList(Client *client, std::istringstream &iss)
{
    std::string nothing;
    iss >> nothing;
    if (!client->isRegistered()) {
        std::string errorMsg = ":server 451 * :You have not registered\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::string listMsg = ":server 321 " + client->getNickname() + " Channel :Users Name\r\n";
    send(client->getFd(), listMsg.c_str(), listMsg.size(), 0);
    for (size_t i = 0; i < _channels.size(); i++) {
        Channel *channel = _channels[i];
        std::string channelName = channel->getOriginalName();
        std::string topic = channel->getTopic();
        if (topic.empty())
            topic = "No topic";
        int userCount = channel->getClientCount();
        std::ostringstream oss;
        oss << userCount;
        listMsg = ":server 322 " + client->getNickname() + " " + channelName + " " + oss.str() + " :" + topic + "\r\n";
        send(client->getFd(), listMsg.c_str(), listMsg.size(), 0);
    }
    std::string endMsg = ":server 323 " + client->getNickname() + " :End of /LIST\r\n";
    send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
}
#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleList(Client *client)
{
    if (!client->isRegistered()) {
        std::string errorMsg = ":server 451 * :You have not registered\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::string listMsg = ":server 321 " + client->getNickname() + " Channel :Users Name\r\n";
    send(client->getFd(), listMsg.c_str(), listMsg.size(), 0);
    for (size_t i = 0; i < _channels.size(); i++) {
        Channel *channel = _channels[i];
        std::string channelName = channel->getOriginalName();
        std::string topic = channel->getTopic();
        if (topic.empty())
            topic = "No topic";
        int userCount = channel->getClientCount();
        std::ostringstream oss;
        oss << userCount;
        listMsg = ":server 322 " + client->getNickname() + " " + channelName + " " + oss.str() + " :" + topic + "\r\n";
        send(client->getFd(), listMsg.c_str(), listMsg.size(), 0);
    }
    std::string endMsg = ":server 323 " + client->getNickname() + " :End of /LIST\r\n";
    send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
}
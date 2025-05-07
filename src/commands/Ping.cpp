#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handlePing(Client *client, std::istringstream &iss)
{
    std::string parameter;
    if (!(iss >> parameter)) {
        sendReplyTo(client, ERR_NOORIGIN, "", "No origin specified");
        return;
    }
    std::string pong = "PONG " + parameter + "\r\n";
    send(client->getFd(), pong.c_str(), pong.size(), 0);
    std::cout << "Responding with: " << pong;
}
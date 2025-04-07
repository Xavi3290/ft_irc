#include "../../inc/Server.hpp"
// #include "../../inc/Client.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handlePing(Client *client, std::istringstream &iss)
{
    std::string parameter;
    if (!(iss >> parameter)) {
        std::string errorMsg = ":server 409 * :No origin specified for PING\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::string pong = "PONG " + parameter + "\r\n";
    send(client->getFd(), pong.c_str(), pong.size(), 0);
    std::cout << "Responding with: " << pong;
}
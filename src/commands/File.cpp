#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleFile(Client *client, std::istringstream &iss)
{
    if (!client->isRegistered()) {
        std::string errorMsg = ":server 451 * :You have not registered\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::string target, filename;
    size_t filesize;
    if (!(iss >> target >> filename >> filesize)) {
        std::string errorMsg = ":server 461 FILE :Not enough parameters\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    
    std::ostringstream oss;
    oss << filesize;
    std::string info = ":server NOTICE " + target + " :Incoming file " + filename + " (" + oss.str() + " bytes)\r\n";
    send(client->getFd(), info.c_str(), info.size(), 0);

    std::string filedata;
    filedata.resize(filesize);
    size_t total_read = 0;
    while (total_read < filesize) {
        int bytes = recv(client->getFd(), &filedata[total_read], filesize - total_read, 0);
        if (bytes <= 0) {
            std::string errorMsg = ":server ERROR :File transfer interrupted\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        total_read += bytes;
    }

    Channel *channel = getChannelByName(target);
    if (channel) {
        std::ostringstream oss2;
        oss2 << filesize;
        std::string broadcast = ":" + client->getNickname() + " FILE " + filename + " " + oss2.str() + "\r\n" + filedata;
        channel->broadcastMessage(broadcast, client);
        std::cout << "File " << filename << " (" << filesize << " bytes) sent by client " 
                << client->getFd() << " to channel " << target << std::endl;
    } else {
        std::string errorMsg = ":server 402 " + target + " :No such nick/channel\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
    }
}
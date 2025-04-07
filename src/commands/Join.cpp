#include "../../inc/Server.hpp"
// #include "../../inc/Client.hpp"
// #include "../../inc/Channel.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleJoin(Client *client, std::istringstream &iss)
{
    std::string channelName;

    if (!(iss >> channelName))
    {
        sendReplyTo(client, ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters");
        return;
    }

    if (!client->isRegistered()) {
			sendReplyTo(client, ERR_NOTREGISTERED, "", "You have not registered");
            return;
        }
	if(channelName[0] != '#') {
		sendReplyTo(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
		return;
	}
        Channel *channel = getChannelByName(channelName);
        if (!channel) {
            channel = new Channel(channelName);
            _channels.push_back(channel);
			channel->addOperator(client);
        }
        if (!channel->hasClient(client))
            channel->addClient(client);
        
        std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + channelName + "\r\n";
        channel->broadcastMessage(joinMsg, NULL);
        
        //SI NO HAY TOPIC

        //sendReplyTo(client, RPL_NOTOPIC, channelName, "No topic is set");

        //LISTAR USUARIOS
        const std::vector<Client *> &clients = channel->getClients();
        std::string nameList;
        for (size_t i = 0; i < clients.size(); ++i)
            nameList += clients[i]->getNickname() + " ";

        std::string namesReply = ":irc.42.localhost 353 " + client->getNickname() + " = " + channelName + " :" + nameList + "\r\n";
        send(client->getFd(), namesReply.c_str(), namesReply.size(), 0);

        // 4. Fin de lista (RPL_ENDOFNAMES)
        sendReplyTo(client, RPL_ENDOFNAMES, channelName, "End of /NAMES list");
}

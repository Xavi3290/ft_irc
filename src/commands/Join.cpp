#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <string>
#include <sstream> 
#include <cctype>

void Server::handleJoin(Client *client, std::istringstream &iss)
{
    std::string channelName, key;

    if (!(iss >> channelName))
    {
        sendReplyTo(client, ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters");
        return;
    }

    iss >> key;

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
		channel->addClient(client);
    }
	if (!channel->hasClient(client)) {
        if (channel->isInviteOnly() && !channel->isInvited(client)) {
            sendReplyTo(client, ERR_INVITEONLYCHAN, channelName, "Cannot join channel (+i)");
            return;
        }
        if (channel->isFull()) {
            sendReplyTo(client, ERR_CHANNELISFULL, channelName, "Cannot join channel (+l)");
            return;
        }
        if (channel->isKeySet() && channel->getKey() != key) {
            sendReplyTo(client, ERR_BADCHANNELKEY, channelName, "Cannot join channel (+k)");
            return;
        }
        channel->addClient(client);
    }
    
    std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + channel->getOriginalName() + "\r\n";
    channel->broadcastMessage(joinMsg, NULL);

    if (channel) {
		
		if (channel->getTopic().empty()) {
						
			std::string errorMsg = ":server 331 " + client->getNickname() + " " + channelName + " :No topic is set\r\n";
        	send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
		} 
		else {
			std::string topicMsg = ":server 332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
			send(client->getFd(), topicMsg.c_str(), topicMsg.size(), 0);
		}
		const std::vector<Client *> &clients = channel->getClients();
        std::string nameList;
        for (size_t i = 0; i < clients.size(); i++){
            if (channel->isOperator(clients[i]))
                nameList += "@";
            nameList += clients[i]->getNickname() + " ";
        }
        std::string namesReply = ":server 353 " + client->getNickname() + " = " + channelName + " :" + nameList + "\r\n";
        send(client->getFd(), namesReply.c_str(), namesReply.size(), 0);
        sendReplyTo(client, RPL_ENDOFNAMES, channelName, "End of /NAMES list");
    }
    std::cout << "Client " << client->getFd() << " joined channel " << channelName << std::endl;
}

#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream> 

void Server::handleMode(Client *client, std::istringstream &iss) {
		std::string channelName, mode;
		if (!(iss >> channelName)) {
			sendReplyTo(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
			return;
		}
		Channel *channel = getChannelByName(channelName);
		if (!channel) {
			sendReplyTo(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
			return;
		}
		if (!(iss >> mode)) {
			std::string modes = channel->getMode(channel, client);

			std::string modeMsg = ":server 324 " + client->getNickname() + " " + channelName + " :" + modes +"\r\n";
			send(client->getFd(), modeMsg.c_str(), modeMsg.size(), 0);
			return;
		}
		if (!channel->isOperator(client)) {
			sendReplyTo(client, ERR_CHANOPRIVSNEEDED, channelName, "You're not channel operator");
			return;
		}
		if (mode == "-t") {
			channel->setTopicRestricted(false);
			std::string modeMsg = ":server MODE " + channelName + " -t\r\n";
			channel->broadcastMessage(modeMsg, client);
		} 
		else if (mode == "+t") {
			channel->setTopicRestricted(true);
			std::string modeMsg = ":server MODE " + channelName + " +t\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else if (mode == "-i") {
			channel->setInviteOnly(false);
			std::string modeMsg = ":server MODE " + channelName + " -i\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else if (mode == "+i") {
			channel->setInviteOnly(true);
			std::string modeMsg = ":server MODE " + channelName + " +i\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else if (mode == "+k") {
			std::string key;
			if (!(iss >> key)) {
				sendReplyTo(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
				return;
			}
			channel->setKeySet(true);
			channel->setKey(key);
			std::string modeMsg = ":server MODE " + channelName + " +k " + key + "\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else if (mode == "-k") {
			channel->setKey("");
			std::string modeMsg = ":server MODE " + channelName + " -k\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else if(mode == "+o") {
			std::string targetNick;
			if (!(iss >> targetNick)) {
				sendReplyTo(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
				return;
			}
			Client *target = findClientByNick(targetNick);
			if (!target) {
				sendReplyTo(client, ERR_NOSUCHNICK, targetNick, "No such nick");
				return;
			}
			if (!channel->hasClient(target)) {
				sendReplyTo(client, ERR_NOTONCHANNEL, channelName, "You're not on that channel");
				return;
			}
			channel->addOperator(target);
			std::string modeMsg = ":server MODE " + channelName + " +o " + targetNick + "\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else if(mode == "-o") {
			std::string targetNick;
			if (!(iss >> targetNick)) {
				sendReplyTo(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
				return;
			}
			Client *target = findClientByNick(targetNick);
			if (!target) {
				sendReplyTo(client, ERR_NOSUCHNICK, targetNick, "No such nick");
				return;
			}
			if (!channel->hasClient(target)) {
				sendReplyTo(client, ERR_NOTONCHANNEL, channelName, "You're not on that channel");
				return;
			}
			channel->removeOperator(target);
			std::string modeMsg = ":server MODE " + channelName + " -o " + targetNick + "\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else if (mode == "+l") {
			int limit;
			if (!(iss >> limit)) {
				sendReplyTo(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
				return;
			}
			std::stringstream ss;
			ss << limit;
			std::string limitStr = ss.str();
			channel->setMaxClients(limit);
			std::string modeMsg = ":server MODE " + channelName + " +l " + limitStr + "\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else if (mode == "-l") {
			channel->setMaxClients(0);
			std::string modeMsg = ":server MODE " + channelName + " -l\r\n";
			channel->broadcastMessage(modeMsg, client);
		}
		else
			sendReplyTo(client, ERR_UNKNOWNMODE, mode, "Unknown mode");
	}
#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>  // Para salida por consola
#include <string>
#include <sstream>
#include <stdlib.h> 

void Server::handlePrivMsg(Client *client, std::istringstream &iss)
{
    if (!client->isRegistered()) {
        std::string errorMsg = ":server 451 * :You have not registered\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::string target;
    if (!(iss >> target)) {
        std::string errorMsg = ":server 461 PRIVMSG :Not enough parameters\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }
    std::string msg;
    getline(iss, msg);

    

    if (msg.find("!bot") != std::string::npos) {
        if (msg.find("!bot hello") != std::string::npos) {
            std::string botResponse = ":Bot PRIVMSG " + target + " :Hello, I am Bot!\r\n";
            Channel *channel = getChannelByName(target);
            if (channel) {
                channel->broadcastMessage(botResponse, NULL);
                std::cout << "Bot responded in channel " << target << std::endl;
            }
        }
        else if (msg.find("!bot help") != std::string::npos) {
            std::string helpMsg = ":Bot PRIVMSG " + target + " :Available commands: !bot help, !bot time, !bot date, !bot stats, !bot rps <rock|paper|scissors>\r\n";
            Channel *channel = getChannelByName(target);
            if (channel) {
                channel->broadcastMessage(helpMsg, NULL);
                std::cout << "Bot provided help in channel " << target << std::endl;
            }
        } 
        else if (msg.find("!bot time") != std::string::npos) {
            time_t now = time(NULL);
            struct tm *lt = localtime(&now);
            char timeStr[64];
            strftime(timeStr, sizeof(timeStr), "%H:%M:%S", lt);
            std::string timeMsg = ":Bot PRIVMSG " + target + " :Current time is " + std::string(timeStr) + "\r\n";
            Channel *channel = getChannelByName(target);
            if (channel) {
                channel->broadcastMessage(timeMsg, NULL);
                std::cout << "Bot provided time in channel " << target << std::endl;
            }
        } 
        else if (msg.find("!bot date") != std::string::npos) {
            time_t now = time(NULL);
            struct tm *lt = localtime(&now);
            char dateStr[64];
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", lt);
            std::string dateMsg = ":Bot PRIVMSG " + target + " :Today's date is " + std::string(dateStr) + "\r\n";
            Channel *channel = getChannelByName(target);
            if (channel) {
                channel->broadcastMessage(dateMsg, NULL);
                std::cout << "Bot provided date in channel " << target << std::endl;
            }
        } 
        else if (msg.find("!bot stats") != std::string::npos) {
            Channel *channel = getChannelByName(target);
            if (channel) {
                size_t count = channel->getClientCount();
                std::ostringstream oss;
                oss << count;
                std::string statsMsg = ":Bot PRIVMSG " + target + " :There are " + oss.str() + " users in channel " + target + "\r\n";
                channel->broadcastMessage(statsMsg, NULL);
                std::cout << "Bot provided stats in channel " << target << std::endl;
            }
        } 
        else if (msg.find("!bot rps") != std::string::npos) {
            std::istringstream rpsStream(msg);
            std::string botCmd, rpsCmd, userMove;
            rpsStream >> botCmd >> rpsCmd >> userMove;
            if (userMove != "rock" && userMove != "paper" && userMove != "scissors") {
                std::string errorMsg = ":Bot PRIVMSG " + target + " :Invalid move. Use rock, paper, or scissors.\r\n";
                Channel *channel = getChannelByName(target);
                if (channel) {
                    channel->broadcastMessage(errorMsg, NULL);
                }
                return;
            }
            srand(time(NULL));
            int botIndex = rand() % 3;
            std::string moves[3] = {"rock", "paper", "scissors"};
            std::string botMove = moves[botIndex];
            std::string result;
            if (userMove == botMove) {
                result = "It's a tie!";
            } else if ((userMove == "rock" && botMove == "scissors") ||
                    (userMove == "paper" && botMove == "rock") ||
                    (userMove == "scissors" && botMove == "paper")) {
                result = "You win!";
            } else {
                result = "I win!";
            }
            std::string rpsMsg = ":Bot PRIVMSG " + target + " :You chose " + userMove + ", I chose " + botMove + ". " + result + "\r\n";
            Channel *channel = getChannelByName(target);
            if (channel) {
                channel->broadcastMessage(rpsMsg, NULL);
                std::cout << "Bot played RPS in channel " << target << std::endl;
            }
        }
        return;
    }
    
    Channel *channel = getChannelByName(target);
    if (channel && channel->hasClient(client) && target[0] == '#') {
        sendToChannel(client, target, msg);
           //std::string broadcast = ":" + client->getNickname() + " " + target + " " + msg + "\r\n";
        //channel->broadcastMessage(broadcast, client);
        std::cout << "Broadcast message from client " << client->getFd() << " to channel " << target << std::endl;
    } else if(findClientByNick(target)) {
		if (findClientByNick(target)->isAway()) {
			std::cout << "teeeeest" << std::endl;
			std::string awayMsg = ":server 301 " + client->getNickname() + " " +
			target + findClientByNick(target)->getAwayMessage() + "\r\n";
			send(client->getFd(), awayMsg.c_str(), awayMsg.size(), 0);
		}
 		sendToUser(client, target, msg);
        std::cout << "Message from client " << client->getFd() << " to user " << target << std::endl;
    } else {
        std::string errorMsg = "Error: you are not in channel " + target + "\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
    }
}
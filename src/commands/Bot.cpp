#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>
#include <sstream>
#include <ctime>
#include <cstdlib>

void Server::handleBotCommand(const std::string &target, const std::string &msg)
{
    Channel *channel = getChannelByName(target);
    if (!channel) return;

    if (msg.find("!bot hello") != std::string::npos) {
        std::string botResponse = ":Bot PRIVMSG " + target + " :Hello, I am Bot!\r\n";
        channel->broadcastMessage(botResponse, NULL);
        std::cout << "Bot responded in channel " << target << std::endl;
    }
    else if (msg.find("!bot help") != std::string::npos) {
        std::string helpMsg = ":Bot PRIVMSG " + target + " :Available commands: !bot help, !bot time, !bot date, !bot stats, !bot rps <rock|paper|scissors>\r\n";
        channel->broadcastMessage(helpMsg, NULL);
        std::cout << "Bot provided help in channel " << target << std::endl;
    }
    else if (msg.find("!bot time") != std::string::npos) {
        time_t now = time(NULL);
        struct tm *lt = localtime(&now);
        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", lt);
        std::string timeMsg = ":Bot PRIVMSG " + target + " :Current time is " + std::string(timeStr) + "\r\n";
        channel->broadcastMessage(timeMsg, NULL);
        std::cout << "Bot provided time in channel " << target << std::endl;
    }
    else if (msg.find("!bot date") != std::string::npos) {
        time_t now = time(NULL);
        struct tm *lt = localtime(&now);
        char dateStr[64];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", lt);
        std::string dateMsg = ":Bot PRIVMSG " + target + " :Today's date is " + std::string(dateStr) + "\r\n";
        channel->broadcastMessage(dateMsg, NULL);
        std::cout << "Bot provided date in channel " << target << std::endl;
    }
    else if (msg.find("!bot stats") != std::string::npos) {
        size_t count = channel->getClientCount();
        std::ostringstream oss;
        oss << count;
        std::string statsMsg = ":Bot PRIVMSG " + target + " :There are " + oss.str() + " users in channel " + target + "\r\n";
        channel->broadcastMessage(statsMsg, NULL);
        std::cout << "Bot provided stats in channel " << target << std::endl;
    }
    else if (msg.find("!bot rps") != std::string::npos) {
        std::istringstream rpsStream(msg);
            std::string botCmd, rpsCmd, userMove;
            rpsStream >> botCmd >> rpsCmd >> userMove;
            if (userMove != "rock" && userMove != "paper" && userMove != "scissors") {
                std::string errorMsg = ":Bot PRIVMSG " + target + " :Invalid move. Use rock, paper, or scissors.\r\n";
                channel->broadcastMessage(errorMsg, NULL);
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
            channel->broadcastMessage(rpsMsg, NULL);
            std::cout << "Bot played RPS in channel " << target << std::endl;     
    }
}

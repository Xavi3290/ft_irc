#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <sys/poll.h>
#include <arpa/inet.h> 
#include <map>           
#include "Client.hpp"
#include "Channel.hpp"

class Server {
    public:
        Server(int port, const std::string &password);
        ~Server();
        
        bool init();
        void run();

    private:
        int _port;
        std::string _password;
        int _listenFd;
        struct sockaddr_in _server_addr;
        socklen_t _addrlen;
        std::vector<pollfd> _pollFds;
        std::vector<Client*> _clients;
        std::vector<Channel*> _channels;
        
        bool setNonBlocking(int fd);
        bool setupSocket();
        void handleNewConnection();
        void handleClientData(size_t i);
        
        Client* findClientByFd(int fd);
        Client* findClientByNick(const std::string &nickname);
        void removeClient(int fd);
        void parseCommand(Client *client, const std::string &message);
        
        Channel *getChannelByName(const std::string &name);
		void removeClientChannel(int fd);
        
        void sendToChannel(Client *sender, const std::string &chanelName, const std::string &message);
        void sendToUser(Client *sender, const std::string &targetNick, const std::string &message);
        void sendToAll(Client *sender, const std::string &message);

        void sendReplyTo(Client *client, int code, const std::string &params = "", const std::string &message = "");

        typedef void (Server::*CommandHandler)(Client* client, std::istringstream&);
        std::map<std::string, CommandHandler> _handlers;
        
        void handlePass(Client *client, std::istringstream &iss);
        void handleNick(Client *client, std::istringstream &iss);
        void handleUser(Client *client, std::istringstream &iss);
        void handlePing(Client *client, std::istringstream &iss);
        void handleJoin(Client *client, std::istringstream &iss);
        void handlePrivMsg(Client *client, std::istringstream &iss);
        void handlePart(Client *client, std::istringstream &iss);
        void handleFile(Client *client, std::istringstream &iss);
        void handleList(Client *client, std::istringstream &iss);
        void handleNames(Client *client, std::istringstream &iss);
        void handleKick(Client *client, std::istringstream &iss);
        void handleTopic(Client *client, std::istringstream &iss);
        void handleQuit(Client *client, std::istringstream &iss);
		void handleWho(Client *client, std::istringstream &iss);
		void handleMode(Client *client, std::istringstream &iss);
		void handleInvite(Client *client, std::istringstream &iss);
		void handleWhois(Client *client, std::istringstream &iss);
		void handleAway(Client *client, std::istringstream &iss);
		void handleNotice(Client *client, std::istringstream &iss);
        void handleBotCommand(const std::string &target, const std::string &msg);
};

#endif

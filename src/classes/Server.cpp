#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"
#include "../../inc/Utils.hpp"

#include <iostream>  // Para salida por consola
#include <cstdlib>    // Para atoi() y EXIT_SUCCESS/EXIT_FAILURE
#include <cstring>    // Para memset()
#include <cerrno>      // Para manejo de errores
#include <unistd.h>  // Para close(), read()
#include <fcntl.h>    // Para fcntl()
#include <sys/types.h>
#include <sys/socket.h> // Para socket(), bind(), listen(), accept(), setsockopt(), send(), recv()
#include <netinet/in.h> // Para sockaddr_in y htons()
#include <cstdio>      // Para perror()
#include <sstream> 
#include <set>


Server::Server(int port, const std::string &password) : _port(port), _password(password), _listenFd(-1) {

    _handlers["PASS"]    = &Server::handlePass;
    _handlers["NICK"]    = &Server::handleNick;
    _handlers["USER"]    = &Server::handleUser;
    _handlers["PING"]    = &Server::handlePing;
    _handlers["JOIN"]    = &Server::handleJoin;
    _handlers["PRIVMSG"] = &Server::handlePrivMsg;
    _handlers["PART"]    = &Server::handlePart;
    _handlers["LIST"]    = &Server::handleList;
    _handlers["NAMES"]   = &Server::handleNames;
    _handlers["KICK"]    = &Server::handleKick;
    _handlers["TOPIC"]   = &Server::handleTopic;
    _handlers["QUIT"]    = &Server::handleQuit;
    _handlers["WHO"]     = &Server::handleWho;
    _handlers["MODE"]    = &Server::handleMode;
    _handlers["INVITE"]  = &Server::handleInvite;
    _handlers["WHOIS"]   = &Server::handleWhois;
    _handlers["whois"]   = &Server::handleWhois;
    _handlers["AWAY"]    = &Server::handleAway;
	_handlers["NOTICE"]  = &Server::handleNotice;
}

Server::~Server()
{
    if (_listenFd != -1)
        close(_listenFd);
		for (size_t i = 1; i < _pollFds.size(); i++) {
			close(_pollFds[i].fd);
		}
		for (size_t i = 0; i < _clients.size(); i++) {
			delete _clients[i];
		}
		for(size_t i = 0; i < _channels.size(); i++) {
			delete _channels[i];   
		}
}
	
// SENDERS TYPES
void Server::sendReplyTo(Client *client, int code, const std::string &params, const std::string &message)
{
	std::string reply = NumericReplies::reply(code, client->getNickname(), params, message);
	send(client->getFd(), reply.c_str(), reply.length(), 0);
}

void Server::sendToChannel(Client *sender, const std::string &channelName, const std::string &message)
{
    Channel *channel = getChannelByName(channelName);
    if (!channel)
    {
        sendReplyTo(sender, ERR_NOSUCHCHANNEL, channelName, "No such channel");
        return;
    }

    if (!channel->hasClient(sender))
    {
        sendReplyTo(sender, ERR_CANNOTSENDTOCHAN, channelName, "Cannot send to channel");
        return;
    }

    std::string fullMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@host PRIVMSG " + channelName + message + "\r\n";
    channel->broadcastMessage(fullMsg, sender);
}

void Server::sendToUser(Client *sender, const std::string &targetNick, const std::string &message)
{
    Client *receiver = findClientByNick(targetNick);
    if (!receiver) {
        sendReplyTo(sender, ERR_NOSUCHNICK, targetNick, "No such nick");
        return;
    }

    std::string fullMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@localhost PRIVMSG " + receiver->getNickname() + message + "\r\n";
    send(receiver->getFd(), fullMsg.c_str(), fullMsg.size(), 0);
}

void Server::sendToAll(Client *sender, const std::string &message)
{
    std::string fullMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@localhost NOTICE * :" + message + "\r\n";
    for (size_t i = 0; i < _clients.size(); i++) {
        Client *target = _clients[i];
        if (target != sender)
            send(target->getFd(), fullMsg.c_str(), fullMsg.size(), 0);
    }
}

Client* Server::findClientByNick(const std::string &nickname) {
    for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i]->getNickname() == nickname) {
            return _clients[i];
        }
    }
    return NULL;
}


bool Server::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl(F_GETFL)");
        return false;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl(F_SETFL)");
        return false;
    }
    return true;
}


bool Server::setupSocket() {
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenFd < 0) {
        perror("socket");
        return false;
    }
        
    // Permitir reutilizar la dirección
    int opt = 1;
    if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(_listenFd);
        return false;
    }

    // Configurar el socket en modo no bloqueante
    if (!setNonBlocking(_listenFd)) {
        close(_listenFd);
        return false;
    }

    // Configurar la direccion del servidor (IP: cualquier dirección local, puerto especificado)
    _addrlen = sizeof(_server_addr);
    std::memset(&_server_addr, 0, _addrlen);
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    _server_addr.sin_port = htons(_port);

    if (bind(_listenFd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
        perror("bind");
        close(_listenFd);
        return false;
    }
        
    // Poner el socket a escuchar conexiones entrantes
    if (listen(_listenFd, SOMAXCONN) < 0) {
        perror("listen");
        close(_listenFd);
        return false;
    }

    // Inicializar poll() con el socket de escucha
    struct pollfd listen_poll;
    listen_poll.fd = _listenFd;
    listen_poll.events = POLLIN; // Monitorizar para ver si hay datos entrantes (nuevas conexiones)
    _pollFds.push_back(listen_poll);

    std::cout << "Server IRC listening on port " << _port << std::endl;
    return true;
}

bool Server::init() {
    return setupSocket();
}

void Server::handleNewConnection() {
    //struct sockaddr_in client_addr;
    //socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(_listenFd, (struct sockaddr *)&_server_addr, &_addrlen);
    if (client_fd >= 0) {
        if (!setNonBlocking(client_fd)) {
            close(client_fd);
            return;
        }
        struct pollfd client_poll;
        client_poll.fd = client_fd;
        client_poll.events = POLLIN; // Monitorizar para ver si hay datos entrantes
        _pollFds.push_back(client_poll);

        Client *newClient = new Client(client_fd);
		std::string ip = inet_ntoa(_server_addr.sin_addr);
		newClient->setIP(ip);
        _clients.push_back(newClient);

        std::cout << "New connection accepted: fd = " << client_fd << std::endl;
    } else {
        perror("accept");
    }
}

Client *Server::findClientByFd(int fd) {
    for (size_t i =0; i < _clients.size(); i++) {
        if (_clients[i]->getFd() == fd) {
            return _clients[i];
        }
    }
    return NULL;
}

void Server::removeClient(int fd) {
    // for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
    // {
    //     if (it->fd == fd)
    //     {
    //         _pollFds.erase(it);
    //         break;
    //     }
    // }

    // delete _clients[it];
    // _clients.erase(fd);
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i]->getFd() == fd) {
			close(_clients[i]->getFd());
            delete _clients[i];
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
}

void Server::removeClientChannel(int fd) {
	for (size_t i = 0; i < _channels.size(); i++) {
		Channel *channel = _channels[i];
		for (size_t j = 0; j < channel->getClients().size(); j++) {
			if (channel->getClients()[j]->getFd() == fd) {
				channel->removeClient(channel->getClients()[j]);
				if (channel->getClients().empty()) {
					delete channel;
					_channels.erase(_channels.begin() + i);
				}
				else
					channel->broadcastMessage(":" + channel->getClients()[j]->getNickname() + " PART " + channel->getOriginalName() + "\r\n", channel->getClients()[j]);
				break;
			}
		}
	}
}

void Server::parseCommand(Client *client, const std::string &message) {
    std::istringstream iss(message);
    std::string command;
    iss >> command;


	std::cout << "Command: " << message << std::endl;
    std::map<std::string, CommandHandler>::iterator it = _handlers.find(command);
    if (it != _handlers.end()) {
        CommandHandler handler = it->second;
        (this->*handler)(client, iss);
    } else {
		sendReplyTo(client, ERR_UNKNOWNCOMMAND, command, "Unknown command");
        std::cout << "Unknown command from client " << client->getFd() << ": " << message << std::endl;
    }
}

void Server::handleClientData(size_t i)
{
    if (_pollFds[i].revents & POLLIN)
    {
        char buffer[1024];
        int bytes_read = recv(_pollFds[i].fd, buffer, sizeof(buffer), 0);
        if (bytes_read > 0)
        {
            Client *client = findClientByFd(_pollFds[i].fd);
            if (client) {
                std::string message(buffer, bytes_read);
                client->appendBuffer(message);

                std::string &fullBuffer = client->getBuffer();
				std::cout << "Full Buffer: " << fullBuffer << std::endl;
                size_t pos;
				int i = 0;
                while ((pos = fullBuffer.find("\n")) != std::string::npos) {
					i++;
					std::cout << "interadir : " << i << std::endl;
                    std::string rawCommand = fullBuffer.substr(0, pos);
                    fullBuffer.erase(0, pos + 1);
                    parseCommand(client, rawCommand);
                }
            }
        }
        else if (bytes_read == 0)
        {
            std::cout << "Client " << _pollFds[i].fd << " disconnected" << std::endl;
            close(_pollFds[i].fd);
            removeClient(_pollFds[i].fd);
            _pollFds.erase(_pollFds.begin() + i);
            i--;
        }
        else
        {
			if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                perror("recv");
                close(_pollFds[i].fd);
                removeClient(_pollFds[i].fd);
                _pollFds.erase(_pollFds.begin() + i);
                i--;
            }
        }
    }
}

void Server::run() {
    while (42) {
        int poll_count = poll(&_pollFds[0], _pollFds.size(), -1);
        if (poll_count < 0) {
            perror("poll");
            break;
        }
        
        // Revisar si el socket de escucha tiene nuevas conexiones
        if (_pollFds[0].revents & POLLIN) {
            handleNewConnection();
        }

        // Procesar cada cliente conectado (empezando desde el índice 1)
        for (size_t i = 1; i < _pollFds.size(); i++) {
            if (_pollFds[i].revents & POLLIN) {
                handleClientData(i);
                // Dado que el vector _pollFds puede modificarse (al eliminar un cliente desconectado), reajustamos el índice
                //i--;
            }
        }
    }  
}

Channel *Server::getChannelByName(const std::string &name) {
    std::string lowerName = toLower(name);
    for (size_t i = 0; i < _channels.size(); i++) {
        if (_channels[i]->getLowerName() == lowerName) {
            return _channels[i];
        }
    }
    return NULL;
}
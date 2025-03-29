/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 19:15:50 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/27 18:07:04 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/NumericReplies.hpp"

#include <iostream>     // Para salida por consola
#include <cstdlib>      // Para atoi() y EXIT_SUCCESS/EXIT_FAILURE
#include <cstring>      // Para memset()
#include <cerrno>       // Para manejo de errores
#include <unistd.h>     // Para close(), read()
#include <fcntl.h>      // Para fcntl()
#include <sys/types.h>
#include <sys/socket.h> // Para socket(), bind(), listen(), accept(), setsockopt(), send(), recv()
#include <netinet/in.h> // Para sockaddr_in y htons()
#include <cstdio>       // Para perror()
#include <sstream> 

Server::Server(int port, const std::string &password) : _port(port), _password(password), _listenFd(-1) {}

Server::~Server() {
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

void Server::sendToChannel(Client *sender, const std::string &channelName, const std::string &message)
{
    Channel *channel = getChannelByName(channelName);
    if (!channel)
    {
        std::string errorMsg = NumericReplies::reply(ERR_NOSUCHCHANNEL, sender->getNickname(), channelName);
        send(sender->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    if (!channel->hasClient(sender))
    {
        std::string errorMsg = NumericReplies::reply(ERR_CANNOTSENDTOCHAN, sender->getNickname(), channelName);
        send(sender->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    std::string fullMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@localhost PRIVMSG " + channelName + " :" + message + "\r\n";
    channel->broadcastMessage(fullMsg, sender);
}

void Server::sendToUser(Client *sender, const std::string &targetNick, const std::string &message)
{
    Client *receiver = findClientByNick(targetNick);
    if (!receiver) {
        std::string errorMsg = NumericReplies::reply(ERR_NOSUCHNICK, sender->getNickname(), targetNick);
        send(sender->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    std::string fullMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@localhost PRIVMSG " + receiver->getNickname() + " :" + message + "\r\n";
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
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i]->getFd() == fd) {
            delete _clients[i];
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
}

void Server::parseCommand(Client *client, const std::string &message) {
    std::istringstream iss(message);
    std::string command;
    iss >> command;


    if (command != "PASS" && !client->hasProvidedPass()) {
        std::string errorMsg = ":server 451 * :You have not registered\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    if (command == "PASS") {
        std::string pass;
        if (!(iss >> pass)) {
            std::string errorMsg = ":server 461 PASS :Not enough parameters\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        if (pass != _password) {
            std::string errorMsg = ":server 464 * :Password incorrect\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            std::cout << "Client " << client->getFd() << " provided invalid password." << std::endl;
            close(client->getFd());
            removeClient(client->getFd());
            return;
        } else {
            client->setPassProvided(true);
            std::cout << "Client " << client->getFd() << " provided valid password." << std::endl;
        }
    } 
    else if (command == "NICK") {
        std::string newNick;
        if (!(iss >> newNick)) {
            std::string errorMsg = ":server 431 * :No nickname given\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        client->setNickname(newNick);
        std::cout << "Client " << client->getFd() << " set nickname to " << newNick << std::endl;
        if (!client->getNickname().empty() && !client->getUsername().empty() && client->hasProvidedPass() && !client->isRegistered()) {
            std::string welcome = ":server 001 " + client->getNickname() + " :Welcome to the IRC server!\r\n";
            send(client->getFd(), welcome.c_str(), welcome.size(), 0);
            client->setRegistered(true);
            std::cout << "Client " << client->getFd() << " registered." << std::endl;
        }
    }
    else if (command == "USER") {
        std::string username;
        if (!(iss >> username)) {
            std::string errorMsg = ":server 461 USER :Not enough parameters\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        client->setUsername(username);
        std::cout << "Client " << client->getFd() << " set username to " << username << std::endl;
        if (!client->getNickname().empty() && !client->getUsername().empty() && client->hasProvidedPass() && !client->isRegistered()) {
            std::string welcome = ":server 001 " + client->getNickname() + " :Welcome to the IRC server!\r\n";
            send(client->getFd(), welcome.c_str(), welcome.size(), 0);
            client->setRegistered(true);
            std::cout << "Client " << client->getFd() << " registered." << std::endl;
        }
    }
    else if (command == "PING") {
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
    else if (command == "JOIN") {
        std::string channelName;
        if (!(iss >> channelName))
        {
            std::string errorMsg = NumericReplies::reply(ERR_NEEDMOREPARAMS, client->getNickname(), command);
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        handleJoin(client, channelName);
    }
    else if (command == "PRIVMSG") {
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
        if (channel && channel->hasClient(client)) {
            std::string broadcast = ":" + client->getNickname() + " " + target + " " + msg + "\r\n";
            channel->broadcastMessage(broadcast, client);
            std::cout << "Broadcast message from client " << client->getFd() << " to channel " << target << std::endl;
        } else {
            std::string errorMsg = "Error: you are not in channel " + target + "\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        }
    }
    else if (command == "PART") {
        if (!client->isRegistered()) {
            std::string errorMsg = "451 :You have not registered\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        std::string channelName;
        iss >> channelName;
        if (channelName.empty()) {
            std::string errorMsg = "461 PART :Not enough parameters\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        Channel *channel = getChannelByName(channelName);
        if (channel && channel->hasClient(client)) {
            channel->removeClient(client);
            std::string partMsg = "You have left channel " + channelName + "\r\n";
            send(client->getFd(), partMsg.c_str(), partMsg.size(), 0);
            std::cout << "Client " << client->getFd() << " left channel " << channelName << std::endl;
            std::string broadcast = ":" + client->getNickname() + " has left channel " + channelName + "\r\n";
            channel->broadcastMessage(broadcast, client);
        } else {
            std::string errorMsg = "Error: you are not in channel " + channelName + "\r\n";
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        }
    }
    else if (command == "FILE") {
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
    else {
        std::string errorMsg = ":server 421 " + command + " :Unknown command\r\n";
        send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
        std::cout << "Unknown command from client " << client->getFd() << ": " << message;
    }
}

void Server::handleJoin(Client *client, const std::string &channelName)
{
    if (!client->isRegistered()) {
            std::string errorMsg = NumericReplies::reply(ERR_NOTREGISTERED, client->getNickname());
            send(client->getFd(), errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }

        Channel *channel = getChannelByName(channelName);
        if (!channel) {
            channel = new Channel(channelName);
            _channels.push_back(channel);
        }
        if (!channel->hasClient(client))
            channel->addClient(client);
    
        std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + channelName + "\r\n";
        channel->broadcastMessage(joinMsg, NULL);
        
        //SI NO HAY TOPIC
        std::string topicMsg = NumericReplies::reply(RPL_NOTOPIC, client->getNickname(), channelName);
        send(client->getFd(), topicMsg.c_str(), topicMsg.size(), 0);

        //LISTAR USUARIOS
        const std::vector<Client *> &clients = channel->getClients();
        std::string nameList;
        for (size_t i = 0; i < clients.size(); ++i)
            nameList += clients[i]->getNickname() + " ";

        std::string namesReply = ":irc.42.localhost 353 " + client->getNickname() + " = " + channelName + " :" + nameList + "\r\n";
        send(client->getFd(), namesReply.c_str(), namesReply.size(), 0);

        // 4. Fin de lista (RPL_ENDOFNAMES)
        std::string endMsg = NumericReplies::reply(RPL_ENDOFNAMES, client->getNickname(), channelName);
        send(client->getFd(), endMsg.c_str(), endMsg.size(), 0);
        
        //ANTIGUO MODIFICADO

        // std::string joinMsg = "You have joined channel " + channelName + "\r\n";

        //AHORA HexChat CREA LOS CANALES CORRECTAMENTE
        // FALTA IMPLEMENTAR LISTA DE USUARIOS <---- NO SE SI ES NECESARIO
        
        // std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost JOIN :" + channelName + "\r\n";
        // send(client->getFd(), joinMsg.c_str(), joinMsg.size(), 0);
        // std::string joinMsg2 = ":irc.42.localhost 332" + client->getUsername() + channelName + ":posible TOPIC si hay\r\n";
        // send(client->getFd(), joinMsg2.c_str(), joinMsg2.size(), 0);
        // std::cout << "Client " << client->getFd() << " joined channel " << channelName << std::endl;
}

void Server::handleClientData(size_t i)
{
    if (_pollFds[i].revents & POLLIN)
    {
        char buffer[1024];
        int bytes_read = recv(_pollFds[i].fd, buffer, sizeof(buffer), 0);
        if (bytes_read > 0)
        {
            //buffer[bytes_read] = '\0';
            std::string message(buffer, bytes_read);
            Client *client = findClientByFd(_pollFds[i].fd);
            if (client)
                parseCommand(client, message);
            else
                std::cout << "Recived data from unknown client fd: " << _pollFds[i].fd << std::endl;   //sin std::endl para no hacer flush?
            // Aquí se puede añadir el parseo y la gestión de comandos IRC
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
    for (size_t i = 0; i < _channels.size(); i++) {
        if (_channels[i]->getName() == name) {
            return _channels[i];
        }
    }
    return NULL;
}

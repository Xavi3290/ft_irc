/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 19:15:50 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/18 19:41:58 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

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
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_port);

    if (bind(_listenFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("blind");
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
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(_listenFd, (struct sockaddr *)&client_addr, &client_len);
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

    if (command == "NICK") {
        std::string newNick;
        iss >> newNick;
        client->setNickname(newNick);
        std::cout << "Client " << client->getFd() << " set nickname to " << newNick << std::endl;
    }
    else if (command == "USER") {
        std::string username;
        iss >> username;
        client->setUsername(username);
        std::cout << "Client " << client->getFd() << " set username to " << username << std::endl;
        if (!client->getNickname().empty() && !client->getUsername().empty()) {
            std::string welcome = "Welcome to the IRC server!\r\n";
            send(client->getFd(), welcome.c_str(), welcome.length(), 0);
            client->setRegistered(true);
            std::cout << "Client " << client->getFd() << " registered." << std::endl;
        }
    }
    else if (command == "PING") {
        std::string parameter;
        iss >> parameter;
        std::string pong = "PONG " + parameter + "\r\n";
        send(client->getFd(), pong.c_str(), pong.size(), 0);
        std::cout << "Responding with: " << pong;
    }
    else if (command == "JOIN") {
        std::string channelName;
        iss >> channelName;
        Channel *channel = getChannelByName(channelName);
        if (!channel) {
            channel = new Channel(channelName);
            _channels.push_back(channel);
        }
        channel->addClient(client);
        std::string joinMsg = "You have joined channel " + channelName + "\r\n";
        send(client->getFd(), joinMsg.c_str(), joinMsg.size(), 0);
        std::cout << "Client " << client->getFd() << " joined channel " << channelName << std::endl;
    }
    else if (command == "PRIVMSG") {
        std::string target;
        iss >> target;
        std::string msg;
        getline(iss, msg);
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
    else {
        std::cout << "Unknown command from client " << client->getFd() << ": " << message;
    }
}

void Server::handleClientData(size_t index) {
    char buffer[1024];
    size_t bytes_read = read(_pollFds[index].fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::string message(buffer);
        Client *client = findClientByFd(_pollFds[index].fd);
        if (client)
            parseCommand(client, message);
        else
            std::cout << "Recived data from unknown client fd: " << _pollFds[index].fd << std::endl;   //sin std::endl para no hacer flush?
        // Aquí se puede añadir el parseo y la gestión de comandos IRC
    } else if (bytes_read == 0) {
        std::cout << "Client " << _pollFds[index].fd << " disconnected" << std::endl;
        close(_pollFds[index].fd);
        removeClient(_pollFds[index].fd);
        _pollFds.erase(_pollFds.begin() + index);
    } else  {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            perror("read");
            close(_pollFds[index].fd);
            removeClient(_pollFds[index].fd);
            _pollFds.erase(_pollFds.begin() + index);
        }
    }
}

void Server::run() {
    while (42) {
        int poll_count = poll(&_pollFds[0], _pollFds.size(), -1);
        if (poll_count < 0) {
            perror("poll");
            continue;
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
                i--;
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
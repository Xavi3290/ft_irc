/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 19:09:19 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/18 18:04:21 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <sys/poll.h> // Para struct pollfd y la función poll()
#include "Client.hpp"

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
        std::vector<struct pollfd> _pollFds;
        std::vector<Client*> _clients;

        bool setNonBlocking(int fd);
        bool setupSocket();
        void handleNewConnection();
        void handleClientData(size_t index);

        Client* findClientByFd(int fd);
        void removeClient(int fd);
        void parseCommand(Client *client, const std::string &message);
};

#endif
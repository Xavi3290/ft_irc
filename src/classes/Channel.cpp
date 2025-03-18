/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 19:04:58 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/18 19:16:56 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"
#include <algorithm>
#include <sys/socket.h>
#include <iostream>

Channel::Channel(const std::string &name) : _name(name) {}

Channel::~Channel() {
    _clients.clear();
}

const std::string &Channel::getName() const {
    return _name;
}

void Channel::addClient(Client *client) {
    if (!hasClient(client)) {
        _clients.push_back(client);
    }
}

void Channel::removeClient(Client *client) {
    _clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
}

bool Channel::hasClient(Client *client) const {
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] == client) {
            return true;
        }
    }
    return false;
}

void Channel::broadcastMessage(const std::string &message, Client *sender) {
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] != sender) {
            send(_clients[i]->getFd(), message.c_str(), message.size(), 0);
        }
    }
}
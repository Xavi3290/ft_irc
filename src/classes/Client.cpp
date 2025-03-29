/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 22:01:09 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/26 19:51:12 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client.hpp"

Client::Client(int fd)
    : _fd(fd), _nickname(""), _username(""), _registered(false), _passProvided(false) {}

Client::~Client() {}

int Client::getFd() const {
    return _fd;
}

const std::string& Client::getNickname() const {
    return _nickname;
}

void Client::setNickname(const std::string &nick) {
    _nickname = nick;
}

const std::string& Client::getUsername() const {
    return _username;
}

void Client::setUsername(const std::string &username) {
    _username = username;
}

bool Client::isRegistered() const {
    return _registered;
}

void Client::setRegistered(bool reg) {
    _registered = reg;
}

bool Client::hasProvidedPass() const {
    return _passProvided;
}

void Client::setPassProvided(bool provided) {
    _passProvided = provided;
}

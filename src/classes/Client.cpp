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
#include <sys/socket.h> // send()
#include <iostream>

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

const std::string& Client::getRealname() const {
    return _realname;
}

void Client::setRealname(const std::string &realname) {
    _realname = realname;
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

std::string Client::getPrefix() const {
    return _nickname + "!" + _username + "@localhost";
}

bool Client::isAway() const {
	return _isAway;
}

void Client::setAway(bool away) {
	_isAway = away;
}

void Client::setAwayMessage(const std::string &message) {
	_awayMessage = message;
}

const std::string &Client::getAwayMessage() const {
	return _awayMessage;
}

void Client::send(const std::string& message) {
    std::string msg = message;

    if (msg.size() < 2 || msg.substr(msg.size() - 2) != "\r\n")
        msg += "\r\n";

    ssize_t bytesSent = ::send(_fd, msg.c_str(), msg.size(), 0);
    if (bytesSent == -1) {
        std::cout << "Error enviando mensaje al cliente (" << _fd << "): " << std::endl;
    }
}

void Client::setIP(const std::string &ip) {
	_ip = ip;
}

const std::string &Client::getIP() const {
	return _ip;
}
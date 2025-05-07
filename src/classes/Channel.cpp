/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 19:04:58 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/27 17:51:59 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"
#include "../../inc/Utils.hpp"
#include <algorithm>
#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include <cctype>   // para std::tolower

Channel::Channel(const std::string &name) {
	_originalName = name;
	_topicRestricted  = true;
	_inviteOnly = false;
	_keySet = false;
	_maxClients = 0;
    _lowerName = toLower(name);
}

Channel::~Channel() {
    _clients.clear();
}

const std::string &Channel::getOriginalName() const {
    return _originalName;
}

const std::string &Channel::getLowerName() const {
    return _lowerName;
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

size_t Channel::getClientCount() const {
    return _clients.size();
}

const std::vector<Client*> &Channel::getClients()
{
    return _clients;
}

const std::vector<Client*> &Channel::getOperators()
{
	return _operators;
}

bool Channel::isOperator(Client *client) const
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client)
			return true;
	}
	return false;
}

std::string Channel::getTopic() const
{
	return _topic;
}

void Channel::addOperator(Client *client)
{
	if (!isOperator(client))
		_operators.push_back(client);
}

void Channel::removeOperator(Client *client)
{
	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}

void Channel::setTopicRestricted(bool topicRestricted)
{
	_topicRestricted = topicRestricted;
}

bool Channel::isTopicRestricted() const
{
	return _topicRestricted;
}

void Channel::setInviteOnly(bool inviteOnly)
{
	_inviteOnly = inviteOnly;
}

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

void Channel::setKey(const std::string &key)
{
	_key = key;
}

const std::string &Channel::getKey() const
{
	return _key;
}

bool Channel::isKeySet() const
{
	return _keySet;
}

void Channel::setKeySet(bool keySet)
{
	_keySet = keySet;
}

void Channel::setMaxClients(int maxClients)
{
	_maxClients = maxClients;
}

int Channel::getMaxClients() const
{
	return _maxClients;
}

bool Channel::isFull() const
{
	if (_maxClients == 0)
		return false;
	return _clients.size() >= static_cast<size_t>(_maxClients);
}

std::string Channel::getMode(Channel *cha, Client *client) const
{
	std::string modes;

	if (_inviteOnly)
		modes += "i";
	if (_keySet)
		modes += "k";
	if (_topicRestricted)
		modes += "t";
	if (_maxClients > 0)
		modes += "l";
	if (!modes.empty())
		modes = "+" + modes;
	if (_maxClients > 0) {
		std::stringstream ss;
		ss << cha->getMaxClients();
		std::string maxClientsStr = ss.str();
		modes += " " + maxClientsStr;
	}
	if (_keySet && cha->isOperator(client))
		modes += " " + _key;
	return modes;
}

void Channel::addInvited(Client *client)
{
	if (!isInvited(client))
		_invited.push_back(client);
}

void Channel::removeInvited(Client *client)
{
	_invited.erase(std::remove(_invited.begin(), _invited.end(), client), _invited.end());
}

bool Channel::isInvited(Client *client) const
{
	for (size_t i = 0; i < _invited.size(); i++)
	{
		if (_invited[i] == client)
			return true;
	}
	return false;
}
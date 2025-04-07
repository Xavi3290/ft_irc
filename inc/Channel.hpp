/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 19:04:16 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/27 17:51:43 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel {
    public:
        Channel(const std::string &name);
        ~Channel();
        
        const std::string &getName() const;
        void addClient(Client *client);
        void removeClient(Client *client);
        bool hasClient(Client *client) const;
        void broadcastMessage(const std::string &message, Client *sender);
        size_t getClientCount() const;
        const std::vector<Client*> &getClients();
		const std::vector<Client*> &getOperators();
		bool isOperator(Client *client) const;
		std::string getTopic() const;
		void addOperator(Client *client);
		void removeOperator(Client *client);
		
		void setTopic(const std::string &topic);
		
		void setTopicRestricted(bool topicRestricted);
		bool isTopicRestricted() const;
		
		void setInviteOnly(bool inviteOnly);
		bool isInviteOnly() const;

		void setKey(const std::string &key);
		const std::string &getKey() const;

		void setMaxClients(int maxClients);
		int getMaxClients() const;

    private:
        std::string _name;
		std::string _topic;
        std::vector<Client*> _clients;
		std::vector<Client*> _operators;
		bool _inviteOnly;
		bool _topicRestricted;
		std::string _key;
		int _maxClients;

};

#endif

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Client;

class Channel {
    public:
        Channel(const std::string &name);
        ~Channel();
        
        const std::string &getOriginalName() const;
        const std::string &getLowerName() const;
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
		bool isKeySet() const;
		void setKeySet(bool keySet);

		void setMaxClients(int maxClients);
		int getMaxClients() const;
		bool isFull() const;

		std::string getMode(Channel *cha, Client *client) const;

		void addInvited(Client *client);
		void removeInvited(Client *client);
		bool isInvited(Client *client) const;

    private:
        std::string _originalName;
		std::string _lowerName;
		std::string _topic;
        std::vector<Client*> _clients;
		std::vector<Client*> _operators;
		std::vector<Client*> _invited;
		bool _inviteOnly;
		bool _topicRestricted;
		bool _keySet;
		std::string _key;
		int _maxClients;

};

#endif

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

    private:
        std::string _name;
        std::vector<Client*> _clients;
};

#endif

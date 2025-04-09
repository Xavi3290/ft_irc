/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 22:01:05 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/26 19:46:20 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
    public:
        Client(int fd);
        ~Client();
        
        int getFd() const;
        const std::string& getNickname() const;
        void setNickname(const std::string &nick);
        const std::string &getUsername()const;
        void setUsername(const std::string &username);
        bool isRegistered() const;
        void setRegistered(bool registered);
        bool hasProvidedPass() const;
        void setPassProvided(bool provided);
		bool isAway() const;
		void setAway(bool away);
		void setAwayMessage(const std::string &message);
		const std::string &getAwayMessage() const;

        std::string getPrefix() const;


    private:
        int _fd;
        std::string _nickname;
        std::string _username;
        bool _registered;
        bool _passProvided;
		bool _isAway;
		std::string _awayMessage;
};

#endif

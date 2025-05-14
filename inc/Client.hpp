#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include "Channel.hpp"

class Client {
    public:
        Client(int fd);
        ~Client();
        
        int getFd() const;
        const std::string& getNickname() const;
        void setNickname(const std::string &nick);
        const std::string &getUsername()const;
        void setUsername(const std::string &username);
        const std::string &getRealname() const;
        void setRealname(const std::string &realname);
        bool isRegistered() const;
        void setRegistered(bool registered);
        bool hasProvidedPass() const;
        void setPassProvided(bool provided);
		bool isAway() const;
		void setAway(bool away);
		void setAwayMessage(const std::string &message);
		const std::string &getAwayMessage() const;
		const std::string &getIP() const;
		void setIP(const std::string &ip);

        std::string getPrefix() const;
        void send(const std::string &message);

        void appendBuffer(const std::string &data);
        std::string &getBuffer();


    private:
        int _fd;
        std::string _nickname;
        std::string _username;
        //bool _hasUser;
        std::string _realname;
        bool _registered;
        bool _passProvided;
		bool _isAway;
		std::string _awayMessage;
		std::string _ip;

        std::string _buffer;
};

#endif

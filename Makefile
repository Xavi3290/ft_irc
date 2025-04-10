NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 
SRCS =  src/main.cpp \
		src/classes/Server.cpp \
		src/classes/Client.cpp \
		src/classes/Channel.cpp \
		src/classes/NumericReplies.cpp \
		src/commands/Pass.cpp \
		src/commands/Nick.cpp \
		src/commands/User.cpp \
		src/commands/Ping.cpp \
		src/commands/Join.cpp \
		src/commands/PrivMsg.cpp \
		src/commands/Part.cpp \
		src/commands/File.cpp \
		src/commands/List.cpp \
		src/commands/Names.cpp \
		src/commands/Kick.cpp \
		src/commands/Topic.cpp \
		src/commands/Quit.cpp \

		
INCLUDES = inc/Server.hpp inc/Client.hpp inc/Channel.hpp inc/NumericReplies.hpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

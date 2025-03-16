SRC_DIR = src/
OBJ_DIR = obj/
INC_DIR = inc/

NAME = ircserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CDEPS = -MMD -MP

CALL = $(CXXFLAGS) $(CDEPS)

SRCS =		src/main.cpp \
#			src/main.cpp \

OBJS = $(SRCS:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)
DEPS = $(OBJS:.o=.d)

HEADERS = 	#inc/BitcoinExchange.hpp \


all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CALL) $(OBJS) -o $(NAME)


$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp Makefile
	$(CXX) $(CALL) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@


clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re

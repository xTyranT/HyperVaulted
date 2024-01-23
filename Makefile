CPP = c++

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g3

SRCSDIR = ./srcs

SRC_FILES = Server.cpp webserv.cpp stringManipulators.cpp

SRCS = $(wildcard $(SRCSDIR)/*.cpp)

OBJSDIR = ./objs

OBJS = $(addprefix $(OBJSDIR)/, $(SRC_FILES:.cpp=.o))

NAME = HyperVaulted

all : $(NAME)

$(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(NAME) : $(OBJS)
	$(CPP) $(CPPFLAGS) $(SRCS) -o $(NAME)

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re
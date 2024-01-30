CPP = c++

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g3

SRCSDIR = ./srcs

UTILSDIR = ./utils

OBJSDIR = ./objs

NAME = HyperVaulted

SRC_FILES = Server.cpp webserv.cpp Multiplexing.cpp 

UTIL_FILES = stringManipulators.cpp

SRCS = $(wildcard $(SRCSDIR)/*.cpp)

OBJS = $(addprefix $(OBJSDIR)/, $(SRC_FILES:.cpp=.o))

UTILS_OBJS = $(addprefix $(OBJSDIR)/, $(UTIL_FILES:.cpp=.o))

all : $(NAME)

$(OBJSDIR):
	mkdir -p $(OBJSDIR)

$(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp | $(OBJSDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJSDIR)/%.o : $(UTILSDIR)/%.cpp | $(OBJSDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(NAME) : $(OBJS) $(UTILS_OBJS)
	$(CPP) $(CPPFLAGS) $^ -o $(NAME)

clean :
	rm -rf $(OBJS) $(UTILS_OBJS)

fclean : clean
	rm -rf $(NAME)
	rm -rf $(OBJSDIR)

re : fclean all

.PHONY : all clean fclean re
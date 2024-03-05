CPP = c++

CPPFLAGS = -std=c++98 -Wall -Werror -Wextra -fsanitize=address -g3 

SRCSDIR = ./srcs

UTILSDIR = ./utils

OBJSDIR = ./objs

NAME = HyperVaulted

SRC_FILES = Server.cpp webserv.cpp Multiplexing.cpp Client.cpp Request.cpp Response.cpp Post.cpp Cgi.cpp FileTemplate.cpp

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
	rm -rf ./www/html/server_1/imgs/upload*

fclean : clean
	rm -rf ErrorPages/[0123456789]*.html
	rm -rf $(NAME)
	rm -rf $(OBJSDIR)

re : fclean all

.PHONY : all clean fclean re
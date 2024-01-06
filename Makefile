CPP = c++

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g3

SRCS = webserv.cpp

OBJS = $(SRCS:.cpp=.o)

NAME = HyperVaulted

%.o : %.cpp
	$(CPP) $(CPPFLAGS) -o $(OBJS) -c $(SRCS)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CPP) $(CPPFLAGS) $(SRCS) -o $(NAME)

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : clean fclean re
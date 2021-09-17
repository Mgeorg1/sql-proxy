NAME =	proxy

SRCS =	main.cpp\
		Client.cpp\
		Proxy.cpp\
		logger.cpp

COMP = c++

OBJS = $(SRCS:.cpp=.o)

all:	$(NAME)

$(NAME):	$(OBJS)
			$(COMP) $(OBJS) -o $(NAME)

clean:
			rm -f $(OBJS)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:	re fclean clean all
SRCS_DIR = srcs/
FILES = away.cpp channel.cpp client.cpp erase.cpp invite.cpp join.cpp kick.cpp kill.cpp list.cpp main.cpp mode.cpp names.cpp oper.cpp part.cpp ping.cpp privmsg.cpp quit.cpp registration.cpp server.cpp topic.cpp userhost.cpp utils.cpp wallops.cpp
CPPFILES = $(addprefix ${SRCS_DIR}, ${FILES})
OFILES = $(patsubst %.cpp,%.o,$(CPPFILES))
DEPS = $(patsubst %.o,%.d,$(OFILES))
DEPFLAGS = -MMD -MF $(@:.o=.d)
FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g
NAME = ircserv

all:	$(NAME)

$(NAME):	$(OFILES)
			clang++ $(FLAGS) $(OFILES) -o $(NAME)
-include $(DEPS)

%.o:		%.cpp
			clang++ $(FLAGS) -c $< $(DEPFLAGS) -o $(<:.cpp=.o)

clean:		
			rm -f $(OFILES)
			rm -rf $(DEPS)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
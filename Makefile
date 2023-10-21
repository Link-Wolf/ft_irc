# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/12/12 09:27:42 by iCARUS            #+#    #+#              #
#    Updated: 2023/01/20 13:28:47 by iCARUS           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=			ircserv

FILES		=			\
	ft_irc				\
	Server.class		\
	commands			\
	Configuration.class	\
	Channel.class		\
	Client.class		\
	Bot.class			\

SRC_DIR		=			src/
SRC_FIL		=			$(FILES)
SRCS		=			$(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC_FIL)))

OBJS		=			$(SRCS:.cpp=.o)

CC			=			c++
CFLAGS		=			-Wall -Wextra -Werror
CPPFLAGS	=			-I$(SRC_DIR)
RM			=			rm -f

all: $(NAME)
	@printf "\t\e[96mALL\t\t[ ✓ ]\n\e[39m"

clean:
	@printf "\t\e[31mCLEAN\t\t[ ✓ ]\n\e[39m"
	@$(RM) $(OBJS)

fclean:	clean
	@printf "\t\e[35mFCLEAN\t\t[ ✓ ]\n\e[39m"
	@$(RM) $(NAME)

re:		fclean all
	@printf "\t\e[96mRE\t\t[ ✓ ]\n\e[39m"

$(NAME): $(OBJS)
	@printf "\e[36m\033[2K\r\t\t"$(NAME)"\n\e[39m"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -o $(NAME) $(OBJS)
	@printf "\e[36m\033[F\033[2K\r\tCOMPILATION\t[ ✓ ]\n\e[39m"

%.o: %.cpp %.hpp
	@printf "\e[36m\033[2K\r\t\t"$@"\e[39m"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.PHONY: all clean fclean re








# $(NAME): $(LIBFT) $(OBJS)
# 	$(CC) $(CFLAGS) -o $(NAME) -Llibft -lft $(OBJS) -L /usr/local/lib/ -lmlx -framework OpenGL -framework AppKit
# 	@printf "\e[36m\tCompilation \t[ ✓ ]\n\e[39m"

# $(LIBFT):
# 	@$(MAKE) -C $(LIBFTDIR)

# $(NAME_BONUS): $(LIBFT) $(OBJS_BONUS)
# 	$(CC) $(CFLAGS) -o $(NAME_BONUS) -Llibft -lft $(OBJS_BONUS) -L /usr/local/lib/ -lmlx -framework OpenGL -framework AppKit
# 	@printf "\e[96m\tBonus \t\t[ ✓ ]\n\e[39m"

# clean:
# 	@$(MAKE) clean -C $(LIBFTDIR)
# 	$(RM) $(OBJS) $(OBJS_BONUS)
# 	@printf "\e[31m\tClean \t\t[ ✓ ]\n\e[39m"

# fclean: clean
# 	@$(MAKE) fclean -C $(LIBFTDIR)
# 	$(RM) $(NAME) $(NAME_BONUS)
# 	@printf "\e[35;5;161m\tBinary Clean \t[ ✓ ]\n\e[39m"

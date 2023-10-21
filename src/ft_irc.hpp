/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 07:48:51 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/17 10:08:51 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_HPP
# define FT_IRC_HPP

# include <iostream>
# include <cstdlib>
# include <fstream>
# include <vector>

# include <unistd.h>
# include <signal.h>

# include "Server.class.hpp"
// # include "commands/ACommand.class.hpp"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

void						check_all_clients(Server &server);
void						process_client_message(Client *client, Server &server);
std::vector<std::string>	get_messages(Client *client);
std::vector<std::string>	split_message(std::string message);
void						sig_handler(int signo);

#endif	

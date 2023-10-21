/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.class.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xxxxxxx <xxxxxxx@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/19 08:48:53 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/17 16:16:58 by xxxxxxx          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Bot;

#ifndef BOT_CLASS_HPP
#define BOT_CLASS_HPP

#include <string>
#include <vector>
#include <map>

#include <poll.h>
#include <sys/socket.h>

#include "Client.class.hpp"
#include "Server.class.hpp"

class Bot : public Client
{
private:
	Server *server;

public:
	Bot(Server *server);
	Bot(Bot const &src);
	~Bot(void);

	Bot &operator=(Bot const &rhs);

	void	sendMessage(const std::string &command, const std::string &args);
	void	sendMessage(int response_code, const std::string &args);
	void	sendMessage(const std::string &source, const std::string &command, const std::string &args);

	bool	ends_with(std::string const & value, std::string const & ending);
};

std::ostream	&operator<<(std::ostream &o, Bot const &i);

#endif

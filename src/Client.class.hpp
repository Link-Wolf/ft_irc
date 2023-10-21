/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.class.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xxxxxxx <xxxxxxx@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/19 08:48:53 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/17 16:16:59 by xxxxxxx          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Client;

#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include <string>
#include <vector>
#include <map>

#include <poll.h>
#include <sys/socket.h>

#include "Channel.class.hpp"

class Client
{
public:

private:
	struct pollfd		poll_data;
	std::string			nickname;
	std::string			username;
	std::string			realname;
	std::string			buffer;
	std::string			mode;
	int					connection_status;	
	bool				pass;
	size_t 				nb_channels;

public:
	enum ConnectionStatus
	{
		CAP_LS_302 = 1,
		NICK_OR_USER = 2,
		REGISTERED = 3,
	};

	Client();
	Client(Client const &src);
	Client(
		int				fd,
		std::string		nickname,
		std::string		username,
		std::string		realname
 		);
	virtual ~Client();

	Client				&operator=(Client const &rhs);

	static std::string		stringifyCode(int code);

	virtual void	sendMessage(const std::string &command, const std::string &args);
	virtual void	sendMessage(int response_code, const std::string &args);
	virtual void	sendMessage(const std::string &source, const std::string &command, const std::string &args);

	const struct pollfd		&getPollData(void) const;
	void					setPollData(const struct pollfd	&src);
	const std::string		&getNickname(void) const;
	void					setNickname(const std::string &src);
	const std::string		&getUsername(void) const;
	void					setUsername(const std::string &src);
	const std::string		&getRealname(void) const;
	void					setRealname(const std::string &src);
	const std::string		&getBuffer(void) const;
	void					setBuffer(const std::string &src);
	const int				&getConnectionStatus(void) const;
	void					setConnectionStatus(const int &status);
	const bool				&isPass(void) const;
	void					setPass(bool pass);
	const size_t			&getNbChannels(void) const;
	void					setNbChannels(size_t nb_channels);
	const std::string		&getMode(void) const;
	void					addMode(const std::string &mode);
	void					delMode(const std::string &mode);
};

std::ostream			&operator<<(std::ostream &o, Client const &i);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.class.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xxxxxxx <xxxxxxx@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/19 09:39:47 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/17 16:20:01 by xxxxxxx          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Server;

#ifndef SERVER_CLASS_HPP

#define SERVER_CLASS_HPP

#include <vector>
#include <string>
#include <map>
#include <fstream>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>

#include "Bot.class.hpp"
#include "Client.class.hpp"
#include "Configuration.class.hpp"
#include "Channel.class.hpp"
#include "enum.hpp"

class Server
{
public:
	typedef std::map<int, Client *>				t_nickMapClient;
	typedef std::map<std::string, int>			t_nickMapFd;
	typedef std::map<std::string, Channel *>	t_nameMapChannel;
	typedef t_nickMapClient::iterator			t_clientIterator;
	typedef t_nameMapChannel::iterator			t_channelIterator;
	typedef t_nickMapClient::const_iterator	t_const_clientIterator;
	typedef t_nameMapChannel::const_iterator	t_const_channelIterator;

	typedef void (Server::*t_command)(std::vector<std::string>, Client *);
	
private:
	t_nickMapClient										clients;
	t_nickMapFd											client_fds;
	t_nameMapChannel									channels;
	Configuration										*config;
	int													listening_fd;
	std::map<std::string, t_command>					commands;
	std::vector<std::map<int, Client *>::iterator>		clients_to_delete;
	std::vector<std::map<std::string, int>::iterator>	fds_to_delete;

	void	cap(std::vector<std::string> tokens, Client *client);
	void	nick(std::vector<std::string> tokens, Client *client);
	void	user(std::vector<std::string> tokens, Client *client);
	void	pass(std::vector<std::string> tokens, Client *client);
	void	join(std::vector<std::string> tokens, Client *client);
	void	quit(std::vector<std::string> tokens, Client *client);
	void	ping(std::vector<std::string> tokens, Client *client);
	void	motd(std::vector<std::string> tokens, Client *client);
	void	part(std::vector<std::string> tokens, Client *client);
	void    mode(std::vector<std::string> tokens, Client *client);
	void	kick(std::vector<std::string> tokens, Client *client);
	void	topic(std::vector<std::string> tokens, Client *client);
	void	notice(std::vector<std::string> tokens, Client *client);
	void    invite(std::vector<std::string> tokens, Client *client);
	void	privmsg(std::vector<std::string> tokens, Client *client);
	
public:
	Server(void);
	Server(Server const &src);
	Server(std::string port, std::string password);
	~Server(void);

	Server	&operator=(Server const &rhs);

	void						addClient(Client *client);
	void						addChannel(Channel *channel);
	void						removeClient(int fd);
	void						removeClient(std::string client_nickname);
	void						removeChannel(std::string channel_name);
	int							executePoll(void);
	void						acceptConnection(void);
	void						executeCommand(std::vector<std::string> tokens, Client *client);
	void						welcome(Client *client);
	bool						existsChannel(std::string name) const;
	bool						existsClient(std::string name) const;
	void						delete_to_delete(void);
	void						sendGoodByeGoodByeToEverybady(std::string source, std::string command, std::string params);
	std::vector<std::string>	split(std::string str, char c) const;
	char						closestPlusMinus(const std::string &str, const char &c) const;

	t_nickMapClient			&getClients(void);
	const t_nickMapClient	&getClients(void) const;
	void					setClients(const t_nickMapClient &src);
	const					t_nickMapFd &getClientsFds(void) const;
	void					setClientsFds(const t_nickMapFd &src);
	const t_nameMapChannel	&getChannels(void) const;
	t_nameMapChannel		&getChannels(void);
	void					setChannels(const t_nameMapChannel &src);
	const Configuration 	*getConfig(void) const;
	void					setConfig(const Configuration *src);
	const int				&getListeningFd(void) const;
	void					setListeningFd(const int &src);
};

std::ostream 	&operator<<(std::ostream &o, Server const &i);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/19 09:59:40 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/16 17:56:43 by iCARUS           ###   ########.fr       */

/*                                                                            */
/* ************************************************************************** */

class Channel;

#ifndef CHANNEL_CLASS_HPP
#define CHANNEL_CLASS_HPP

#include <map>
#include <string>
#include <iostream>

#include "Client.class.hpp"

class Channel
{
private:
	typedef std::map<std::string, Client *> t_nickMapClient;

	t_nickMapClient	clients;
	t_nickMapClient	invited;
	Client			*op;
	std::string		mode;
	std::string		name;
	std::string		key;
	std::string		topic;
	

public:
	Channel();
	Channel(const Channel &src);
	Channel(std::string mode, std::string name, Client *op);
	~Channel();

	Channel &operator=(const Channel &src);

	void			addClient(Client *client);
	bool			isMember(Client *client);
	void			removeClient(Client *client);
	void			sendMessage(const std::string &source, const std::string &command, const std::string &args);
	void			privateMessage(const std::string &source, const std::string &command, const std::string &args);	
	bool    		isEmpty(void);
	std::string		getPrefix(Client *client);
	bool			isInvited(Client *client);
	std::string		addMode(const std::string &mode);
	std::string		delMode(const std::string &mode);
	void			addInvited(Client *client);
	void			delInvited(Client *client);
	bool			isOperator(Client *client);

	const std::string		&getName(void) const;
	void					setName(const std::string &src);
	const t_nickMapClient	&getClients(void) const;
	void					setClients(t_nickMapClient &src);
	const Client			*getOp(void) const;
	void					setOp(Client *src);
	const std::string		&getKey(void) const;
	void					setKey(std::string &src);
	const std::string		&getTopic(void) const;
	void					setTopic(std::string &src);
	std::string				getSymbol(void) const;
	void					setInvited(t_nickMapClient &src);
	const t_nickMapClient	&getInvited(void) const;
	const std::string		&getMode(void) const;
	void					setMode(std::string &src);
};

std::ostream &operator<<(std::ostream &o, const Channel &i);

#endif

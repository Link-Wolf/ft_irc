/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 15:21:57 by xxxxxxx           #+#    #+#             */
/*   Updated: 2023/01/20 13:17:20 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Channel.class.hpp"

/*
 ******************************* CONSTRUCTOR **********************************
 */

Channel::Channel()
{
	Channel("", "default_name", nullptr);
}

Channel::Channel(
	std::string mode,
	std::string name,
	Client *op
	)
{
	this->mode = mode;
	this->name = name;
	this->op = op;
	this->topic = "Raaaaat_IRC";
}

Channel::Channel(Channel const &src)
{
	*this = src;
}

/*
 ******************************* DESTRUCTOR ***********************************
 */

Channel::~Channel()
{
}

/*
 ******************************** OVERLOAD ************************************
 */

Channel &Channel::operator=(Channel const &rhs)
{
	if (this != &rhs)
	{
		this->mode = rhs.mode;
		this->name = rhs.name;
		this->op = rhs.op;
		this->clients = rhs.clients;
	}

	return *this;
}

std::ostream &operator<<(std::ostream &o, const Channel &rhs)
{
	o << "Channel(op: " << rhs.getOp() << ", name: " << rhs.getName()
	  << ", mode: " << rhs.getMode() << ")";
	return (o);
}

/*
 ******************************** METHODS *************************************
 */

// Add the given client to the channel
void Channel::addClient(Client *client)
{
	clients[client->getNickname()] = client;
}

// Remove the given client from the channel
void Channel::removeClient(Client *client)
{
	clients.erase(client->getNickname());
}

// Check if the given client is in the channel
bool	Channel::isMember(Client *client)
{
	return (clients.find(client->getNickname()) != clients.end());
}

// Send a message to all clients in the channel
void	Channel::sendMessage(const std::string &source, const std::string &command, const std::string &args)
{
	for (t_nickMapClient::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		it->second->sendMessage(source, command, args);
	}
}

// Send a message to all clients in the channel except the source
void	Channel::privateMessage(const std::string &source, const std::string &command, const std::string &args)
{
	for (t_nickMapClient::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNickname() != source)
			it->second->sendMessage(source, command, args);
	}
}

// Check if the channel doesn't have any client
bool    Channel::isEmpty(void)
{
	return (this->getClients().empty());
}

// Return the prefix of the given client in the channel (in this case, only @ for operator)
std::string	Channel::getPrefix(Client *client)
{
	if (op == client)
		return "@";
	return "";
}

// Check if the given client is invited to the channel 
bool	Channel::isInvited(Client *client)
{
	return (invited.find(client->getNickname()) != invited.end());
}

// Check if the given client is the operator of the channel
bool	Channel::isOperator(Client *client)
{
	return (op == client);
}

// Try to add the given mode(s) to the channel and return the effectively added mode(s)
std::string	Channel::addMode(const std::string &real_mode)
{
	std::string ret;

	for (size_t i = 0; i < real_mode.length(); i++)
	{
		if (this->mode.find(real_mode[i]) != std::string::npos)
			continue;
		this->mode += real_mode[i];
		ret += real_mode[i];
	}
	return ret;
}

// Try to remove the given mode(s) to the channel and return the effectively removed mode(s)
std::string	Channel::delMode(const std::string &real_mode)
{
	std::string ret;
	for (size_t i = 0; i < real_mode.length(); i++)
	{
		if (this->mode.find(real_mode[i]) == std::string::npos)
			continue;
		this->mode.erase(std::remove(this->mode.begin(), this->mode.end(), real_mode[i]), this->mode.end());
		ret += real_mode[i];
	}
	return ret;
}

// Add the given client to the invited list
void	Channel::addInvited(Client *client)
{
	invited[client->getNickname()] = client;
}

// Remove the given client from the invited list
void	Channel::delInvited(Client *client)
{
	invited.erase(client->getNickname());
}

/*
 ******************************** ACCESSOR ************************************
 */

const std::string &Channel::getName(void) const 
{
	return this->name;
}

void Channel::setName(const std::string &src)
{
	this->name = src;
}

const Channel::t_nickMapClient &Channel::getClients(void) const 
{
	return this->clients;
}

void Channel::setClients(Channel::t_nickMapClient &src)
{
	this->clients = src;
}

const Client *Channel::getOp(void) const
{
	return this->op;
}

void Channel::setOp(Client *src)
{
	this->op = src;
}

const std::string &Channel::getMode(void) const
{
	return this->mode;
}

void Channel::setMode(std::string &src)
{
	this->mode = src;
}

const std::string	&Channel::getKey(void) const
{
	return this->key;
}

void	Channel::setKey(std::string &src)
{
	this->key = src;
}

const std::string	&Channel::getTopic(void) const
{
	return this->topic;
}

void	Channel::setTopic(std::string &src)
{
	this->topic = src;
}

// Return the symbol of the channel, depending on its mode (s = secret, p = private) ((effectively, always returns "=" but chhhttt))
std::string	Channel::getSymbol(void) const
{
	if (this->mode.find('s') != std::string::npos)
		return "@";
	else if (this->mode.find('p') != std::string::npos)
		return "*";
	else
		return "=";
}

const Channel::t_nickMapClient &Channel::getInvited(void) const
{
	return this->invited;
}

void	Channel::setInvited(t_nickMapClient &src)
{
	this->invited = src;
}

/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 10:22:32 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/20 13:17:20 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Client.class.hpp"

/*
 ******************************* CONSTRUCTOR **********************************
 */

Client::Client()
{
	Client(-1, "", "", "");
}

Client::Client(
	int socket,
	std::string nickname,
	std::string username,
	std::string realname)
{
	poll_data.fd = socket;
	poll_data.events = POLLIN;
	this->nickname = nickname;
	this->username = username;
	this->realname = realname;
	this->buffer = "";
	this->connection_status = 0;
	this->nb_channels = 0;
}

Client::Client(Client const &src)
{
	*this = src;
}

/*
 ******************************* DESTRUCTOR ***********************************
 */

Client::~Client()
{
}

/*
 ******************************** OVERLOAD ************************************
 */

Client &Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->poll_data = rhs.poll_data;
		this->nickname = rhs.nickname;
		this->username = rhs.username;
		this->realname = rhs.realname;
	}

	return *this;
}

/*	Print Client data according to the following format:
 *	Client(nickname: <nickname>, username: <username>, realname: <realname>, fd: <fd>, channels: [channel1, channel2, ...])
 */
std::ostream &operator<<(std::ostream &o, Client const &rhs)
{
	o << "Client(nickname: " << rhs.getNickname() << ", username: "
	  << rhs.getUsername() << ", realname: " << rhs.getRealname()
	  << ", fd: " << rhs.getPollData().fd;
	return (o);
}

/*
 ******************************** METHODS *************************************
 */

// Stringify the given response code to a 3-digit string (%03d)
std::string	Client::stringifyCode(int code)
{
	std::string		str = std::to_string(code);

	while (str.length() < 3)
		str = "0" + str;
	return (str);
}

// Send a message to the client
void Client::sendMessage(const std::string &command, const std::string &args)
{
	sendMessage("ft_irc", command, args);
}

// Send a message to the client with the given response code
void Client::sendMessage(int response_code, const std::string &args)
{
	sendMessage(stringifyCode(response_code), args);
}

// Send a message to the client with the given response code and the given source
void Client::sendMessage(const std::string &source, const std::string &command, const std::string &args)
{
	std::string	editable(command);
	editable = ":" + source + " " + editable;
	editable += " ";
	editable += args;
	editable += "\r\n";
	send(this->poll_data.fd, editable.c_str(), editable.length(), 0);
}

/*
 ******************************** ACCESSOR ************************************
 */

const struct pollfd &Client::getPollData(void) const
{
	return this->poll_data;
}

void Client::setPollData(const struct pollfd &src)
{
	this->poll_data = src;
}

const std::string &Client::getNickname(void) const
{
	return this->nickname;
}

void Client::setNickname(const std::string &src)
{
	this->nickname = src;
}

const std::string &Client::getUsername(void) const
{
	return this->username;
}

void Client::setUsername(const std::string &src)
{
	this->username = src;
}

const std::string &Client::getRealname(void) const
{
	return this->realname;
}

void Client::setRealname(const std::string &src)
{
	this->realname = src;
}

const std::string	&Client::getBuffer(void) const
{
	return this->buffer;
}

void	Client::setBuffer(const std::string &src)
{
	this->buffer = src;
}

const int &Client::getConnectionStatus(void) const
{
	return this->connection_status;
}

void Client::setConnectionStatus(const int &status)
{
	this->connection_status = status;
}

const bool	&Client::isPass(void) const
{
	return this->pass;
}

void	Client::setPass(bool pass)
{
	this->pass = pass;
}

const size_t	&Client::getNbChannels(void) const
{
	return this->nb_channels;
}

void	Client::setNbChannels(size_t nb_channels)
{
	this->nb_channels = nb_channels;
}

const std::string	&Client::getMode(void) const
{
	return this->mode;
}

void	Client::addMode(const std::string &mode)
{
	for (size_t i = 0; i < mode.length(); i++)
	{
		if (std::find(this->mode.begin(), this->mode.end(), mode[0]) == this->mode.end())
			continue;
		this->mode += mode[i];
	}
}

void	Client::delMode(const std::string &mode)
{
	for (size_t i = 0; i < mode.length(); i++)
	{
		if (find(this->mode.begin(), this->mode.end(), mode[0]) != this->mode.end())
			continue;
		this->mode.erase(std::remove(this->mode.begin(), this->mode.end(), mode[i]), this->mode.end());
	}
}

/* ************************************************************************** */

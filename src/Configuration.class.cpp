/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.class.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 07:50:32 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/20 13:17:20 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Configuration.class.hpp"

/*
 ******************************* CONSTRUCTOR **********************************
 */

Configuration::Configuration()
{
	Configuration("4242", "strong");
}

Configuration::Configuration(const Configuration &src)
{
	*this = src;
}

Configuration::Configuration(std::string port, std::string password)
{
	this->port = port;
	this->password = password;
	this->chanlimit = 42;
	this->channellen = 12;
	this->nicklen = 12;
	this->topiclen = 42;
	this->userlen = 12;
}

/*
 ******************************* DESTRUCTOR ***********************************
 */

Configuration::~Configuration()
{}

/*
 ******************************** OVERLOAD ************************************
 */

Configuration	&Configuration::operator=(const Configuration &rhs)
{
	if ( this != &rhs )
	{
		this->port = rhs.port;
		this->password = rhs.password;
	}
	return *this;
}

std::ostream	&operator<<(std::ostream &o, const Configuration &i)
{
	o << "Configuration(Port: " << i.getPort() << "; Password: "
		<< i.getPassword() << ")" << std::endl;
	return o;
}

/*
 ******************************** METHODS *************************************
 */


/*
 ******************************** ACCESSOR ************************************
 */

const std::string	&Configuration::getPort() const
{
	return this->port;
}

const std::string	&Configuration::getPassword() const
{
	return this->password;
}

void				Configuration::setPort(const std::string &port)
{
	this->port = port;
}

void				Configuration::setPassword(const std::string &password)
{
	this->password = password;
}

size_t 				Configuration::getChanlimit() const
{	
	return this->chanlimit;
}

size_t 				Configuration::getChannellen() const
{
	return this->channellen;
}

size_t 				Configuration::getModes() const
{
	return this->modes;
}

size_t 				Configuration::getNicklen() const
{
	return this->nicklen;
}

size_t 				Configuration::getTopiclen() const
{
	return this->topiclen;
}

size_t 				Configuration::getUserlen() const
{
	return this->userlen;
}

/* ************************************************************************** */

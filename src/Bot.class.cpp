/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.class.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 10:22:32 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/20 13:17:20 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Bot.class.hpp"

/*
 ******************************* CONSTRUCTOR **********************************
 */

Bot::Bot(Server *server) : Client (-1, "Ratatouille", "rata", "touille")
{
	this->server = server;
}

Bot::Bot(Bot const &src) : Client (src)
{
	*this = src;
}

/*
 ******************************* DESTRUCTOR ***********************************
 */

Bot::~Bot()
{
}

/*
 ******************************** OVERLOAD ************************************
 */

Bot &Bot::operator=(Bot const &rhs)
{
	(void) rhs;
	return *this;
}

std::ostream &operator<<(std::ostream &o, Bot const &rhs)
{
	(void) rhs;
	o << "Ratatouille-b0t is rat(ight) here" ;
	return (o);
}

/*
 ******************************** METHODS *************************************
 */

void Bot::sendMessage(const std::string &command, const std::string &args)
{
	sendMessage("ft_irc", command, args);
}

void Bot::sendMessage(int response_code, const std::string &args)
{
	sendMessage(stringifyCode(response_code), args);
}

void Bot::sendMessage(const std::string &source, const std::string &command, const std::string &args)
{
	// React only to PRIVMSG
	if (command != "PRIVMSG")
		return ;
	
	// React to messages ending with "pour quoi", "pourquoi", "quoi", "rat", or containing "dis"
	if (ends_with(args, "pour quoi"))
	{
		std::vector<std::string> cmd;
		cmd.push_back("PRIVMSG");
		cmd.push_back(server->split(args, ' ')[0]);
		cmd.push_back("pour feur!");
		server->executeCommand(cmd, this);
	}
	else if (ends_with(args, "pourquoi"))
	{
		std::vector<std::string> cmd;
		cmd.push_back("PRIVMSG");
		cmd.push_back(server->split(args, ' ')[0]);
		cmd.push_back("pourfeur!");
		server->executeCommand(cmd, this);
	}
	else if (ends_with(args, "quoi"))
	{
		std::vector<std::string> cmd;
		cmd.push_back("PRIVMSG");
		cmd.push_back(server->split(args, ' ')[0]);
		cmd.push_back("feur!");
		server->executeCommand(cmd, this);
	}
	else if (ends_with(args, "rat"))
	{
		std::vector<std::string> cmd;
		cmd.push_back("PRIVMSG");
		cmd.push_back(server->split(args, ' ')[0]);
		cmd.push_back("atouille");
		server->executeCommand(cmd, this);
	}
	else if (args.find("dis") != std::string::npos)
	{
		std::vector<std::string> cmd;
		cmd.push_back("PRIVMSG");
		cmd.push_back(server->split(args, ' ')[0]);
		cmd.push_back(args.substr(args.find("dis") + 3));
		server->executeCommand(cmd, this);
	}
	(void) source;
}

// Function to check if a string `value` ends with string `ending`
bool Bot::ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

/*
 ******************************** ACCESSOR ************************************
 */


/* ************************************************************************** */

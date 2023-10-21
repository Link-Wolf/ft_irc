/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/20 12:51:51 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/20 13:17:20 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands.hpp"

/*
 * The PASS command is used to set a 'connection password'. The password can and must be set before any attempt 
 * to register the connection is made. Currently this requires that the user send a PASS command before sending 
 * the NICK/USER combination.
 */
void	Server::pass(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 2 || tokens[1].empty())
	{
		std::cerr << "Error: not enough parameters" << std::endl;
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enought parameters");
		return ;
	}

	if (client->getConnectionStatus() == Client::REGISTERED)
	{
		std::cerr << "Error: client already registered" << std::endl;
		client->sendMessage(ERR_ALREADYREGISTERED, client->getNickname() + " :You may not reregister");
		return ;
	}

	if (tokens[1] != this->config->getPassword())
	{
		std::cerr << "Error: password mismatch" << std::endl;
		client->sendMessage(ERR_PASSWDMISMATCH, client->getNickname() + " :Password missmatch");
		client->setPass(false);
		return ;
	}

	client->setPass(true);
}

/*
 * The CAP command is used to negotiate capabilities with the server.
 * In fact, since we use Weechat, we only support the CAP LS 302 command.
 */
void	Server::cap(std::vector<std::string> tokens, Client *client)
{
	if (client->getConnectionStatus())
		return ;
		
	if (tokens[1] == "LS" && tokens[2] == "302")
		client->setConnectionStatus(Client::CAP_LS_302);
}

/*
 * The NICK command is used to give user a nickname or change the previous one.
 * The nickname has to be unique on the server.
 */
void	Server::nick(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 2 || tokens[1].empty())
	{
		std::cerr << "Error: no nickname given" << std::endl;
		client->sendMessage(ERR_NONICKNAMEGIVEN, ":No nickname given");
		return ;
	}

	if (tokens[1].find(' ') != std::string::npos
		|| tokens[1].find(',') != std::string::npos
		|| tokens[1].find('*') != std::string::npos
		|| tokens[1].find('?') != std::string::npos
		|| tokens[1].find('!') != std::string::npos
		|| tokens[1].find('@') != std::string::npos
		|| tokens[1].find('.') != std::string::npos
		|| tokens[1][0] == ':' || tokens[1][0] == '$'
		|| tokens[1][0] == '#' || tokens[1][0] == '&')
	{
		std::cerr << "Error: invalid nickname" << std::endl;
		client->sendMessage(ERR_ERRONEUSNICKNAME, client->getNickname() + " " + tokens[1] + " :Erroneus nickname");
		return ;
	}

	for (
		std::map<int, Client *>::const_iterator it
			= clients.begin();
		it != clients.end();
		it++)
	{
		if (it->second->getNickname() == tokens[1])
		{
			std::cerr << "Error: nickname already in use" << std::endl;
			client->sendMessage(ERR_NICKNAMEINUSE, client->getNickname() + " " + tokens[1] + " :Nickname is already in use");
			return ;
		}
	}

	std::string old_nick = client->getNickname();
	if (tokens[1].size() > this->getConfig()->getNicklen())
		client->setNickname(tokens[1].substr(0, this->getConfig()->getNicklen()));
	else
		client->setNickname(tokens[1]);
	client_fds[client->getNickname()] = client->getPollData().fd;
	
	if (client->getConnectionStatus() == Client::CAP_LS_302)
	{
		client->setConnectionStatus(Client::NICK_OR_USER);
		return ;
	}
	if (client->getConnectionStatus() == Client::NICK_OR_USER)
	{
		client->setConnectionStatus(Client::REGISTERED);
		this->welcome(client);
		return ;
	}
	
	this->sendGoodByeGoodByeToEverybady(old_nick, "NICK", client->getNickname());
}

/*
 * The USER command is used at the beginning of connection to specify the username, hostname and realname of a new user.
 */
void	Server::user(std::vector<std::string> tokens, Client *client)
{
	//Check if there is a username
	if (tokens.size() < 5 || tokens[1].empty())
	{
		std::cerr << "Error: not enough parameters" << std::endl;
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enought parameters");
		return ;
	}

	//Check if the client is already registered
	if (client->getConnectionStatus() == Client::REGISTERED)
	{
		std::cerr << "Error: client already registered" << std::endl;
		client->sendMessage(ERR_ALREADYREGISTERED, client->getNickname() + " :You may not reregister");
		return;
	}
	
	if (tokens[1].size() > this->getConfig()->getUserlen())
		client->setUsername(tokens[1].substr(0, this->getConfig()->getUserlen()));
	else
		client->setUsername(tokens[1]);
	client->setRealname(tokens[4]);

	//Update the connection status depending on the previous status
	if (client->getConnectionStatus() == Client::CAP_LS_302)
		client->setConnectionStatus(Client::NICK_OR_USER);
	else
	{
		client->setConnectionStatus(Client::REGISTERED);
		this->welcome(client);
	}	
}

/*
 * The JOIN command is used by a user to join a channel, if the channel doesn't exist, it is created.
 * The channel name must start with a '#'
 * In case of `JOIN 0`, the user leaves all the channels he is in.
 */
void	Server::join(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 2)
	{
		std::cerr << "Error: not enough parameters" << std::endl;
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enought parameters");
		return;
	}

	if (tokens[1] == "0")
	{
		for (
			std::map<std::string, Channel *>::iterator it = channels.begin();
			it != channels.end();
			it++
			)
		{
			if (it->second->isMember(client))
			{
				std::vector<std::string> tokens;
				tokens.push_back("PART");
				tokens.push_back(it->second->getName());
				part(tokens, client);
			}
		}
		client->setNbChannels(0);
		return ;
	}
	
	std::vector<std::string> channel_names = split(tokens[1], ',');
	std::vector<Channel *> channels_to_sub;
	for (
		std::vector<std::string>::iterator it_names = channel_names.begin();
		it_names != channel_names.end();
		it_names++
		)
	{
		if (*it_names == "#" || (*it_names)[0] != '#' || it_names->find(',') != std::string::npos || it_names->find(' ') != std::string::npos || it_names->find('\x07') != std::string::npos)
		{
			client->sendMessage(ERR_BADCHANMASK,*it_names + " :Cannot join channel: Channel must start with # and cannot contain spaces, commas or \\x07");
			continue ;
		}		
		if (client->getNbChannels() >= this->config->getChanlimit())
		{
			client->sendMessage(ERR_TOOMANYCHANNELS, client->getNickname() + " " + *(it_names) + " :You have joined too many channels"); 
			continue ;
		}
		if (existsChannel(*it_names))
		{
			channels_to_sub.push_back(this->getChannels()[*it_names]);
		}
		else 
		{
			Channel *channel_to_add = new Channel("", *it_names, client);
			channel_to_add->addClient(clients[-1]);
			channels_to_sub.push_back(channel_to_add);
			channels[*it_names] = channel_to_add;
		}
	}
	for (
		std::vector<Channel *>::iterator it = channels_to_sub.begin();
		it != channels_to_sub.end();
		it++)
	{
		if ((*it)->getMode().find('i') != std::string::npos && (*it)->isInvited(client))
		{
			client->sendMessage(ERR_INVITEONLYCHAN, client->getNickname() + " " + (*it)->getName() + " :Cannot join channel (+i)");
			continue ;
		}
		(*it)->addClient(client);
		if ((*it)->isInvited(client))
			(*it)->delInvited(client);
		(*it)->sendMessage(client->getNickname(), "JOIN", tokens[1]);
		if ((*it)->getTopic() != "")
			client->sendMessage(RPL_TOPIC, client->getNickname() + " " + tokens[1] + " :" + (*it)->getTopic());
		client->setNbChannels(client->getNbChannels() + 1);
		for (
			std::map<std::string, Client *>::const_iterator it_client = (*it)->getClients().begin();
			it_client != (*it)->getClients().end();
			it_client++
			)
		{
			client->sendMessage(RPL_NAMREPLY, client->getNickname() + " " + (*it)->getSymbol() + " "
				+ (*it)->getName() + " :" + (*it)->getPrefix(it_client->second) + (*it_client).second->getNickname());
		}
		client->sendMessage(RPL_ENDOFNAMES, client->getNickname() + " "
				+ (*it)->getName() + " :End of /NAMES list");
	}
}

/*
 * The QUIT command is used to disconnect from the server.
 * It is automatically by the client if the connection is lost or the client quits.
 * If it leaves a channel empty, the channel is destroyed.
 */
void	Server::quit(std::vector<std::string> tokens, Client *client)
{
	client->sendMessage("ERROR", tokens[1]);
	std::vector<std::string> to_del;
	for (t_channelIterator it = channels.begin() ; it != channels.end() ; it++)
	{
		if ((*it).second->isMember(client))
		{
			(*it).second->removeClient(client);
			(*it).second->sendMessage(client->getNickname(), "QUIT", "Quit: " + tokens[1]);
			if ((*it).second->isEmpty())
			{
				to_del.push_back(it->second->getName());
			}
		}
	}

	for (std::vector<std::string>::iterator it = to_del.begin(); it != to_del.end() ; it++)
	{
		removeChannel(*it);
	}

	client->setNbChannels(0);
	removeClient(client->getNickname());
}

/*
 * The PING command is used to test the presence of an active client or server.
 * A PING message is sent to the server and the server responds with a PONG message.
 * If the client does not receive a PONG message within a set amount of time, the client may assume that the connection is no longer active.
 */
void	Server::ping(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 2)
	{
		std::cerr << "Error: not enough parameters" << std::endl;
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enought parameters");
		return;
	}

	client->sendMessage("PONG", "ft_irc " + tokens[1]);
}

/*
 * The MOTD command is used to get the message of the day of the server.
 * The message of the day is usually a file containing information about the server.
 */
void	Server::motd(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() >= 2)
	{
		std::cerr << "Error: no such server" << std::endl;
		client->sendMessage(ERR_NOSUCHSERVER, client->getNickname() + " ft_irc :Not such server");
		return ;
	}

	client->sendMessage(RPL_MOTDSTART, client->getNickname() + " :- [ft_irc] Message of the day - ");

	std::ifstream motd_file("Motd.txt");
	if (motd_file.is_open())
	{
		std::string line;
		while (getline(motd_file, line))
			client->sendMessage(RPL_MOTD, client->getNickname() + " :" + line);
		motd_file.close();
	}
	else
	{
		std::cerr << "Error: could not open motd file" << std::endl;
		client->sendMessage(ERR_NOMOTD, client->getNickname() + " :No MOTD file found");
		return ;
	}

	client->sendMessage(RPL_ENDOFMOTD, client->getNickname() + " :- End of MOTD command -");
}

/*
 * The PART command is used to leave a channel.
 * If the client is the last one in the channel, the channel is destroyed.
 */
void	Server::part(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 2)
	{
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enough parameters");
		return;
	}

	std::vector<std::string> channel_names = split(tokens[1], ',');
	for (
		std::vector<std::string>::iterator it = channel_names.begin();
		it != channel_names.end();
		it++
		)
	{
		if (!existsChannel(*it))
		{
			client->sendMessage(ERR_NOSUCHCHANNEL, client->getNickname() + " " + *it + " :No such channel");
			continue ;
		}

		Channel *channel = channels[*it];

		if (!channel->isMember(client))
		{
			client->sendMessage(ERR_NOTONCHANNEL, client->getNickname() + " " + *it + " :You're not on that channel");
			continue ;
		}

		channel->sendMessage(client->getNickname(), "PART", *it + " " + tokens[2]);
		channel->removeClient(client);
		if (channel->isEmpty())
		{
			removeChannel(*it);
		}
	}
}

/*
 * The TOPIC command is used to get or set the topic of a channel.
 */
void	Server::topic(std::vector<std::string> tokens, Client *client)
{
	(void) tokens;
	(void) client;

	if (tokens.size() < 2)
	{
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enough parameters");
		return;
	}

	if (this->getChannels().size() == 0)
	{
		client->sendMessage(RPL_NOTOPIC, client->getNickname() + " " + tokens[1] + " :No topic is set");
		return;
	}

	if (!this->getChannels()[tokens[1]]->isMember(client))
	{
		client->sendMessage(ERR_NOTONCHANNEL, client->getNickname() + " " + tokens[1] + " :You're not on that channel");
		return;
	}

	

	if (tokens.size() == 2)
	{
		if (this->getChannels()[tokens[1]]->getTopic().empty())
			client->sendMessage(RPL_NOTOPIC, client->getNickname() + " " + tokens[1] + " :No topic is set");
		else
			client->sendMessage(RPL_TOPIC, client->getNickname() + " " + tokens[1] + " :" + this->getChannels()[tokens[1]]->getTopic());
	}
	else
	{
		if (this->getChannels()[tokens[1]]->getMode().find('t') == std::string::npos || this->getChannels()[tokens[1]]->getOp() == client)
		{
			this->getChannels()[tokens[1]]->setTopic(tokens[2]);
			this->getChannels()[tokens[1]]->sendMessage(client->getNickname(), "TOPIC", tokens[1] + " :" + tokens[2]);
		}
		else
			client->sendMessage(ERR_CHANOPRIVSNEEDED, client->getNickname() + " " + tokens[1] + " :You're not channel operator");
	}
}

/*
 * The PRIVMSG command is used to send private messages between users.
 * If the target is a channel, the message is sent to all the users in the channel.
 * If the target is a user, the message is sent to that user.
 */
void	Server::privmsg(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 3)
	{
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enough parameters");
		return;
	}

	if (tokens[1][0] == '#')
	{
		if (!existsChannel(tokens[1]))
		{
			client->sendMessage(ERR_NOSUCHCHANNEL, client->getNickname() + " " + tokens[1] + " :No such channel");
			return;
		}

		Channel *channel = channels[tokens[1]];

		channel->privateMessage(client->getNickname(), "PRIVMSG", tokens[1] + " :" + tokens[2]);
	}
	else
	{
		if (!existsClient(tokens[1]))
		{
			client->sendMessage(ERR_NOSUCHNICK, client->getNickname() + " " + tokens[1] + " :No such nick");
			return;
		}

		Client *target = clients[client_fds[tokens[1]]];
		target->sendMessage(client->getNickname(), "PRIVMSG", client->getNickname() + " :" + tokens[2]);
	}
}

/*
 * The NOTICE command is used to send private messages between users.
 * If the target is a channel, the message is sent to all the users in the channel.
 * If the target is a user, the message is sent to that user.
 * The difference between NOTICE and PRIVMSG is that automatic replies must never be sent in response to a NOTICE message.
 * In our case, our fantastic bot Ratatouille will not answer to a NOTICE message.
 */
void	Server::notice(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 3)
	{
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enough parameters");
		return;
	}

	if (tokens[1][0] == '#')
	{
		if (!existsChannel(tokens[1]))
		{
			client->sendMessage(ERR_NOSUCHCHANNEL, client->getNickname() + " " + tokens[1] + " :No such channel");
			return;
		}

		Channel *channel = channels[tokens[1]];
		if (!channel->isMember(client))
		{
			client->sendMessage(ERR_NOTONCHANNEL, client->getNickname() + " " + tokens[1] + " :You're not on that channel");
			return;
		}

		channel->privateMessage(client->getNickname(), "NOTICE", tokens[1] + " :" + tokens[2]);
	}
	else
	{
		if (!existsClient(tokens[1]))
		{
			client->sendMessage(ERR_NOSUCHNICK, client->getNickname() + " " + tokens[1] + " :No such nick");
			return;
		}

		Client *target = clients[client_fds[tokens[1]]];
		target->sendMessage(client->getNickname(), "NOTICE", client->getNickname() + " :" + tokens[2]);
	}
}

/*
 * The MODE command is used to set or view the user or channel mode.
 * If the target is a channel and the mode changes, the message is sent to all the users in the channel.
 */
void    Server::mode(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 2)
	{
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enough parameters");
		return;
	}

	if (tokens[1][0] == '#')
	{
		if (!existsChannel(tokens[1]))
		{
			client->sendMessage(ERR_NOSUCHCHANNEL, client->getNickname() + " " + tokens[1] + " :No such channel");
			return;
		}

		Channel *channel = channels[tokens[1]];
		if (tokens.size() == 2)
		{
			client->sendMessage(RPL_CHANNELMODEIS, client->getNickname() + " " + tokens[1] + " " + channel->getMode());
			return;
		}

		if (client != channel->getOp())
		{
			client->sendMessage(ERR_CHANOPRIVSNEEDED, client->getNickname() + " " + tokens[1] + " :You're not channel operator");
			return;
		}

		for (size_t i = 0; i < tokens[2].size(); i++)
		{
			if (tokens[2][i] != '+' && tokens[2][i] != '-'  && tokens[2][i] != 'i' && tokens[2][i] != 't')
			{
				client->sendMessage(ERR_UNKNOWNMODE, client->getNickname() + " " + tokens[2][i] + " :is unknown mode char to me");
				return;
			}
		}

		std::string to_send;
		std::string buffer_add;
		std::string buffer_del;
		if (closestPlusMinus(tokens[2], 'i') == '+')
			buffer_add += channel->addMode("i");
		else if (closestPlusMinus(tokens[2], 'i') == '-')
			buffer_del += channel->delMode("i");
		if (closestPlusMinus(tokens[2], 't') == '+')
			buffer_add += channel->addMode("t");
		else if (closestPlusMinus(tokens[2], 't') == '-')
			buffer_del += channel->delMode("t");
		to_send = (buffer_add != "" ? ("+" + buffer_add) : "") + (buffer_del != "" ? ("-" + buffer_del) : "");
		
		channel->sendMessage(client->getNickname(), "MODE", tokens[1] + " " + to_send);			
	}
	else
	{
		if (!existsClient(tokens[1]))
		{
			client->sendMessage(ERR_NOSUCHNICK, client->getNickname() + " " + tokens[1] + " :No such nick");
			return;
		}

		Client *target = clients[client_fds[tokens[1]]];
		if (target->getNickname() != tokens[1])
		{
			client->sendMessage(ERR_USERSDONTMATCH, client->getNickname() + " :Cant change mode for other users");
		}

		if (tokens.size() == 2)
		{
			client->sendMessage(RPL_UMODEIS, client->getNickname() + " :" + target->getMode());
			return;
		}
	}
}

/*
 * The INVITE command is used to invite a user to a channel.
 * The server sends an INVITE message to the target user.
 * The invite is cancelled if the target joins the channel.
 */
void	Server::invite(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 3)
	{
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enough parameters");
		return;
	}

	if (!existsChannel(tokens[1]))
	{
		client->sendMessage(ERR_NOSUCHCHANNEL, client->getNickname() + " " + tokens[1] + " :No such channel");
		return;
	}

	if (!existsClient(tokens[2]))
	{
		client->sendMessage(ERR_NOSUCHNICK, client->getNickname() + " " + tokens[2] + " :No such nick");
		return;
	}

	Channel *channel = channels[tokens[1]];
	if (!channel->isMember(client))
	{
		client->sendMessage(ERR_NOTONCHANNEL, client->getNickname() + " " + tokens[1] + " :You're not on that channel");
		return;
	}

	if (!channel->isOperator(client) && channel->getMode().find('i') != std::string::npos)
	{
		client->sendMessage(ERR_CHANOPRIVSNEEDED, client->getNickname() + " " + tokens[1] + " :You're not channel operator");
		return;
	}

	if (channel->isMember(clients[client_fds[tokens[2]]]))
	{
		client->sendMessage(ERR_USERONCHANNEL, client->getNickname() + " " + tokens[2] + " " + tokens[1] + " :is already on channel");
		return;
	}

	Client *target = clients[client_fds[tokens[2]]];
	target->sendMessage(client->getNickname(), "INVITE", tokens[2] + " " + tokens[1]);
	client->sendMessage(RPL_INVITING, client->getNickname() + " " + tokens[2] + " " + tokens[1]);
	channel->addInvited(clients[client_fds[tokens[2]]]);
}

/*
 * The KICK command is used to request the forced removal of a user from a channel.
 * It causes the target user to be removed from the list of active
 * members for the channel specified by the <channel> parameter.
 */
void	Server::kick(std::vector<std::string> tokens, Client *client)
{
	if (tokens.size() < 3)
	{
		client->sendMessage(ERR_NEEDMOREPARAMS, client->getNickname() + " " + tokens[0] + " :Not enough parameters");
		return;
	}

	if (!existsChannel(tokens[1]))
	{
		client->sendMessage(ERR_NOSUCHCHANNEL, client->getNickname() + " " + tokens[1] + " :No such channel");
		return;
	}

	Channel *channel = channels[tokens[1]];

	if (!existsClient(tokens[2]))
	{
		client->sendMessage(ERR_NOSUCHNICK, client->getNickname() + " " + tokens[2] + " :No such nick");
		return;
	}

	Client *target = clients[client_fds[tokens[2]]];

	if (!channel->isMember(target))
	{
		client->sendMessage(ERR_USERNOTINCHANNEL, client->getNickname() + " " + tokens[2] + " " + tokens[1] + " :is not on channel");
		return;
	}

	if (!channel->isMember(client))
	{
		client->sendMessage(ERR_NOTONCHANNEL, client->getNickname() + " " + tokens[1] + " :You're not on that channel");
		return;
	}

	if (!channel->isOperator(client))
	{
		client->sendMessage(ERR_CHANOPRIVSNEEDED, client->getNickname() + " " + tokens[1] + " :You're not channel operator");
		return;
	}

	if (tokens.size() == 3)
	{
		channel->sendMessage(client->getNickname(), "KICK", tokens[1] + " " + tokens[2] + " :You have been kicked from channel");
	}
	else
	{
		channel->sendMessage(client->getNickname(), "KICK", tokens[1] + " " + tokens[2] + " :" + tokens[3]);
	}
	channel->removeClient(target);
}
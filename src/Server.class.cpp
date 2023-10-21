/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 08:05:59 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/04 13:32:550 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server.class.hpp"

/*
 ******************************* CONSTRUCTOR **********************************
 */

Server::Server(void)
{
	Server("4242", "password");
}

// Pfiou, what a constructor right ? Basically, setup network stuff, create the bot and set all the commands.
Server::Server(std::string listening_port, std::string password)
{
	config = new Configuration(listening_port, password);

	struct addrinfo hints;
	struct addrinfo *serv_info;

	int port;
	try 
	{
		port = std::stoi(config->getPort());
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: invalid port number" << std::endl;
		exit(1);
	}
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		std::cerr << "Error: socket creation failed (" << errno << ")" << std::endl;
		exit(1);
	}
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, config->getPort().c_str(), &hints, &serv_info) < 0)
	{
		std::cerr << "Error: getaddrinfo failed (" << errno << ")" << std::endl;
		exit(1);
	}
	if (serv_info == nullptr)
	{
		std::cerr << "Error: no address found" << std::endl;
		freeaddrinfo(serv_info);
		exit(1);
	}
	if (bind(sock, serv_info->ai_addr, serv_info->ai_addrlen) < 0)
	{
		std::cerr << "Error: bind failed (" << errno << ")" << std::endl;
		freeaddrinfo(serv_info);
		exit(1);
	}
	freeaddrinfo(serv_info);
	if (listen(sock, 5) < 0)
	{
		std::cerr << "Error: listen failed (" << errno << ")" << std::endl;
		exit(1);
	}
	std::clog << "Server is listening on port " << port << std::endl;
	fcntl(sock, F_SETFL, O_NONBLOCK);
	listening_fd = sock;
	clients[-1] = new Bot(this);
	client_fds["Ratatouille"] = -1;

	clients[-1]->setConnectionStatus(Client::REGISTERED);

	commands["CAP"] = &Server::cap;
	commands["NICK"] = &Server::nick;
	commands["USER"] = &Server::user;
	commands["PASS"] = &Server::pass;
	commands["JOIN"] = &Server::join;
	commands["PING"] = &Server::ping;
	commands["QUIT"] = &Server::quit;
	commands["PART"] = &Server::part;
	commands["KICK"] = &Server::kick;
	commands["MODE"] = &Server::mode;
	commands["TOPIC"] = &Server::topic;
	commands["NOTICE"] = &Server::notice;
	commands["INVITE"] = &Server::invite;
	commands["PRIVMSG"] = &Server::privmsg;
}

Server::Server(Server const &src)
{
	*this = src;
}

/*
 ******************************* DESTRUCTOR ***********************************
 */

Server::~Server(void)
{
	clients.clear();
	channels.clear();
	client_fds.clear();
	delete config;
}

/*
 ******************************** OVERLOAD ************************************
 */

Server &Server::operator=(Server const &rhs)
{
	if (this != &rhs)
	{
		config = rhs.config;
		clients = rhs.clients;
		channels = rhs.channels;
	}
	return (*this);
}

/*	Print the server's configuration and the list of clients and channels according to the following format:
 *	Server(
 *		config: Configuration(...),
 *		clients: [
 *			Client(...),
 *			...
 *		],
 *		channels: [
 *			Channel(...),
 *			...
 *		]
 *	)
 */
std::ostream &operator<<(std::ostream &o, Server const &i)
{
	o << "Server(\n\tconfig: " << i.getConfig(); //
	o << ",\n\tclients: [\n";
	for (
		Server::t_const_clientIterator it = i.getClients().begin();
		it != i.getClients().end();
		it++)
	{
		o << "\t\t" << it->second;
		if (it != --i.getClients().end())
			o << ",";
		o << std::endl;
	}
	o << "],\n\tchannels: [\n";
	for (
		Server::t_const_channelIterator it = i.getChannels().begin();
		it != i.getChannels().end();
		it++)
	{
		o << "\t\t" << it->second;
		if (it != --i.getChannels().end())
			o << ",";
		o << std::endl;
	}
	o << "]\n)";
	return (o);
}

/*
 ******************************** METHODS *************************************
 */

// Execute the poll() system call and update the pollfd struct of each client (basically, check who the f*ck is talking to me)
int Server::executePoll(void)
{
	struct pollfd *pollfd_list;
	int n = clients.size();
	pollfd_list = new struct pollfd[n];
	int i = 0;
	for (
		t_clientIterator it = clients.begin();
		it != clients.end();
		it++)
	{
		pollfd_list[i] = it->second->getPollData();
		i++;
	}

	int res = poll(pollfd_list, n, 0);

	i = 0;
	for (
		t_clientIterator it = clients.begin();
		it != clients.end();
		it++)
	{
		it->second->setPollData(pollfd_list[i]);
		i++;
	}

	delete[] (pollfd_list);
	return res;
}

// Accept a new connection from a brand new client
void Server::acceptConnection(void)
{
	int client_socket;

	client_socket = accept(listening_fd, NULL, NULL);
	if (client_socket == -1 && errno != 35)
	{
		std::cerr << "Error: accept failed (" << errno << ")" << std::endl;
		return;
	}
	if (client_socket != -1)
		clients[client_socket] = new Client(client_socket, "", "", "");
}

// Add a new client to the server
void Server::addClient(Client *client)
{
	clients[client->getPollData().fd] = client;
	client_fds[client->getNickname()] = client->getPollData().fd;
}

// Remove a client from the server by its file descriptor
void Server::removeClient(int fd)
{
	fds_to_delete.push_back(client_fds.find(clients.find(fd)->second->getNickname()));
	clients_to_delete.push_back(clients.find(fd));	
}

// Remove a client from the server by its nickname
void Server::removeClient(std::string client_nickname)
{
	removeClient(client_fds[client_nickname]);
}

// Add a new channel to the server
void Server::addChannel(Channel *channel)
{
	channels[channel->getName()] = channel;
}

// Remove a channel from the server by its name
void Server::removeChannel(std::string channel_name)
{
	channels.erase(channel_name);
}

// Execute the command sent by a client 
void Server::executeCommand(std::vector<std::string> tokens, Client *client)
{
	if (tokens.empty())
		return;
		
	if (!commands[tokens[0]])
	{
		std::cerr << "Error: command not found" << std::endl;
		return;
	}

	if (tokens[0] == "CAP" || tokens[0] == "PASS")
	{
		(this->*commands[tokens[0]])(tokens, client);
		return;
	}

	if ((tokens[0] == "NICK" || tokens[0] == "USER") && !client->isPass())
	{
		std::cerr << "Error: password mismatch" << std::endl;
		client->sendMessage(ERR_PASSWDMISMATCH, client->getNickname() + " :Password missmatch");
		return; 
	}
	
	if (tokens[0] != "NICK" && tokens[0] != "USER" && client->getConnectionStatus() != Client::REGISTERED)
	{
		std::cerr << "Error: client not registered" << std::endl;
		client->sendMessage(ERR_NOTREGISTERED, client->getNickname() + " :You have not registered");
		return;
	}
	
	(this->*commands[tokens[0]])(tokens, client);
}

// Execute the `welcome` protocol (home made, order 42 and all that) when a client successfully connects to the server
// (basically, send a bunch of messages to the client, like the MOTD, the server name, the version, etc.)
void Server::welcome(Client *client)
{
	client->sendMessage(RPL_WELCOME, client->getNickname() + " :Welcome " + client->getNickname() + " to the very tiny local 42 Internet Relay Network (without real network)");
	client->sendMessage(RPL_YOURHOST, client->getNickname() + " :Your host is ft_irc (k1r2p10/4242), running version 42 (i don't have a fu*king clue what this is supposed to be in that case but TRUST ME it's 42)");
	client->sendMessage(RPL_CREATED, client->getNickname() + " :This server was created on Thursday, 5th of January 2023...unless ? I don't really remember, I'm not a time traveller");
	client->sendMessage(RPL_MYINFO, client->getNickname() + " k1r2p10:4242 v42 i it");
	client->sendMessage(RPL_ISUPPORT, client->getNickname() + " CHANLIMIT=" + std::to_string(this->getConfig()->getChanlimit()) + " CHANTYPES=# CHANNELLEN=" + std::to_string(this->getConfig()->getChannellen()) + " NICKLEN=" + std::to_string(this->getConfig()->getNicklen()) + " TOPICLEN=" + std::to_string(this->getConfig()->getTopiclen()) + " USERLEN=" + std::to_string(this->getConfig()->getUserlen()) + " :are supported by this server");

	std::vector<std::string> tokens;
	tokens.push_back("MOTD");
	motd(tokens, client);	
}

// Yep, our good ol' `split` function, because why not.
std::vector<std::string> Server::split(std::string str, char c) const
{
	std::vector<std::string> result;
	std::string tmp;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == c)
		{
			result.push_back(tmp);
			tmp.clear();
		}
		else
			tmp += str[i];
	}
	result.push_back(tmp);
	return result;
}

// Check if a channel exists, as simple as that
bool	Server::existsChannel(std::string name) const
{
	if (this->getChannels().find(name) != this->getChannels().end())
		return true;
	return false;
}

// Check if a client exists, as simple as that too
bool	Server::existsClient(std::string name) const
{
	if (this->getClientsFds().find(name) != this->getClientsFds().end())
		return true;
	return false;
}

// Serious stuff here, when a client disconnects, we need to remove it from the server but no one can do that while we're iterating over the clients map, see ?
// So we need to store the clients we want to delete in a vector and delete them later, when we're sure we're not iterating over the map anymore (u'll understand later with container, trust me i'm a doctor)
void	Server::delete_to_delete(void)
{
	for (
		std::vector<std::map<int, Client *>::iterator>::iterator it = clients_to_delete.begin();
		it != clients_to_delete.end();
		it++
		)
	{
		delete (*it)->second;
		clients.erase(*it);
	}
	for (
		std::vector<std::map<std::string, int>::iterator>::iterator it = fds_to_delete.begin();
		it != fds_to_delete.end();
		it++
		)
	{
		client_fds.erase(*it);
	}
	clients_to_delete.clear();
	fds_to_delete.clear();
}

// Send a message to every client on the server, except the one who sent the message
void	Server::sendGoodByeGoodByeToEverybady(std::string source, std::string command, std::string params)
{
	for (
		std::map<int, Client *>::const_iterator it = clients.begin();
		it != clients.end();
		it++
		)
	{
		it->second->sendMessage(source, command, params);
	}
}

// Specific function used in the MODE command, do not try this at home kids
char  Server::closestPlusMinus(const std::string &str, const char &mode) const
{
    size_t lastMode = str.rfind(mode);
    if (lastMode == std::string::npos) {
        return '\0';
    }
    size_t closestPlus = str.rfind('+', lastMode);
    size_t closestMinus = str.rfind('-', lastMode);
    if (closestPlus == std::string::npos && closestMinus == std::string::npos) {
        return '+';
    }
    if (closestPlus == std::string::npos) {
        return '-';
    }
    if (closestMinus == std::string::npos) {
        return '+';
    }
    return (closestPlus > closestMinus) ? '+' : '-';
}

/*
 ******************************** ACCESSOR ************************************
 */

const Server::t_nickMapClient &Server::getClients(void) const
{
	return (clients);
}

Server::t_nickMapClient &Server::getClients(void)
{
	return (clients);
}

void Server::setClients(const Server::t_nickMapClient &src)
{
	clients = src;
}

const Server::t_nickMapFd &Server::getClientsFds(void) const
{
	return (client_fds);
}

void Server::setClientsFds(const Server::t_nickMapFd &src)
{
	client_fds = src;
}

const Server::t_nameMapChannel &Server::getChannels(void) const
{
	return (channels);
}

Server::t_nameMapChannel &Server::getChannels(void)
{
	return (channels);
}

void Server::setChannels(const Server::t_nameMapChannel &src)
{
	channels = src;
}

const Configuration *Server::getConfig(void) const
{
	return (config);
}

void Server::setConfig(const Configuration *src)
{
	delete config;
	config = new Configuration(*src);
}

const int &Server::getListeningFd(void) const
{
	return (listening_fd);
}

void Server::setListeningFd(const int &src)
{
	listening_fd = src;
}

/* ************************************************************************** */
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/29 12:30:55 by xxxxxxx           #+#    #+#             */
/*   Updated: 2023/01/02 16:42:066 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./ft_irc.hpp"

int main(int argc, char const *argv[])
{

	if (argc != 3 || argv[2][0] == '\0')
	{
		std::cerr << "Usage: ./ircserv [port] [password]" << std::endl;
		return (1);
	}	

	//Create a static server instance to avoid memory leaks
	static Server	server(argv[1], argv[2]);
	signal(SIGINT, sig_handler);

	while (1)
	{
		server.acceptConnection();
		check_all_clients(server);
	}
	return (0);
}

// Loop over all clients and check if they have sent a message, if so, process it
void	check_all_clients(Server &server)
{
	int nb_available_fd = server.executePoll();

	for (
		Server::t_clientIterator it = server.getClients().begin();
		it != server.getClients().end() && nb_available_fd > 0;
		it++
		)
	{
		// Check if there is overlap between the flags we want active and the flags really active
		// else, continue to the next client
		if (!(	
				it->second->getPollData().events		// Flags we want active
				& it->second->getPollData().revents		// Flags really active
				)
			)
			continue ;
		nb_available_fd--;
		process_client_message(it->second, server);
	}
	server.delete_to_delete();
}

// Get all messages sent by the client (in case of a splited message) and process it/them
void	process_client_message(Client *client, Server &server)
{
		std::vector<std::string> messages = get_messages(client);

		for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); it++)
		{
			std::string data = *it;
			if (data == "")
				continue ;
			std::clog << "Received message from " << client->getPollData().fd << ": " << data << std::endl;

			std::vector<std::string> tokens = split_message(data);
			server.executeCommand(tokens, client);
		}	
}

// Get all messages sent by the client in  the specific case of a splited message
std::vector<std::string>	get_messages(Client *client)
{
	std::vector<std::string> messages;
	std::string				raw_data;
	char					buff[BUFFER_SIZE] = {0};

	while (recv(client->getPollData().fd, buff,  BUFFER_SIZE, 0) > 0)
	{
		raw_data += buff;
	}
	if (raw_data == "")
		return (messages);
	
	// split raw_data along "\r\n" to split into differents messages
	std::string message;
	message = client->getBuffer();
	for (std::string::iterator it = raw_data.begin(); it != raw_data.end(); it++)
	{
		if (*it == '\r' && *(it + 1) == '\n')
		{
			messages.push_back(message);
			message.clear();
			it++;
		}
		else
			message += *it;
	}
	client->setBuffer(message); 

	return (messages);
}

// Split a message into tokens (separated by spaces, with last token cut on `:`)
std::vector<std::string>	split_message(std::string message)
{
	std::vector<std::string>	tokens;
	std::string					token;

	for (std::string::iterator it = message.begin(); it != message.end(); it++)
	{
		if (*it == ' ')
		{
			tokens.push_back(token);
			token.clear();
			while (*it == ' ')
				it++;
		}
		if (*it == ':')
		{
			token = std::string(it + 1, message.end());
			break;
		}
		else
			token += *it;
	}
	tokens.push_back(token.substr(0, token.size()));

	return (tokens);
}

// Intercept SIGINT (Ctrl+C) to gracefully exit the server UwU
void	sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		std::clog << "\nGoodbye, goodbye, to everybody..." << std::endl;
		exit(0);
	}
}
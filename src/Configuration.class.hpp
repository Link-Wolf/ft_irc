/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.class.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iCARUS <iCARUS@student.42mulhouse.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 07:50:32 by iCARUS            #+#    #+#             */
/*   Updated: 2023/01/10 15:47:11 by iCARUS           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Configuration;

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <iostream>
# include <string>

class Configuration
{
	private:
		std::string		port;
		std::string		password;
		std::string		chanmodes;
		std::string		chantypes;
		size_t			chanlimit;
		size_t			channellen;
		size_t			modes;
		size_t			nicklen;
		size_t			topiclen;
		size_t			userlen;		

	public:
		Configuration();
		Configuration(Configuration const & src);
		Configuration(std::string port, std::string password);
		~Configuration();

		Configuration		&operator=(Configuration const &rhs);

		const std::string	&getPort() const;
		void 				setPort(const std::string &port);
		const std::string	&getPassword() const;
		void 				setPassword(const std::string &password);
		const std::string	&getChanmodes() const;
		void 				setChanmodes(const std::string &chanmodes);
		const std::string	&getChantypes() const;
		size_t 				getChanlimit() const;
		size_t 				getChannellen() const;
		size_t 				getModes() const;
		size_t 				getNicklen() const;
		size_t 				getTopiclen() const;
		size_t 				getUserlen() const;
};

std::ostream				&operator<<(std::ostream &o, Configuration const &i);

#endif

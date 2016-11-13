// STL
#include <iostream>
#include <string.h>
// Boost
#include <boost/asio.hpp>

// Project
#include "client.h"

int main()
{
	try
	{
		char identifier;
		bool identifierIsValid;

		do
		{
			std::cout << "Which server to connect to? ('A' - 'E')" << std::endl;
			std::cin >> identifier;

			identifierIsValid =
				constants::identifierIsValid(identifier);

			if(!identifierIsValid)
			{
				std::cout << identifier << " is invalid. Please try again." << std::endl;
			}
		} while(!identifierIsValid);

		const uint8_t serverIndex(
			constants::charToServerIndex(identifier));

		const uint16_t listeningPort(
			constants::serverIndexToListeningPort(serverIndex));

		std::string username("");
		std::cout << "Enter your username: " << std::endl;
		std::getline(std::cin, username);
		boost::asio::io_service ioService;

		client clientInstance(
			username, 
			listeningPort,
			ioService);

		clientInstance.run();
	}
	catch(std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	return 0;
}
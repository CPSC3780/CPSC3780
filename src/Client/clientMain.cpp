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

		std::string ignore("");
		
		// necessary since last was cin next is getline
		std::getline(std::cin, ignore);

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
			serverIndex,
			ioService);

		clientInstance.run();
	}
	catch(std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	return 0;
}
// STL
#include <iostream>
#include <cstdint>

// Boost
#include <boost/asio.hpp>

// Project
#include "server.h"

int main()
{
	try
	{
		char identifier;
		bool identifierIsValid;

		do 
		{
			std::cout << "Which server instance to launch? ('A' - 'E')" << std::endl;
			std::cin >> identifier;

			identifierIsValid = 
				constants::identifierIsValid(identifier);

			if(!identifierIsValid)
			{
				std::cout << identifier << " is invalid. Please try again." << std::endl;
			}
		} while (!identifierIsValid);

		uint16_t listeningPort;
		uint8_t serverIndex;
		
		constants::serverIdentifierToPortNumberAndIndex(
			identifier,
			listeningPort,
			serverIndex);

		boost::asio::io_service ioService;

		server serverInstance(
			listeningPort,
			serverIndex,
			ioService);

		serverInstance.run();
	}
	catch(std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	return 0;
}
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
		std::string username("");
		std::cout << "Enter your username: " << std::endl;
		std::cin >> username;
		boost::asio::io_service ioService;

		client clientInstance(username, ioService);

		clientInstance.run();
	}
	catch(std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	return 0;
}
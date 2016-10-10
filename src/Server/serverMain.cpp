// STL
#include <iostream>

// Boost
#include <boost/asio.hpp>

// Project
#include "server.h"

int main()
{
	try
	{
		boost::asio::io_service ioService;

		server serverInstance(ioService);

		serverInstance.run();
	}
	catch(std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	return 0;
}
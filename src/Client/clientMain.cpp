// STL
#include <iostream>

// Boost
#include <boost/asio.hpp>

// Project
#include "client.h"

int main()
{
	try
	{
		boost::asio::io_service ioService;

		client clientInstance(ioService);

		clientInstance.run();
	}
	catch(std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	return 0;
}
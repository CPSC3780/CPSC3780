// as per, http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/tutorial/tutdaytime6.html

// STL
#include <iostream>
#include <cstdint>

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
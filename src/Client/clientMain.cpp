// as per, http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/tutorial/tutdaytime4.html

// STL
#include <iostream>
#include <cstdint>

// Boost
#include <boost/asio.hpp>

// Project
#include "client.h"

int main()
{
	try
	{
		// Need to instantiate an io_service object for I/O functionality with asio 
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
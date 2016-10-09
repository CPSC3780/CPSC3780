// as per, http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/tutorial/tutdaytime6.html
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <cstdint>

using boost::asio::ip::udp;

void receiveConnectionsLoop(
	udp::socket& socket);

// TODO: this should also be a class
int main()
{
	// specify port the server listens on
	const int port = 8080;
	std::cout << "Server started at: " << std::endl;

	try
	{
		boost::asio::io_service ioService; 

		// create a UDP socket to receive requests on specified
		udp::socket socket(ioService, udp::endpoint(udp::v4(), port));

		boost::thread_group threads;

		// thread for receiving connections
		threads.create_thread(
			boost::bind(receiveConnectionsLoop, boost::ref(socket)));

		threads.join_all();

	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

void receiveConnectionsLoop(
	udp::socket& socket)
{

	while(true)
	{
		// Wait for connection
		boost::array<char, 1> recv_buf;
		udp::endpoint remote_endpoint;
		boost::system::error_code error;

		// remote_endpoint object is populated by receive_from()
		socket.receive_from(boost::asio::buffer(recv_buf),
			remote_endpoint, 0, error);

		if(error && error != boost::asio::error::message_size)
		{
			throw boost::system::system_error(error);
		}

		// send a warm welcome :)
		std::string message = "Hello, how are you!";
		boost::system::error_code ignored_error;
		socket.send_to(boost::asio::buffer(message),
			remote_endpoint, 0, ignored_error);
	}
}
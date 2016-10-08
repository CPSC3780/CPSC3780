// as per, http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/tutorial/tutdaytime4.html
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::udp;

int main()
{
	try
	{
		// Specify the host and port of the server to connect to
		// just hardcode for now
		const std::string host = "localhost";
		const std::string port = "8080";

		// Need to instantiate an io_service object for I/O functionality with asio 
		boost::asio::io_service io_service;

		// find a list of possible remote endpoints using resolver()
		udp::resolver resolver(io_service);
		udp::resolver::query query(udp::v4(), host, port);
		//  return server endpoint
		udp::endpoint server_endpoint = *resolver.resolve(query);

		// Create a UDP socket to create connection with server
		udp::socket socket(io_service);
		socket.open(udp::v4());

		// initiate connection with the server
		boost::array<char, 1> send_buf = {{0}};
		socket.send_to(boost::asio::buffer(send_buf), server_endpoint);

		// Listen for any data the server sends back
		boost::array<char, 128> recv_buf;
		udp::endpoint sender_endpoint;
		size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

		// output data
		std::cout.write(recv_buf.data(), len);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
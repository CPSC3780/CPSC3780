// as per, http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/tutorial/tutdaytime4.html
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <cstdint>

using boost::asio::ip::udp;

bool exitClient = false;
uint64_t updateInterval = 100;

void sendLoop(
	udp::socket& socket,
	const udp::endpoint& serverEndPoint);

void receiveLoop(
	udp::socket& socket);

// TODO: client should be a class
int main()
{
	try
	{
		// Specify the host and port of the server to connect to
		// just hardcode for now
		const std::string host = "localhost";
		const std::string port = "8080";

		// Need to instantiate an io_service object for I/O functionality with asio 
		boost::asio::io_service ioService;

		// find a list of possible remote endpoints using resolver()
		udp::resolver resolver(ioService);
		udp::resolver::query query(udp::v4(), host, port);

		//  return server endpoint
		udp::endpoint serverEndPoint = 
			*resolver.resolve(query);

		// Create a UDP socket for communication with server
		udp::socket socket(ioService);
		socket.open(udp::v4());

		boost::array<char, 1> send_buf = {{0}};
		socket.send_to(boost::asio::buffer(send_buf), serverEndPoint);

		boost::thread_group threads;

		// thread for sending messages to the server
		threads.create_thread(
			boost::bind(sendLoop, boost::ref(socket), boost::cref(serverEndPoint)));

		// thread for receiving messages
		threads.create_thread(
			boost::bind(receiveLoop, boost::ref(socket)));

		threads.join_all();
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}

void sendLoop(
	udp::socket& socket,
	const udp::endpoint& serverEndPoint)
{
	std::string message("");

	while(!exitClient)
	{
		// communication with the server
		std::cout << "Enter a message: " << std::endl;
		std::cin >> message;

		if(message == "exit")
		{
			exitClient = true;
			break;
		}
		else
		{
			socket.send_to(
				boost::asio::buffer(message, message.size()), 
				serverEndPoint);
		}
	}
}

void receiveLoop(
	udp::socket& socket)
{
	udp::endpoint senderEndPoint;

	while(!exitClient)
	{
		try
		{
			// Listen for any data the server endpoint sends back
			boost::array<char, 128> recv_buf;

			size_t incomingMessageLength =
				socket.receive_from(boost::asio::buffer(recv_buf), senderEndPoint);

			if(incomingMessageLength > 0)
			{
				// output data
				std::cout.write(
					recv_buf.data(),
					incomingMessageLength);
			}

			// sleep
			boost::this_thread::sleep(boost::posix_time::millisec(updateInterval));
		}
		catch(std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

	}
}
// STL
#include <cassert>
#include <string.h>

// Boost
#include <boost/array.hpp>

// Project
#include "client.h"

using boost::asio::ip::udp;

const uint16_t updateInterval = 100;

// #TODO_AH fill in implementation notes

//------------------------------------------------------------------ constructor
// Implementation notes:
//  None
//------------------------------------------------------------------------------
client::client(
	boost::asio::io_service& ioService) :
	m_resolver(ioService),
	m_UDPsocket(ioService)
{
	this->m_terminate = false;

	this->m_activeProtocol =
		client::protocol::UDP;

	const std::string host = "localhost";
	const std::string port = "8080";

	udp::resolver::query serverQuery(
		udp::v4(),
		host,
		port);

	this->m_serverEndPoint =
		*this->m_resolver.resolve(serverQuery);

	this->m_UDPsocket.open(
		udp::v4());

	// necessary? getting a crash later when trying to receive if removed
	std::string initiateMessage = "Client has connected.\n";
	m_UDPsocket.send_to(
		boost::asio::buffer(initiateMessage), m_serverEndPoint);
};

//-------------------------------------------------------------------------- run
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::run()
{
	// thread for input/sending messages
	m_threads.create_thread(
		boost::bind(&client::inputLoop, this));

	// thread for receiving messages
	m_threads.create_thread(
		boost::bind(&client::receiveLoop, this));

	m_threads.join_all();
};

//-------------------------------------------------------------------- inputLoop
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::inputLoop()
{
	while(!this->m_terminate)
	{
		std::string message("");

		// communication with the server
		std::cout << "Enter a message: " << std::endl;
		std::getline(std::cin, message);

		if(message == "/exit")
		{
			this->m_terminate = true;
			std::string disconnect_message = "Client has disconnected.";
			this->sendOverUDP(disconnect_message);
			break;
		}
		else
		{
			// Branch on protocol
			switch(this->m_activeProtocol)
			{
				case client::protocol::UDP:
				{
					this->sendOverUDP(message);
					break;
				}
				case client::protocol::Bluetooth:
				{
					this->sendOverBluetooth(message);
					break;
				}
				case client::protocol::Undefined:
				default:
				{
					assert(false);
					break;
				}
			}
		}
	}
};

//------------------------------------------------------------------ sendOverUDP
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::sendOverUDP(
	const std::string& message)
{
	m_UDPsocket.send_to(
		boost::asio::buffer(message, message.size()),
		m_serverEndPoint);
};

//------------------------------------------------------------ sendOverBluetooth
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::sendOverBluetooth(
	const std::string& message)
{
	// #TODO implement sending over Bluetooth for the client
};

//------------------------------------------------------------------ receiveLoop
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::receiveLoop()
{
	while(!this->m_terminate)
	{
		this->receiveOverUDP();
		this->receiveOverBluetooth();
	}
};

//--------------------------------------------------------------- receiveOverUDP
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::receiveOverUDP()
{
	try
	{
		// Listen for any data the server endpoint sends back
		boost::array<char, 128> recv_buf; // make this a member variable? how do we determine size?

		size_t incomingMessageLength =
			m_UDPsocket.receive_from(
				boost::asio::buffer(recv_buf),
				this->m_serverEndPoint);

		if(incomingMessageLength > 0)
		{
			// output data
			std::cout.write(
				recv_buf.data(),
				incomingMessageLength);
		}

		std::cout << std::endl;

		// sleep
		boost::this_thread::sleep(
			boost::posix_time::millisec(updateInterval));
	}
	catch(std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}
};

//--------------------------------------------------------- receiveOverBluetooth
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::receiveOverBluetooth()
{
	// #TODO implement receiving over Bluetooth for the client
};

// STL
#include <cassert>
#include <string.h>
#include <vector>

// Boost
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>

// Project
#include "client.h"
#include "../Common/dataMessage.h"

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
	std::string initiateMessage = "Client at has connected.\n";
	std::string source = "test";
	std::string destination = "broadcast";
	// dataMessage currentMessage(initiateMessage, "", "");

	// TODO_MT: we need to send as vector data through the buffer so we can send everything in one go

	const uint16_t arbitraryLength = 256;

	std::vector<char> payloadToSend(initiateMessage.begin(), initiateMessage.end());
	std::vector<char> sourceToSend(source.begin(), source.end());
	std::vector<char> destinationToSend(destination.begin(), destination.end());

	boost::array<boost::asio::const_buffer, 3> buffersToSend = {
		boost::asio::buffer(payloadToSend),
		boost::asio::buffer(sourceToSend),
		boost::asio::buffer(destinationToSend)};

	this->m_UDPsocket.send_to(
		buffersToSend,
		m_serverEndPoint);
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
		// TODO_MT: we need to send vector data through the buffer so we can send everything in one go
		dataMessage currentMessage(message, "", "");

		if(currentMessage.viewPayload() == "/exit")
		{
			this->m_terminate = true;
			std::string disconnect_message = "<clientIDGoesHere> has disconnected."; // #TODO_AH implement as member variable of client
			dataMessage currentMessage(disconnect_message, "", "");

			this->sendOverUDP(currentMessage);
			break;
		}
		else
		{
			// Branch on protocol
			switch(this->m_activeProtocol)
			{
				case client::protocol::UDP:
				{
					// TODO_MT: we need to send as vector data through the buffer so we can send everything in one go
					this->sendOverUDP(currentMessage);
					break;
				}
				case client::protocol::Bluetooth:
				{
					this->sendOverBluetooth(currentMessage);
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
	const dataMessage& message)
{
	this->m_UDPsocket.send_to(
		message.asConstBuffer(),
		this->m_serverEndPoint);
};

//------------------------------------------------------------ sendOverBluetooth
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::sendOverBluetooth(
	const dataMessage& message)
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
		boost::array<boost::asio::mutable_buffer, 3> recv_buf; // #TODO_AH make this a member variable

		size_t incomingMessageLength =
			this->m_UDPsocket.receive_from(
				recv_buf,
				this->m_serverEndPoint);

		if(incomingMessageLength > 0)
		{
			// output data
			/* #TODO_AH implement interpretation of 3 buffers
			std::cout.write(
				recv_buf.data(),
				incomingMessageLength);*/
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

//--------------------------------------------------------- serializeUDPMessage
// Implementation notes:
//  TODO
//------------------------------------------------------------------------------
void client::serializeUDPMessage(std::vector<std::string>)
{
	// #TODO implement receiving over Bluetooth for the client
};
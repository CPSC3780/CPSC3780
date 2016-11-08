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
#include "../Common/constants.h"

using boost::asio::ip::udp;

const uint16_t updateInterval = 100;

// #TODO_AH fill in implementation notes

//------------------------------------------------------------------ constructor
// Implementation notes:
//  None
//------------------------------------------------------------------------------
client::client(
	const std::string& inUsername,
	boost::asio::io_service& ioService) :
	m_resolver(ioService),
	m_UDPsocket(ioService)
{
	this->m_username = inUsername;
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

	std::string initiateMessage = this->m_username + " has connected.";
	std::string source = this->m_username;
	std::string destination = "broadcast";

	dataMessage connectionMessage(
		initiateMessage,
		source,
		destination,
		constants::CONNECTION);

	this->sendOverUDP(
		connectionMessage);
};

//-------------------------------------------------------------------------- run
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::run()
{
	// thread for input/sending messages
	this->m_threads.create_thread(
		boost::bind(&client::inputLoop, this));

	// thread for receiving messages
	this->m_threads.create_thread(
		boost::bind(&client::receiveLoop, this));

	this->m_threads.join_all();
};

//-------------------------------------------------------------------- inputLoop
// Implementation notes:
//  None
//------------------------------------------------------------------------------
void client::inputLoop()
{
	while(!this->m_terminate)
	{
		std::string chatInput("");

		// communication with the server
		std::cout << "Enter a message: " << std::endl;
		std::getline(std::cin, chatInput);

		// By default, destination and message type are "broadcast"
		// and "chat", respectively
		std::string destination = "broadcast";
		int messageType = constants::CHAT;

		std::stringstream ss;
		ss << chatInput;

		std::string temp("");
		ss >> temp;

		if(temp == "/message")
		{
			std::string actualMessage("");
			ss >> destination;
			
			chatInput = chatInput.substr(chatInput.find(destination) + destination.length());
			messageType = constants::PRIVATE;
		}

		dataMessage currentMessage(
			chatInput,
			this->m_username,
			destination,
			messageType);

		if(currentMessage.viewPayload() == "/exit")
		{
			this->m_terminate = true;

			std::string disconnectMessage =
				this->m_username + " has disconnected.";

			dataMessage currentMessage(
				disconnectMessage,
				this->m_username,
				destination,
				constants::DISCONNECT);

			this->sendOverUDP(currentMessage);

			continue;
		}
		else
		{
			// Branch on protocol
			switch(this->m_activeProtocol)
			{
				case client::protocol::UDP:
				{
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
		boost::asio::buffer(message.asCharVector()),
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
		const uint16_t arbitraryLength = 256;
		// Listen for any data the server endpoint sends back

		std::vector<char> receivedMessage(arbitraryLength);

		size_t incomingMessageLength =
			this->m_UDPsocket.receive_from(
				boost::asio::buffer(receivedMessage),
				this->m_serverEndPoint);

		dataMessage message(
			receivedMessage);

		if(incomingMessageLength > 0)
		{
			if((message.viewMessageTypeAsString() != "connection")
				&&(message.viewMessageTypeAsString() != "disconnect"))
			{
				std::cout << message.viewSourceID() << " says: ";
			}

			std::cout << message.viewPayload() << std::endl;
		}

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
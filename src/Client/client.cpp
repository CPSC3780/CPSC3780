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

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Initializes the client and establishes a connection with the server.
//------------------------------------------------------------------------------
client::client(
	const std::string& inUsername,
	const uint16_t& inServerPort,
	const int8_t& inServerIndex,
	boost::asio::io_service& ioService) :
	m_resolver(ioService),
	m_UDPsocket(ioService),
	m_serverPort(inServerPort)
{
	this->m_username = inUsername;
	this->m_serverIndex = inServerIndex;
	this->m_terminate = false;

	this->m_activeProtocol =
		client::protocol::UDP;

	// #TODO can be hardcoded to lan ip, should make this dynamic
	const std::string serverAddress = 
		constants::serverHostName(this->m_serverIndex);

	const std::string serverPort =
		std::to_string(inServerPort);

	boost::asio::ip::udp::resolver::query serverQuery(
		boost::asio::ip::udp::v4(),
		serverAddress,
		serverPort);

	this->m_serverEndPoint =
		*this->m_resolver.resolve(serverQuery);

	this->m_UDPsocket.open(
		boost::asio::ip::udp::v4());

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
//  Creates the various threads for the necessary loops. These threads loop
//  until a terminate condition is reached.
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
//  Parses the user input from the command line, branches to different areas
//  based on the parsed /command, if no /command found, broadcast the message
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
		constants::MessageType messageType = constants::MessageType::CHAT;

		std::stringstream ss;
		ss << chatInput;

		std::string temp("");
		ss >> temp;

		if(temp == "/message")
		{
			std::string actualMessage("");
			ss >> destination;
			
			std::getline(ss, chatInput);
			messageType = constants::MessageType::PRIVATE_MESSAGE;
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
				case client::protocol::BLUETOOTH:
				{
					this->sendOverBluetooth(currentMessage);
					break;
				}
				case client::protocol::UNDEFINED:
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
//  Sends a message to the sever over UDP
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
//  Sends a message to the server over Bluetooth
//------------------------------------------------------------------------------
void client::sendOverBluetooth(
	const dataMessage& message)
{
	// #TODO implement sending over Bluetooth for the client
};

//------------------------------------------------------------------ receiveLoop
// Implementation notes:
//  Listen for messages from the server
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
//  Listen for any messages the server sends back over UDP
//------------------------------------------------------------------------------
void client::receiveOverUDP()
{
	try
	{
		const uint16_t arbitraryLength = 256;

		std::vector<char> receivedMessage(arbitraryLength);

		size_t incomingMessageLength =
			this->m_UDPsocket.receive_from(
				boost::asio::buffer(receivedMessage),
				this->m_serverEndPoint);

		dataMessage message(
			receivedMessage);

		if(incomingMessageLength > 0)
		{
			constants::MessageType messageType = 
				message.viewMessageType();

			switch(messageType)
			{
				case constants::MessageType::CONNECTION:
				{
					// Do nothing
					break;
				}
				case constants::MessageType::PRIVATE_MESSAGE:
				{
					std::cout << "Private Message: " << message.viewSourceIdentifier()
						<< " says: " << message.viewPayload() << std::endl;
					break;
				}
				case constants::MessageType::DISCONNECT:
				{
					// Do nothing
					break;
				}
				case constants::MessageType::CHAT:
				{
					std::cout << message.viewSourceIdentifier() << " says: "
						<< message.viewPayload() << std::endl;
					break;
				}
				case constants::MessageType::SYNC_LEFT:
				case constants::MessageType::SYNC_RIGHT:
				{
					// Syncs are only used by servers, never by clients
					assert(false);
					break;
				}
				case constants::MessageType::PING:
				{
					// Do nothing
					break;
				}
				default:
				{
					// Programming error, unexpected type
					assert(false);
				}
			}
		}

		// sleep
		boost::this_thread::sleep(
			boost::posix_time::millisec(
			constants::updateIntervalMilliseconds));
	}
	catch(std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}
};

//--------------------------------------------------------- receiveOverBluetooth
// Implementation notes:
//  Listen for any messages the server sends back over Bluetooth
//------------------------------------------------------------------------------
void client::receiveOverBluetooth()
{
	// #TODO implement receiving over Bluetooth for the client
};
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
	m_serverPort(inServerPort),
	m_terminate(false),
	m_sequenceNumber(0)
{
	this->m_username = inUsername;
	this->m_serverIndex = inServerIndex;

	this->m_activeProtocol =
		client::Protocol::p_UDP;

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

	std::string destination = constants::serverIndexToServerName(this->m_serverIndex);
	std::string initiateMessage = this->m_username + " has connected.";

	dataMessage connectionMessage(
		this->sequenceNumber(),
		constants::mt_CLIENT_CONNECT,
		this->m_username,
		destination,
		initiateMessage);

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
	// thread for getting the server to relay messages to this client
	this->m_threads.create_thread(
		boost::bind(&client::getLoop, this));

	// thread for input/sending messages
	this->m_threads.create_thread(
		boost::bind(&client::inputLoop, this));

	// thread for receiving messages
	this->m_threads.create_thread(
		boost::bind(&client::receiveLoop, this));

	this->m_threads.join_all();
}

//---------------------------------------------------------------------- getLoop
// Implementation notes:
//  The client periodically sends a get to the server, which will cause the
//  server to send all messages destined for this client.
//------------------------------------------------------------------------------
void client::getLoop()
{
	while(!this->m_terminate)
	{
		try
		{
			dataMessage connectionMessage(
				this->sequenceNumber(),
				constants::mt_CLIENT_GET,
				this->m_username,
				constants::serverIndexToServerName(this->m_serverIndex),
				"blank");

			this->sendOverUDP(
				connectionMessage);
		}
		catch(std::exception& exception)
		{
			std::cout << exception.what() << std::endl;
		}

		// sleep
		boost::this_thread::sleep(
			boost::posix_time::millisec(
			constants::updateIntervalMilliseconds));
	}
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
		constants::MessageType messageType = constants::MessageType::mt_UNDEFINED;

		std::stringstream ss;
		ss << chatInput;

		std::string temp("");
		ss >> temp;

		if((temp == "/message") || (temp == "/m"))
		{
			std::string actualMessage("");
			ss >> destination;
			
			std::getline(ss, chatInput);
			messageType = constants::MessageType::mt_CLIENT_SEND;
		}
		else
		{
			std::cout << "Invalid command. (Use '/m' || '/message' <target> <message>" << std::endl;
			continue;
		}

		dataMessage currentMessage(
			this->sequenceNumber(),
			messageType,
			this->m_username,
			destination,
			chatInput);

		if(currentMessage.viewPayload() == "/exit")
		{
			this->m_terminate = true;

			std::string disconnectMessage =
				this->m_username + " has disconnected.";

			dataMessage currentMessage(
				this->sequenceNumber(),
				constants::MessageType::mt_CLIENT_DISCONNECT,
				this->m_username,
				destination,
				disconnectMessage);

			this->sendOverUDP(currentMessage);
		}
		else
		{
			// Branch on protocol
			switch(this->m_activeProtocol)
			{
				case client::Protocol::p_UDP:
				{
					this->sendOverUDP(currentMessage);
					break;
				}
				case client::Protocol::p_BLUETOOTH:
				{
					this->sendOverBluetooth(currentMessage);
					break;
				}
				case client::Protocol::p_UNDEFINED:
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
				case constants::MessageType::mt_UNDEFINED:
				{
					assert(false);
					break;
				}
				case constants::MessageType::mt_CLIENT_CONNECT:
				{
					assert(false);
					break;
				}
				case constants::MessageType::mt_CLIENT_DISCONNECT:
				{
					assert(false);
					break;
				}
				case constants::MessageType::mt_CLIENT_SEND:
				{
					assert(false);
					break;
				}
				case constants::MessageType::mt_CLIENT_GET:
				{
					assert(false);
					break;
				}
				case constants::MessageType::mt_CLIENT_ACK:
				{
					assert(false);
					break;
				}
				case constants::MessageType::mt_SERVER_SEND:
				{
					std::cout << message.viewSourceIdentifier()
						<< " says: " << message.viewPayload() << std::endl;

					dataMessage ackMessage(
						message.viewSequenceNumber(),
						constants::mt_CLIENT_ACK,
						this->m_username,
						constants::serverIndexToServerName(this->m_serverIndex),
						"blank");

					this->sendOverUDP(ackMessage);
					break;
				}
				case constants::MessageType::mt_SERVER_ACK:
				{
					assert(false);
					break;
				}
				case constants::MessageType::mt_SERVER_SYNC:
				{
					// Syncs are only used by servers, never by clients
					assert(false);
					break;
				}
				case constants::MessageType::mt_PING:
				{
					// #TODO necessary?
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

//--------------------------------------------------------------- sequenceNumber
// Implementation notes:
//  Increments the sequence number every time it is used, self explanatory.
//------------------------------------------------------------------------------
const int64_t& client::sequenceNumber()
{
	return ++this->m_sequenceNumber;
};
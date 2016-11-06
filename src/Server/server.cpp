// STL
#include <cstdint>
#include <iostream>

// Boost
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

// Project
#include "server.h"

using boost::asio::ip::udp;

const uint16_t listeningPort = 8080;

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Initializes the socket with the ioService and the lisentingPort, and sets
//  the terminate condition to false.
//------------------------------------------------------------------------------
server::server(
	boost::asio::io_service& ioService) :
	m_UDPsocket(ioService, udp::endpoint(udp::v4(), listeningPort))
{
	this->m_terminate = false;

	std::cout << "Server started." << std::endl;
	std::cout << "Listening on port: " << listeningPort << std::endl;
};

//-------------------------------------------------------------------------- run
// Implementation notes:
//  Creates the various threads for the necessary loops. These threads loop
//  until a terminate condition is reached.
//------------------------------------------------------------------------------
void server::run()
{
	// thread for receiving connections
	this->m_threads.create_thread(
		boost::bind(&server::listenLoop, this));

	// thread for relaying messages with various protocols
	this->m_threads.create_thread(
		boost::bind(&server::relayLoop, this));

	this->m_threads.join_all();
};

//------------------------------------------------------------------- listenLoop
// Implementation notes:
//  Listen for connections and messages
//------------------------------------------------------------------------------
void server::listenLoop()
{
	while(!this->m_terminate)
	{
		const uint16_t arbitraryLength = 256;

		// Wait for connection
		std::vector<char> receivedPayload(arbitraryLength);

		boost::system::error_code error;

		// remote_endpoint object is populated by receive_from()
		this->m_UDPsocket.receive_from(
			boost::asio::buffer(receivedPayload),
			this->m_remoteEndPoint, 0, error);

		if(error && error != boost::asio::error::message_size)
		{
			throw boost::system::system_error(error);
		}

		dataMessage message(
			receivedPayload);

		std::cout << "Received " << message.viewMessageType();
		std::cout << " message from " << message.viewSourceID() << std::endl;

		if(message.viewMessageType() == "disconnect")
		{
			this->removeConnection(
				message.viewSourceID(),
				this->m_remoteEndPoint);
		}

		if(message.viewMessageType() == "connection")
		{
			this->addConnection(
				message.viewSourceID(),
				this->m_remoteEndPoint);
		}

		if((message.viewMessageType() != "connection") 
			&& (message.viewMessageType() != "disconnect"))
		{
			// #TODO decide if test code or if we should keep
			std::cout << "Message: " << message.viewPayload() << std::endl;
			std::cout << "Target: " << message.viewDestinationID() << std::endl;
		}

		this->addToMessageQueue(
			message);
	}
};

//-------------------------------------------------------------------- relayLoop
// Implementation notes:
//  Relays messages received to all clients connected to this server instance
//  through various protocols
//------------------------------------------------------------------------------
void server::relayLoop()
{
	while(!this->m_terminate)
	{
		this->relayUDP();
		this->relayBluetooth();
	}
}

//--------------------------------------------------------------------- relayUDP
// Implementation notes:
//  Relay over UDP
//------------------------------------------------------------------------------
void server::relayUDP()
{
	boost::system::error_code ignoredError;

	while(!(this->m_messageQueue.empty()))
	{
		const dataMessage& currentMessage(
			this->m_messageQueue.front());

		if(currentMessage.viewDestinationID() == "broadcast")
		{
			// #TODO_AH pair is kinda ugly, maybe make this a class? rename client to something else?
			for(const std::pair<std::string, boost::asio::ip::udp::endpoint> currentClient : this->m_connectedClients)
			{
				if(currentClient.first == currentMessage.viewSourceID())
				{
					// Continue so we don't relay the sent message back to the sender
					continue;
				}

				this->m_UDPsocket.send_to(
					boost::asio::buffer(currentMessage.asCharVector()),
					currentClient.second, 0, ignoredError);
			}
		}
		else
		{
			// search connectedClients.first for currentMessage.destination
		}

		this->m_messageQueue.pop();
	}
};

//--------------------------------------------------------------- relayBluetooth
// Implementation notes:
//  Relay over Bluetooth
//------------------------------------------------------------------------------
void server::relayBluetooth()
{
	// #TODO implement Bluetooth relay
};

//---------------------------------------------------------------- addConnection
// Implementation notes:
//  Add a new connection to the connections list
//------------------------------------------------------------------------------
void server::addConnection(
	const std::string& clientID,
	const boost::asio::ip::udp::endpoint& client)
{
	this->m_connectedClients.push_back(
		std::make_pair(clientID, client));
};

//------------------------------------------------------------- removeConnection
// Implementation notes:
//  Remove the matching connection from the connections list
//------------------------------------------------------------------------------
void server::removeConnection(
	const std::string& clientID,
	const boost::asio::ip::udp::endpoint& client)
{
	// #TODO_AH implement, make sure it sends a disconnect message to all
};

//------------------------------------------------------------ addToMessageQueue
// Implementation notes:
//  Add a new message to the message queue
//------------------------------------------------------------------------------
void server::addToMessageQueue(
	const dataMessage& message)
{
	this->m_messageQueue.push(
		message);
};
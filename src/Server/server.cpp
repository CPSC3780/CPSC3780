// STL
#include <cstdint>
#include <iostream>

// Boost
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

// Project
#include "server.h"
#include "../Common/constants.h"

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Initializes the server based on the specified listeningPort
//------------------------------------------------------------------------------
server::server(
	const uint16_t& inListeningPort,
	const uint8_t& inServerIndex,
	boost::asio::io_service& ioService) :
	m_UDPsocket(
		ioService,
		boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(),
		inListeningPort)),
	m_index(inServerIndex),
	m_terminate(false),
	m_leftAdjacentServerConnection(nullptr),
	m_leftAdjacentServerConnectedClients({}),
	m_rightAdjacentServerConnection(nullptr),
	m_rightAdjacentServerConnectedClients({})
{
	const std::string serverName(
		constants::serverIndexToServerName(inServerIndex));

	std::cout << serverName << " server started." << std::endl;
	std::cout << "Listening on port: " << inListeningPort << std::endl;
};

//------------------------------------------------------------------- destructor
// Implementation notes:
//  Delete adjacent server connections
//------------------------------------------------------------------------------
server::~server()
{
	delete this->m_leftAdjacentServerConnection;
	delete this->m_rightAdjacentServerConnection;
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

	// thread for syncing adjacent servers
	this->m_threads.create_thread(
		boost::bind(&server::sendClientsToAdjacentServers, this));

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

		std::vector<char> receivedPayload(arbitraryLength);

		boost::system::error_code error;

		boost::asio::ip::udp::endpoint clientEndpoint;

		// receive_from() populates the client endpoint
		this->m_UDPsocket.receive_from(
			boost::asio::buffer(receivedPayload),
			clientEndpoint, 0, error);

		if(error && error != boost::asio::error::message_size)
		{
			throw boost::system::system_error(error);
		}

		dataMessage message(
			receivedPayload);

		std::cout << "Received " << message.viewMessageTypeAsString();
		std::cout << " message from " << message.viewSourceIdentifier() << std::endl;

		switch(message.viewMessageType())
		{
			case constants::MessageType::CONNECTION:
			{
				this->addClientConnection(
					message.viewSourceIdentifier(),
					clientEndpoint);
				break;
			}
			case constants::MessageType::DISCONNECT:
			{
				this->removeClientConnection(
					message.viewSourceIdentifier());
				break;
			}
			case constants::MessageType::CHAT:
			{
				// Do nothing
				break;
			}
			case constants::MessageType::PRIVATE_MESSAGE:
			{
				// Do nothing
				break;
			}
			case constants::MessageType::SYNC_RIGHT:
			case constants::MessageType::SYNC_LEFT:
			{
				this->receiveClientsFromAdjacentServers(
					message);
				break;
			}
			default:
			{
				assert(false);
			}
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
};

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

		if(currentMessage.viewDestinationIdentifier() == "broadcast")
		{
			// If broadcast, send to all connected clients except sender
			for(const remoteConnection& targetClient : this->m_connectedClients)
			{
				if(targetClient.viewIdentifier() != currentMessage.viewSourceIdentifier())
				{
					this->m_UDPsocket.send_to(
						boost::asio::buffer(currentMessage.asCharVector()),
						targetClient.viewEndpoint(), 0, ignoredError);
				}
			}
		}
		else
		{
			// if not broadcast, it's a private message, send only to the matched client
			for(const remoteConnection& targetClient : this->m_connectedClients)
			{
				if(targetClient.viewIdentifier() == currentMessage.viewDestinationIdentifier())
				{
					this->m_UDPsocket.send_to(
						boost::asio::buffer(currentMessage.asCharVector()),
						targetClient.viewEndpoint(), 0, ignoredError);
					break;
				}
			}
		}

		// Message sent, remove from queue
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
}

//------------------------------------------------- sendClientsToAdjacentServers
// Implementation notes:
//  Sends the list of clients connected to this server to the adjacent servers.
//------------------------------------------------------------------------------
void server::sendClientsToAdjacentServers()
{
	while(!this->m_terminate)
	{
		// left adjacent server (if it exists)

		// right adjacent server (if it exists)

		// sleep
		boost::this_thread::sleep(
			boost::posix_time::millisec(
			constants::syncIntervalMilliseconds));
	}
}

//------------------------------------------------- sendClientsToAdjacentServers
// Implementation notes:
//  Receives the list of clients from an adjacent server and stores them.
//------------------------------------------------------------------------------
void server::receiveClientsFromAdjacentServers(
	const dataMessage& inSyncMessage)
{
	switch(inSyncMessage.viewMessageType())
	{
		case constants::MessageType::SYNC_RIGHT:
		{
			this->m_rightAdjacentServerConnectedClients =
				inSyncMessage.viewServerSyncPayload();
			break;
		}
		case constants::MessageType::SYNC_LEFT:
		{
			this->m_leftAdjacentServerConnectedClients =
				inSyncMessage.viewServerSyncPayload();
			break;
		}
		default
		{
			// should never make it here, programming error if this happens
			assert(false);
			break;
		}

	}
};

//---------------------------------------------------------- addClientConnection
// Implementation notes:
//  Adds a new client connection to the connections list
//------------------------------------------------------------------------------
void server::addClientConnection(
	const std::string& inClientUsername,
	const boost::asio::ip::udp::endpoint& inClientEndpoint)
{
	this->m_connectedClients.push_back(
		remoteConnection(inClientUsername, inClientEndpoint));
};

//------------------------------------------------------- removeClientConnection
// Implementation notes:
//  Remove the matching client connection from the connections list
//------------------------------------------------------------------------------
void server::removeClientConnection(
	const std::string& inClientUsername)
{
	for(std::vector<remoteConnection>::iterator currentClient = this->m_connectedClients.begin(); 
		currentClient != this->m_connectedClients.end(); 
		++currentClient)
	{
		if(currentClient->viewIdentifier() == inClientUsername)
		{
			this->m_connectedClients.erase(currentClient);
		}
		break;
	}
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
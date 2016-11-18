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
	const int8_t& inServerIndex,
	boost::asio::io_service& ioService) :
	m_resolver(ioService),
	m_ioService(&ioService),
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
				continue;
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
		while(!(this->m_messageQueue.empty()))
		{
			const dataMessage& inMessageToSend(
				this->m_messageQueue.front());

			this->relayUDP(inMessageToSend);
			this->relayBluetooth(inMessageToSend);

			// Message sent over both protocols, remove from queue
			this->m_messageQueue.pop();
		}
	}
};

//--------------------------------------------------------------------- relayUDP
// Implementation notes:
//  Relay over UDP
//------------------------------------------------------------------------------
void server::relayUDP(
	const dataMessage& inMessageToSend)
{
	boost::system::error_code ignoredError;

	if(inMessageToSend.viewDestinationIdentifier() == "broadcast")
	{
		// If broadcast, send to all connected clients except sender
		for(const remoteConnection& targetClient : this->m_connectedClients)
		{
			if(targetClient.viewIdentifier() != inMessageToSend.viewSourceIdentifier())
			{
				this->m_UDPsocket.send_to(
					boost::asio::buffer(inMessageToSend.asCharVector()),
					targetClient.viewEndpoint(), 0, ignoredError);
			}
		}
	}
	else
	{
		// if not broadcast, it's a private message, send only to the matched client

		// check list of directly connect clients first
		for(const remoteConnection& targetClient : this->m_connectedClients)
		{
			if(targetClient.viewIdentifier() == inMessageToSend.viewDestinationIdentifier())
			{
				this->m_UDPsocket.send_to(
					boost::asio::buffer(inMessageToSend.asCharVector()),
					targetClient.viewEndpoint(), 0, ignoredError);
				return;
			}
		}

		// check list of left adjacent server
		for(const std::string targetClientIdentifier : this->m_leftAdjacentServerConnectedClients)
		{
			if(targetClientIdentifier == inMessageToSend.viewDestinationIdentifier())
			{
				// #TODO_AH make this better, repeat of sync code since connections aren't maintained
				const int8_t leftAdjacentServerIndex =
					this->m_index - 1;

				const std::string leftAdjacentServerAddress =
					constants::serverHostName(leftAdjacentServerIndex);

				const std::string leftAdjacentServerPort =
					std::to_string(
						constants::serverListeningPorts[leftAdjacentServerIndex]);

				boost::asio::ip::udp::resolver::query serverQuery(
					boost::asio::ip::udp::v4(),
					leftAdjacentServerAddress,
					leftAdjacentServerPort);

				boost::asio::ip::udp::endpoint leftAdjacentServerEndPoint =
					*this->m_resolver.resolve(serverQuery);

				this->m_UDPsocket.send_to(
					boost::asio::buffer(inMessageToSend.asCharVector()),
					leftAdjacentServerEndPoint, 0, ignoredError);
				return;
			}
		}

		// check list of right adjacent server
		for(const std::string targetClientIdentifier : this->m_rightAdjacentServerConnectedClients)
		{
			if(targetClientIdentifier == inMessageToSend.viewDestinationIdentifier())
			{
				// #TODO_AH make this better, repeat of sync code since connections aren't maintained
				const int8_t rightAdjacentServerIndex =
					this->m_index + 1;

				const std::string rightAdjacentServerAddress =
					constants::serverHostName(rightAdjacentServerIndex);

				const std::string rightAdjacentServerPort =
					std::to_string(
						constants::serverListeningPorts[rightAdjacentServerIndex]);

				boost::asio::ip::udp::resolver::query serverQuery(
					boost::asio::ip::udp::v4(),
					rightAdjacentServerAddress,
					rightAdjacentServerPort);

				boost::asio::ip::udp::endpoint rightAdjacentServerEndPoint =
					*this->m_resolver.resolve(serverQuery);

				this->m_UDPsocket.send_to(
					boost::asio::buffer(inMessageToSend.asCharVector()),
					rightAdjacentServerEndPoint, 0, ignoredError);
				return;
			}
		}

		std::cout << "Message dropped. Client \"" << inMessageToSend.viewDestinationIdentifier() << "\" was not found." << std::endl;
		// #TODO_AH send message back to client that sent dropped message indicating them of this.
	}
};

//--------------------------------------------------------------- relayBluetooth
// Implementation notes:
//  Relay over Bluetooth
//------------------------------------------------------------------------------
void server::relayBluetooth(
	const dataMessage& inMessageToSend)
{
	// #TODO implement Bluetooth relay
};

//------------------------------------------------- sendClientsToAdjacentServers
// Implementation notes:
//  Sends the list of clients connected to this server to the adjacent servers.
//  Current implementation makes a new connection with each server on every
//  iteration of the loop, could be made much more efficient.
//------------------------------------------------------------------------------
void server::sendClientsToAdjacentServers()
{
	while(!this->m_terminate)
	{
		// left adjacent server (if it exists)
		if((this->m_index - 1) >= 0)
		{
			try
			{
				const int8_t leftAdjacentServerIndex =
					this->m_index - 1;

				const std::string leftAdjacentServerAddress =
					constants::serverHostName(leftAdjacentServerIndex);

				const std::string leftAdjacentServerPort =
					std::to_string(
						constants::serverListeningPorts[leftAdjacentServerIndex]);

				boost::asio::ip::udp::resolver::query serverQuery(
					boost::asio::ip::udp::v4(),
					leftAdjacentServerAddress,
					leftAdjacentServerPort);

				boost::asio::ip::udp::endpoint leftAdjacentServerEndPoint =
					*this->m_resolver.resolve(serverQuery);

				boost::asio::ip::udp::socket leftAdjacentServerUDPsocket(
					*(this->m_ioService));

				leftAdjacentServerUDPsocket.open(
					boost::asio::ip::udp::v4());

				dataMessage syncMessage(
					this->m_connectedClients,
					constants::serverNames[this->m_index],
					constants::serverNames[leftAdjacentServerIndex],
					constants::SYNC_LEFT);

				leftAdjacentServerUDPsocket.send_to(
					boost::asio::buffer(syncMessage.asCharVector()),
					leftAdjacentServerEndPoint);

				leftAdjacentServerUDPsocket.close();
			}
			catch(...)
			{
				// Do nothing, adjacent server is offline
			}
		}
		else
		{
			// Do nothing, no left adjacent server
		}

		// right adjacent server (if it exists)
		if((this->m_index + 1) <= constants::highestServerIndex)
		{
			try
			{
				const int8_t rightAdjacentServerIndex =
					this->m_index + 1;

				const std::string rightAdjacentServerAddress =
					constants::serverHostName(rightAdjacentServerIndex);

				const std::string rightAdjacentServerPort =
					std::to_string(
						constants::serverListeningPorts[rightAdjacentServerIndex]);

				boost::asio::ip::udp::resolver::query serverQuery(
					boost::asio::ip::udp::v4(),
					rightAdjacentServerAddress,
					rightAdjacentServerPort);

				boost::asio::ip::udp::endpoint rightAdjacentServerEndPoint =
					*this->m_resolver.resolve(serverQuery);

				boost::asio::ip::udp::socket rightAdjacentServerUDPsocket(
					*(this->m_ioService));

				rightAdjacentServerUDPsocket.open(
					boost::asio::ip::udp::v4());

				dataMessage syncMessage(
					this->m_connectedClients,
					constants::serverNames[this->m_index],
					constants::serverNames[rightAdjacentServerIndex],
					constants::SYNC_RIGHT);

				rightAdjacentServerUDPsocket.send_to(
					boost::asio::buffer(syncMessage.asCharVector()),
					rightAdjacentServerEndPoint);

				rightAdjacentServerUDPsocket.close();
			}
			catch(...)
			{
				// Do nothing, adjacent server is offline
			}

			// sleep
			boost::this_thread::sleep(
				boost::posix_time::millisec(
				constants::syncIntervalMilliseconds));
		}
		else
		{
			// Do nothing, no right adjacent server
		}
	}
};

//------------------------------------------------- sendClientsToAdjacentServers
// Implementation notes:
//  Receives the list of clients from an adjacent server and stores them.
//------------------------------------------------------------------------------
void server::receiveClientsFromAdjacentServers(
	const dataMessage& inSyncMessage)
{
	switch(inSyncMessage.viewMessageType())
	{
		case constants::MessageType::SYNC_LEFT:
		{
			this->m_rightAdjacentServerConnectedClients =
				inSyncMessage.viewServerSyncPayload();
			break;
		}
		case constants::MessageType::SYNC_RIGHT:
		{
			this->m_leftAdjacentServerConnectedClients =
				inSyncMessage.viewServerSyncPayload();
			break;
		}
		default:
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
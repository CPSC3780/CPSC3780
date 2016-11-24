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
	m_sequenceNumber(0),
	m_leftAdjacentServerIndex(inServerIndex - 1),
	m_leftAdjacentServerConnection(nullptr),
	m_rightAdjacentServerIndex(inServerIndex + 1),
	m_rightAdjacentServerConnection(nullptr)
{
	const std::string serverName(
		constants::serverIndexToServerName(inServerIndex));

	std::cout << serverName << " server started." << std::endl;
	std::cout << "Listening on port: " << inListeningPort << std::endl;

	// Left Adjacent Server query setup
	if(constants::leftAdjacentServerIndexIsValid(
		this->m_leftAdjacentServerIndex))
	{
		const std::string leftAdjacentServerAddress =
			constants::serverHostName(this->m_leftAdjacentServerIndex);

		const std::string leftAdjacentServerPort =
			std::to_string(
				constants::serverListeningPorts[this->m_leftAdjacentServerIndex]);

		boost::asio::ip::udp::resolver::query serverQuery(
			boost::asio::ip::udp::v4(),
			leftAdjacentServerAddress,
			leftAdjacentServerPort);

		boost::asio::ip::udp::endpoint leftAdjacentServerEndPoint =
			*this->m_resolver.resolve(serverQuery);

		this->m_leftAdjacentServerConnection = new remoteConnection(
			constants::serverIndexToServerName(this->m_leftAdjacentServerIndex),
			leftAdjacentServerEndPoint);
	}

	// Right Adjacent Server query setup
	if(constants::rightAdjacentServerIndexIsValid(
		this->m_rightAdjacentServerIndex))
	{
		const std::string rightAdjacentServerAddress =
			constants::serverHostName(this->m_rightAdjacentServerIndex);

		const std::string rightAdjacentServerPort =
			std::to_string(
				constants::serverListeningPorts[this->m_rightAdjacentServerIndex]);

		boost::asio::ip::udp::resolver::query serverQuery(
			boost::asio::ip::udp::v4(),
			rightAdjacentServerAddress,
			rightAdjacentServerPort);

		boost::asio::ip::udp::endpoint rightAdjacentServerEndPoint =
			*this->m_resolver.resolve(serverQuery);

		this->m_rightAdjacentServerConnection = new remoteConnection(
			constants::serverIndexToServerName(this->m_rightAdjacentServerIndex),
			rightAdjacentServerEndPoint);
	}
};

//------------------------------------------------------------------- destructor
// Implementation notes:
//  Delete adjacent server connections
//------------------------------------------------------------------------------
server::~server()
{
	delete this->m_leftAdjacentServerConnection;
	delete this->m_rightAdjacentServerConnection;

	this->m_UDPsocket.close();
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
		boost::bind(&server::sendSyncPayloads, this));

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
		try
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
				case constants::MessageType::mt_CLIENT_CONNECT:
				{
					this->addClientConnection(
						message.viewSourceIdentifier(),
						clientEndpoint);

					break;
				}
				case constants::MessageType::mt_CLIENT_DISCONNECT:
				{

					this->removeClientConnection(
						message.viewSourceIdentifier());
					break;
				}
				case constants::MessageType::mt_RELAY_CHAT:
				{
					// Do nothing
					break;
				}
				case constants::MessageType::mt_CLIENT_PRIVATE_CHAT:
				{
					// Do nothing
					break;
				}
				case constants::MessageType::mt_SERVER_SYNC:
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
		catch(std::exception& exception)
		{
			// std::cout << exception.what() << std::endl;
		}
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
	dataMessage inMessageToSend)
{
	try
	{
		boost::system::error_code ignoredError;

		// 		if(inMessageToSend.viewDestinationIdentifier() == "broadcast")
		// 		{
		// 
		// 			// If broadcast, send to all connected clients except sender
		// 			for(const remoteConnection& targetClient : this->m_connectedClients)
		// 			{
		// 				if(targetClient.viewIdentifier() != inMessageToSend.viewSourceIdentifier())
		// 				{
		// 					this->m_UDPsocket.send_to(
		// 						boost::asio::buffer(inMessageToSend.asCharVector()),
		// 						targetClient.viewEndpoint(), 0, ignoredError);
		// 				}
		// 				else
		// 				{
		// 					// Do nothing, don't broadcast back to sender
		// 				}
		// 			}
		// 
		// 			if(inMessageToSend.relayToAdjacentServers())
		// 			{
		// 				// Change the server relay status to avoid an infinite loop
		// 				inMessageToSend.setServerRelayStatus(
		// 					false);
		// 
		// 				// Also relay to left adjacent server
		// 				if(this->m_leftAdjacentServerConnection != nullptr)
		// 				{
		// 					this->m_UDPsocket.send_to(
		// 						boost::asio::buffer(inMessageToSend.asCharVector()),
		// 						this->m_leftAdjacentServerConnection->viewEndpoint(), 0, ignoredError);
		// 				}
		// 				else
		// 				{
		// 					// Do nothing, no valid left adjacent server
		// 				}
		// 
		// 				// Also relay to right adjacent server
		// 				if(this->m_rightAdjacentServerConnection != nullptr)
		// 				{
		// 					this->m_UDPsocket.send_to(
		// 						boost::asio::buffer(inMessageToSend.asCharVector()),
		// 						this->m_rightAdjacentServerConnection->viewEndpoint(), 0, ignoredError);
		// 				}
		// 				else
		// 				{
		// 					// Do nothing, no valid left adjacent server
		// 				}
		// 			}
		// 			else
		// 			{
		// 				// Do nothing, don't relay back to server that sent it
		// 			}
		// 		}
		// 		else
		// 		{
		// 			// if not broadcast, it's a private message, send only to the matched client
		// 
		// 			// check list of directly connect clients first
		// 			for(const remoteConnection& targetClient : this->m_connectedClients)
		// 			{
		// 				if(targetClient.viewIdentifier() == inMessageToSend.viewDestinationIdentifier())
		// 				{
		// 					this->m_UDPsocket.send_to(
		// 						boost::asio::buffer(inMessageToSend.asCharVector()),
		// 						targetClient.viewEndpoint(), 0, ignoredError);
		// 					return;
		// 				}
		// 				else
		// 				{
		// 					// Do nothing
		// 				}
		// 			}
		// 
		// 			// check list of left adjacent server
		// 			if(this->m_leftAdjacentServerConnection != nullptr)
		// 			{
		// 				for(const std::string targetClientIdentifier : this->m_leftAdjacentServerConnectedClients)
		// 				{
		// 					if(targetClientIdentifier == inMessageToSend.viewDestinationIdentifier())
		// 					{
		// 						this->m_UDPsocket.send_to(
		// 							boost::asio::buffer(inMessageToSend.asCharVector()),
		// 							this->m_leftAdjacentServerConnection->viewEndpoint(), 0, ignoredError);
		// 						return;
		// 					}
		// 					else
		// 					{
		// 						// Do nothing
		// 					}
		// 				}
		// 			}
		// 			else
		// 			{
		// 				// Do nothing, no left adjacent server
		// 			}
		// 
		// 			// check list of right adjacent server
		// 			if(this->m_rightAdjacentServerConnection != nullptr)
		// 			{
		// 				for(const std::string targetClientIdentifier : this->m_rightAdjacentServerConnectedClients)
		// 				{
		// 					if(targetClientIdentifier == inMessageToSend.viewDestinationIdentifier())
		// 					{
		// 						this->m_UDPsocket.send_to(
		// 							boost::asio::buffer(inMessageToSend.asCharVector()),
		// 							this->m_rightAdjacentServerConnection->viewEndpoint(), 0, ignoredError);
		// 						return;
		// 					}
		// 				}
		// 			}
		// 			else
		// 			{
		// 				// Do nothing, no right adjacent server
		// 			}
		// 
		// 			std::cout << "Message dropped. Client \"" << inMessageToSend.viewDestinationIdentifier() << "\" was not found." << std::endl;
		// 			for(const remoteConnection& targetClient : this->m_connectedClients)
		// 			{
		// 				if(targetClient.viewIdentifier() == inMessageToSend.viewSourceIdentifier())
		// 				{
		// 					// Change the message type so we can inform
		// 					// the original sender that the destination client
		// 					// wasn't found
		// 					inMessageToSend.setMessageType(
		// 						constants::MessageType::mt_CLIENT_TARGET_NOT_FOUND);
		// 
		// 					this->m_UDPsocket.send_to(
		// 						boost::asio::buffer(inMessageToSend.asCharVector()),
		// 						targetClient.viewEndpoint(), 0, ignoredError);
		// 					return;
		// 				}
		// 				else
		// 				{
		// 					// Do nothing, target is not the sender
		// 				}
		// 			}
		// 		}
	}
	catch(std::exception& exception)
	{
		// std::cout << exception.what() << std::endl;
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

//------------------------------------------------------------- sendSyncPayloads
// Implementation notes:
//  #TODO_AH fix me
//------------------------------------------------------------------------------
void server::sendSyncPayloads()
{
	while(!this->m_terminate)
	{
		std::vector<std::string> thisServersClients;

		for(const remoteConnection& currentClient : this->m_connectedClients)
		{
			thisServersClients.push_back(currentClient.viewIdentifier());
		}

		this->m_clientsServedByServerIndex[this->m_index] =
			thisServersClients;

		this->sendSyncPayloadsLeft();
		this->sendSyncPayloadsRight();

		// sleep
		boost::this_thread::sleep(
			boost::posix_time::millisec(
			constants::syncIntervalMilliseconds));
	}
};

//--------------------------------------------------------- sendSyncPayloadsLeft
// Implementation notes:
//  #TODO_AH fix me
//------------------------------------------------------------------------------
void server::sendSyncPayloadsLeft()
{
	if(this->m_leftAdjacentServerConnection != nullptr)
	{
		for(int8_t i = this->m_index; i <= constants::highestServerIndex; i++)
		{
			if(this->m_clientsServedByServerIndex[i].size() == 0)
			{
				continue;
			}
			else
			{
				try
				{
					boost::system::error_code ignoredError;

					const dataMessage syncMessageToSend(
						this->sequenceNumber(),
						constants::MessageType::mt_SERVER_SYNC,
						constants::serverIndexToServerName(this->m_index),
						constants::serverIndexToServerName(this->m_leftAdjacentServerIndex),
						this->m_clientsServedByServerIndex[i],
						i);

					this->m_UDPsocket.send_to(
						boost::asio::buffer(syncMessageToSend.asCharVector()),
						this->m_leftAdjacentServerConnection->viewEndpoint(), 0, ignoredError);
				}
				catch(std::exception& exception)
				{
					// std::cout << exception.what() << std::endl;
				}
			}
		}
	}
	else
	{
		// Do nothing
	}
};

//--------------------------------------------------------- sendSyncPaylodsRight
// Implementation notes:
//  #TODO_AH fix me
//------------------------------------------------------------------------------
void server::sendSyncPayloadsRight()
{
	if(this->m_rightAdjacentServerConnection != nullptr)
	{
		for(int8_t i = this->m_index; i >= 0; i--)
		{
			if(this->m_clientsServedByServerIndex[i].size() == 0)
			{
				continue;
			}
			else
			{
				try
				{
					boost::system::error_code ignoredError;

					const dataMessage syncMessageToSend(
						this->sequenceNumber(),
						constants::MessageType::mt_SERVER_SYNC,
						constants::serverIndexToServerName(this->m_index),
						constants::serverIndexToServerName(this->m_rightAdjacentServerIndex),
						this->m_clientsServedByServerIndex[i],
						i);

					this->m_UDPsocket.send_to(
						boost::asio::buffer(syncMessageToSend.asCharVector()),
						this->m_rightAdjacentServerConnection->viewEndpoint(), 0, ignoredError);
				}
				catch(std::exception& exception)
				{
					// std::cout << exception.what() << std::endl;
				}
			}
		}
	}
	else
	{
		// Do nothing
	}

};

//--------------------------------------------------------------- sequenceNumber
// Implementation notes:
//  #TODO_AH fix me
//------------------------------------------------------------------------------
const int64_t& server::sequenceNumber()
{
	return ++this->m_sequenceNumber;
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
		case constants::MessageType::mt_SERVER_SYNC:
		{
			this->m_clientsServedByServerIndex[inSyncMessage.viewServerSyncPayloadOriginIndex()] =
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
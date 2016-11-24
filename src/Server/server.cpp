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
	// thread for listening/acting via UDP
	this->m_threads.create_thread(
		boost::bind(&server::listenLoopUDP, this));

	// thread for listening/acting via Blueooth
	this->m_threads.create_thread(
		boost::bind(&server::listenLoopBluetooth, this));

	// thread for syncing adjacent servers
	this->m_threads.create_thread(
		boost::bind(&server::sendSyncPayloads, this));

	this->m_threads.join_all();
};

//------------------------------------------------------------------- listenLoop
// Implementation notes:
//  Listen and acts via UDP
//------------------------------------------------------------------------------
void server::listenLoopUDP()
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
			std::cout << " message from " << message.viewSourceIdentifier();

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
				case constants::MessageType::mt_CLIENT_SEND:
				{
					this->addToMessageList(
						message);
					break;
				}
				case constants::MessageType::mt_CLIENT_GET:
				{
					this->sendMessagesToClient(
						message.viewSourceIdentifier());
					break;
				}
				case constants::MessageType::mt_CLIENT_ACK:
				{
					this->removeReceivedMessageFromList(
						message);
					break;
				}
				case constants::MessageType::mt_SERVER_SEND:
				{
					// #TODO_AH implement
					break;
				}
				case constants::MessageType::mt_SERVER_ACK:
				{
					// #TODO_AH implement
					break;
				}
				case constants::MessageType::mt_SERVER_SYNC:
				{
					this->receiveClientsFromAdjacentServers(
						message);

					std::cout << " (Origin: " << constants::serverIndexToServerName(
						message.viewServerSyncPayloadOriginIndex()) << ")" << std::endl;
					continue;
					break;
				}
				case constants::MessageType::mt_PING:
				{
					// Do nothing
					break;
				}
				default:
				{
					assert(false);
				}
			}

			std::cout << std::endl;
		}
		catch(...)
		{

		}

	}
}

//---------------------------------------------------------- sendMessageToClient
// Implementation notes:
//  Sends all messages destined for the client who sent the get request
//------------------------------------------------------------------------------
void server::sendMessagesToClient(
	const std::string& inClientIdentifier)
{
	boost::system::error_code ignoredError;

	for(const remoteConnection& targetClient : this->m_connectedClients)
	{
		if(targetClient.viewIdentifier() == inClientIdentifier)
		{
			for(const dataMessage& currentMessage : this->m_messageList)
			{
				if(currentMessage.viewDestinationIdentifier() == inClientIdentifier)
				{
					try
					{
						this->m_UDPsocket.send_to(
							boost::asio::buffer(currentMessage.asCharVector()),
							targetClient.viewEndpoint(), 0, ignoredError);
					}
					catch(std::exception& exception)
					{
						// std::cout << exception.what() << std::endl;
					}
				}
				else
				{
					// Do nothing, message is not destined for the 
					// client that issued the get
				}
			}

			break;
		}
		else
		{
			// Do nothing, target client is not the one we want
		}
	}
}

//------------------------------------------------ removeReceivedMessageFromList
// Implementation notes:
//  Removes the message the client confirmed to have received from the list
//------------------------------------------------------------------------------
void server::removeReceivedMessageFromList(
	const dataMessage& inMessage)
{
	for(std::list<dataMessage>::iterator it = this->m_messageList.begin();
		it != this->m_messageList.end();
		it++)
	{
		if(it->viewSequenceNumber()== inMessage.viewSequenceNumber())
		{
			this->m_messageList.erase(it);
			break;
		}
		else
		{
			continue;
		}
	}
};

//---------------------------------------------------------- listenLoopBluetooth
// Implementation notes:
//  Listens and acts via Bluetooth
//------------------------------------------------------------------------------
void server::listenLoopBluetooth()
{
	while(!this->m_terminate)
	{
		// #TODO_MT implement
	}
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
			const size_t clientListSize =
				this->m_clientsServedByServerIndex[i].size();

			if(clientListSize == 0)
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
			const size_t clientListSize =
				this->m_clientsServedByServerIndex[i].size();

			if(clientListSize == 0)
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

//------------------------------------------------------------- addToMessageList
// Implementation notes:
//  Add a new message to the message list
//------------------------------------------------------------------------------
void server::addToMessageList(
	dataMessage message)
{
	message.setMessageType(
		constants::MessageType::mt_SERVER_SEND);

	this->m_messageList.push_back(
		message);
};
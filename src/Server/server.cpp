// Winsock
#include <winsock2.h>
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <bluetoothapis.h>
#include <stdio.h>
#include <tchar.h>
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
	this->findBluetoothRadios();
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

	// thread for listening/acting via Bluetooth
	this->m_threads.create_thread(
		boost::bind(&server::listenLoopBluetooth, this));

	// thread for syncing adjacent servers
	this->m_threads.create_thread(
		boost::bind(&server::sendSyncPayloads, this));

	// thread that attempts to forward messages where the target 
	// client was not found previously
	this->m_threads.create_thread(
		boost::bind(&server::attemptForward, this));

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
					this->processClientSendMessage(
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
					this->processServerRelayMessage(
						message);
					break;
				}
				case constants::MessageType::mt_SERVER_ACK:
				{
					// #TODO necessary?
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
};

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
};

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
		if(it->viewSequenceNumber() == inMessage.viewSequenceNumber())
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

//----------------------------------------------------- processClientSendMessage
// Implementation notes:
//  Determines if the message the client sent should be kept on this server
//  or if it should be forwarded to another server
//------------------------------------------------------------------------------
void server::processClientSendMessage(
	const dataMessage& inMessage)
{
	const std::string destinationID(
		inMessage.viewDestinationIdentifier());

	// check this server's client list first
	for(const remoteConnection& currentClient : this->m_connectedClients)
	{
		if(currentClient.viewIdentifier() == destinationID)
		{
			// destination client was found on this server, stop searching
			// and add to the message list of this server
			this->addToMessageList(
				inMessage);

			return;
		}
		else
		{
			// client does not match, continue searching
			continue;
		}
	}

	// check clients on servers to the left
	for(int8_t serverIndex = 0;
		serverIndex < this->m_index;
		serverIndex++)
	{
		for(size_t i = 0;
			i < this->m_clientsServedByServerIndex[serverIndex].size();
			i++)
		{
			if(this->m_clientsServedByServerIndex[serverIndex][i] == destinationID)
			{

				if(this->m_leftAdjacentServerConnection != nullptr)
				{
					try
					{
						boost::system::error_code ignoredError;

						this->m_UDPsocket.send_to(
							boost::asio::buffer(inMessage.asCharVector()),
							this->m_leftAdjacentServerConnection->viewEndpoint(), 0, ignoredError);

					}
					catch(std::exception& exception)
					{
						// std::cout << exception.what() << std::endl;
					}
				}
				else
				{
					// programming error, should never make it here
					assert(false);
				}

				return;
			}
			else
			{
				// client does not match, continue searching
				continue;
			}
		}
	}

	// check clients on servers to the right
	for(int8_t serverIndex = constants::highestServerIndex;
		serverIndex > this->m_index;
		serverIndex--)
	{
		for(size_t i = 0;
			i < this->m_clientsServedByServerIndex[serverIndex].size();
			i++)
		{
			if(this->m_clientsServedByServerIndex[serverIndex][i] == destinationID)
			{

				if(this->m_rightAdjacentServerConnection != nullptr)
				{
					try
					{
						boost::system::error_code ignoredError;

						this->m_UDPsocket.send_to(
							boost::asio::buffer(inMessage.asCharVector()),
							this->m_rightAdjacentServerConnection->viewEndpoint(), 0, ignoredError);

					}
					catch(std::exception& exception)
					{
						// std::cout << exception.what() << std::endl;
					}
				}
				else
				{
					// programming error, should never make it here
					assert(false);
				}

				return;
			}
			else
			{
				// client does not match, continue searching
				continue;
			}
		}
	}

	// if we make it here, as per the requirements, we hold on to the message
	this->addToMessageListOfUnassociatedClients(
		inMessage);
};

//---------------------------------------------------- processServerRelayMessage
// Implementation notes:
//  Determines if a message relayed from another server has reached
//  the destination or if it must be relayed further
//------------------------------------------------------------------------------
void server::processServerRelayMessage(
	const dataMessage& inMessage)
{
	if(inMessage.viewDestinationIdentifier()
		== constants::serverIndexToServerName(this->m_index))
	{
		this->addToMessageList(
			inMessage);
	}
	else
	{
		if(inMessage.viewServerSyncPayloadOriginIndex() < this->m_index)
		{
			// forward right
			if(this->m_rightAdjacentServerConnection != nullptr)
			{
				try
				{
					boost::system::error_code ignoredError;

					this->m_UDPsocket.send_to(
						boost::asio::buffer(inMessage.asCharVector()),
						this->m_rightAdjacentServerConnection->viewEndpoint(), 0, ignoredError);
				}
				catch(std::exception& exception)
				{
					// std::cout << exception.what() << std::endl;
				}
			}
			else
			{
				// programming error, should never make it here
				assert(false);
			}
		}
		else
		{
			// forward left
			if(this->m_leftAdjacentServerConnection != nullptr)
			{
				try
				{
					boost::system::error_code ignoredError;

					this->m_UDPsocket.send_to(
						boost::asio::buffer(inMessage.asCharVector()),
						this->m_leftAdjacentServerConnection->viewEndpoint(), 0, ignoredError);

				}
				catch(std::exception& exception)
				{
					// std::cout << exception.what() << std::endl;
				}
			}
			else
			{
				// programming error, should never make it here
				assert(false);
			}
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
		if(listen(this->m_btSocket, 5))
		{
			//Perform error handling
			closesocket(this->m_btSocket);
		}
		SOCKADDR_BTH clientAddr;
		int size = sizeof(clientAddr);
		SOCKET clientSocket = accept(this->m_btSocket, (SOCKADDR *)&clientAddr, &size);
		if(clientSocket != INVALID_SOCKET)
		{
			wprintf(L"Client connected from %04x%08x on channel %d.",
				GET_NAP(clientAddr.btAddr), GET_SAP(clientAddr.btAddr), clientAddr.port);
			char* message = "Hello, world";
			send(clientSocket, message, sizeof(message), 0);
			char buffer[1024] = {0};
			int dataSize = recv(clientSocket, buffer, sizeof(buffer), 0);
		}
	}
};

//--------------------------------------------------------------- attemptForward
// Implementation notes:
//  Listens and acts via Bluetooth
//------------------------------------------------------------------------------
void server::attemptForward()
{
	while(!this->m_terminate)
	{
		// handle potential weird behavior with deletions
		const size_t currentMessageListSize =
			this->m_messageListOfUnassociatedClients.size();

		for(size_t i = 0; i < currentMessageListSize; i++)
		{

			const dataMessage messageToCheck =
				this->m_messageListOfUnassociatedClients.front();

			this->m_messageListOfUnassociatedClients.pop_front();

			this->processClientSendMessage(
				messageToCheck);
		}

		// sleep
		boost::this_thread::sleep(
			boost::posix_time::millisec(
			constants::forwardIntervalMilliseconds));
	}
};

//------------------------------------------------------------- sendSyncPayloads
// Implementation notes:
//  Sends the sync payloads for this server and all known servers to adjacent
//  servers if they exists. This is done as one UDP message per server.
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
//  Sends all known sync payloads to the left adjacent server
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
//  Sends all known sync payloads to the right adjacent server
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
//  Increments the sequence number every time it is used, self explanatory.
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
	this->m_clientsServedByServerIndex[inSyncMessage.viewServerSyncPayloadOriginIndex()] =
		inSyncMessage.viewServerSyncPayload();
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
	for(std::vector<remoteConnection>::iterator it = this->m_connectedClients.begin();
		it != this->m_connectedClients.end();
		it++)
	{
		if(it->viewIdentifier() == inClientUsername)
		{
			this->m_connectedClients.erase(it);
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

//---------------------------------------- addToMessageListOfUnassociatedClients
// Implementation notes:
//  Add a new message to the message list of unassociated clients
//------------------------------------------------------------------------------
void server::addToMessageListOfUnassociatedClients(
	dataMessage message)
{
	message.setMessageType(
		constants::MessageType::mt_CLIENT_SEND);

	this->m_messageListOfUnassociatedClients.push_back(
		message);
};

//---------------------------------------------------------- findBluetoothRadios
// Implementation notes
//  Searches for Bluetooth radios
//------------------------------------------------------------------------------
void server::findBluetoothRadios()
{
	BLUETOOTH_FIND_RADIO_PARAMS findParams;
	findParams.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);
	HANDLE bluetoothRadio;
	HBLUETOOTH_RADIO_FIND radioFindHandle = BluetoothFindFirstRadio(&findParams, &bluetoothRadio);
	if(radioFindHandle != nullptr)
	{
		wprintf(L"Found Bluetooth radio\n");
		wprintf(L"Opening server socket for bluetooth connections...\n");
		SOCKET btSock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if(btSock == INVALID_SOCKET)
			wprintf(L"Error creating socket: %d\n", WSAGetLastError());
		else
		{
			this->m_btSocket = btSock;
			WSAPROTOCOL_INFO protoInfo;
			int infoStructSize = sizeof(protoInfo);
			if(getsockopt(btSock, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&protoInfo, &infoStructSize))
			{
				wprintf(L"Error getting socket options: %d\n", WSAGetLastError());
			}
			else
			{
				// Bind the socket
				SOCKADDR_BTH address;
				address.addressFamily = AF_BTH;
				address.btAddr = 0;
				address.serviceClassId = GUID_NULL;
				address.port = BT_PORT_ANY;
				if(bind(btSock, (sockaddr*)&address, sizeof(address)))
				{
					wprintf(L"Error binding socket: %d\n", WSAGetLastError());
				}
				else
				{
					int addressLen = sizeof(address);
					sockaddr* pAddress = (sockaddr*)&address;
					getsockname(btSock, pAddress, &addressLen);
					wprintf(L"Bind Successful: device=%04x%08x channel = %d\n",
						GET_NAP(address.btAddr), GET_SAP(address.btAddr), address.port);
				}
			}
		}
	}
}

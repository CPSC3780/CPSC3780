#pragma once

// Boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// STL
#include <vector>
#include <list>
#include <string>
#include <utility>
#include <cstdint>

// Project
#include "../Common/remoteConnection.h"
#include "../Common/dataMessage.h"

class server
{
public:

	//-------------------------------------------------------------- constructor
	// Brief Description
	//  Constructor for the server
	//
	// Method:    server
	// FullName:  server::server
	// Access:    public 
	// Returns:   
	// Parameter: const uint16_t& inListeningPort
	// Parameter: const int8_t& inServerIndex
	// Parameter: boost::asio::io_service& ioService
	//--------------------------------------------------------------------------
	server(
		const uint16_t& inListeningPort,
		const int8_t& inServerIndex,
		boost::asio::io_service& ioService);

	//--------------------------------------------------------------- destructor
	// Brief Description
	//  Destructor for the server
	//
	// Method:    ~server
	// FullName:  server::~server
	// Access:    public 
	// Returns:   
	//--------------------------------------------------------------------------
	~server();

	//---------------------------------------------------------------------- run
	// Brief Description
	//  Creates a thread for each major function of the server. These functions
	//  loop indefinitely.
	//
	// Method:    run
	// FullName:  server::run
	// Access:    public 
	// Returns:   void
	//--------------------------------------------------------------------------
	void run();

private:

	//------------------------------------------------------------ listenLoopUDP
	// Brief Description
	//  The server's listening loop for UDP. It receives messages from clients
	//  over UDP and acts accordingly.
	//
	// Method:    listenLoopUDP
	// FullName:  server::listenLoopUDP
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void listenLoopUDP();

	//----------------------------------------------------- sendMessagesToClient
	// Brief Description
	//  Called when a client sends a get to the server. It makes the server send
	//  that client all messages that are destined for it.
	//
	// Method:    sendMessagesToClient
	// FullName:  server::sendMessagesToClient
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void sendMessagesToClient(
		const std::string& inClientIdentifier);

	// #TODO_AH fix me
	void removeReceivedMessageFromList(
		const dataMessage& inMessage);

	// #TODO_AH fix me
	void processClientSendMessage(
		const dataMessage& inMessage);

	// #TODO_AH fix me
	void processServerRelayMessage(
		const dataMessage& inMessage);

	//------------------------------------------------------ listenLoopBluetooth
	// Brief Description
	//  The server's listening loop for Bluetooth. It receives messages from 
	//  clients over Bluetooth and acts accordingly.
	//
	// Method:    listenLoopBluetooth
	// FullName:  server::listenLoopBluetooth
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void listenLoopBluetooth();

	//--------------------------------------------- sendSyncPayloads
	// Brief Description
	//  #TODO_AH fix me
	//
	// Method:    sendSyncPayloads
	// FullName:  server::sendSyncPayloads
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void sendSyncPayloads();

	void sendSyncPayloadsLeft();

	void sendSyncPayloadsRight();

	const int64_t& sequenceNumber();

	//---------------------------------------- receiveClientsFromAdjacentServers
	// Brief Description
	//  Receives the sync sent from an adjacent server and populates the 
	//  appropriate list of usernames via the data message.
	//
	// Method:    receiveClientsFromAdjacentServers
	// FullName:  server::receiveClientsFromAdjacentServers
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage& inSyncMessage
	//--------------------------------------------------------------------------
	void receiveClientsFromAdjacentServers(
		const dataMessage& inSyncMessage);

	//------------------------------------------------------ addClientConnection
	// Brief Description
	//  Used by the server to add a new client connection when it receives a
	//  connection message from a client. All broadcast messages received 
	//  afterwards will be relayed to this client. This client will also be a 
	//  valid target for private messages.
	//
	// Method:    addClientConnection
	// FullName:  server::addClientConnection
	// Access:    private 
	// Returns:   void
	// Parameter: const std::string& inClientUsername
	// Parameter: const boost::asio::ip::udp::endpoint& inClientEndpoint
	//--------------------------------------------------------------------------
	void addClientConnection(
		const std::string& inClientUsername,
		const boost::asio::ip::udp::endpoint& inClientEndpoint);

	//--------------------------------------------------- removeClientConnection
	// Brief Description
	//  Removes the client connection. The client will no longer be associated
	//  with this server.
	//
	// Method:    removeClientConnection
	// FullName:  server::removeClientConnection
	// Access:    private 
	// Returns:   void
	// Parameter: const std::string& inClientUsername
	//--------------------------------------------------------------------------
	void removeClientConnection(
		const std::string& inClientUsername);

	//--------------------------------------------------------- addToMessageList
	// Brief Description
	//  Helper function. Adds a data message to the list of messages that
	//  haven't been delivered to a client.
	//
	// Method:    addToMessageList
	// FullName:  server::addToMessageList
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage& message
	//--------------------------------------------------------------------------
	void addToMessageList(
		dataMessage message);

	// Member Variables
	boost::asio::ip::udp::socket m_UDPsocket;
	boost::asio::ip::udp::resolver m_resolver;
	boost::asio::io_service* m_ioService;
	int8_t m_index;
	boost::thread_group m_threads;

	bool m_terminate;
	int64_t m_sequenceNumber;

	std::list<dataMessage> m_messageList;

	std::vector<remoteConnection> m_connectedClients;

	int8_t m_leftAdjacentServerIndex;
	remoteConnection* m_leftAdjacentServerConnection;

	int8_t m_rightAdjacentServerIndex;
	remoteConnection* m_rightAdjacentServerConnection;

	std::vector<std::string> m_clientsServedByServerIndex[constants::numberOfServers];
};
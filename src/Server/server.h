#pragma once

// Boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// STL
#include <vector>
#include <queue>
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

	//--------------------------------------------------------------- listenLoop
	// Brief Description
	//  The server's listening loop, used for receiving connections and messages
	//  from clients and adjacent servers.
	//
	// Method:    listenLoop
	// FullName:  server::listenLoop
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void listenLoop();

	//---------------------------------------------------------------- relayLoop
	// Brief Description
	//  The server's relay loop, used to relay the messages to the clients and
	//  adjacent servers over the supported protocols.
	//
	// Method:    relayLoop
	// FullName:  server::relayLoop
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void relayLoop();


	//----------------------------------------------------------------- relayUDP
	// Brief Description
	//  Relays messages to clients and adjacent servers over UDP.
	//
	// Method:    relayUDP
	// FullName:  server::relayUDP
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage& inMessageToSend
	//--------------------------------------------------------------------------
	void relayUDP(
		dataMessage inMessageToSend);

	//----------------------------------------------------------- relayBluetooth
	// Brief Description
	//  Relays messages to clients and adjacent servers over Bluetooth.
	//
	// Method:    relayBluetooth
	// FullName:  server::relayBluetooth
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage & inMessageToSend
	//--------------------------------------------------------------------------
	void relayBluetooth(
		const dataMessage& inMessageToSend);

	//--------------------------------------------- sendClientsToAdjacentServers
	// Brief Description
	//  Sends a list of all clients connected to this server to the adjacent
	//  servers if a connection can be established.
	//
	// Method:    sendClientsToAdjacentServers
	// FullName:  server::sendClientsToAdjacentServers
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void sendClientsToAdjacentServers();

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
	// Parameter: const std::string & inClientUsername
	//--------------------------------------------------------------------------
	void removeClientConnection(
		const std::string& inClientUsername);

	//-------------------------------------------------------- addToMessageQueue
	// Brief Description
	//  Helper function. Adds a data message to the queue that will then
	//  subsequently be relayed as specified.
	//
	// Method:    addToMessageQueue
	// FullName:  server::addToMessageQueue
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage & message
	//--------------------------------------------------------------------------
	void addToMessageQueue(
		const dataMessage& message);

	// Member Variables
	boost::asio::ip::udp::socket m_UDPsocket;
	boost::asio::ip::udp::resolver m_resolver;
	boost::asio::io_service* m_ioService;
	int8_t m_index;
	boost::thread_group m_threads;

	bool m_terminate;

	std::queue<dataMessage> m_messageQueue;

	std::vector<remoteConnection> m_connectedClients;

	int8_t m_leftAdjacentServerIndex;
	remoteConnection* m_leftAdjacentServerConnection;
	std::vector<std::string> m_leftAdjacentServerConnectedClients;

	int8_t m_rightAdjacentServerIndex;
	remoteConnection* m_rightAdjacentServerConnection;
	std::vector<std::string> m_rightAdjacentServerConnectedClients;
};
#pragma once
// Winsock
#include <winsock2.h>
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <bluetoothapis.h>
#include <stdio.h>
#include <tchar.h>
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

	//-------------------------------------------- removeReceivedMessageFromList
	// Brief Description
	//  Removes the corresponding message specified via the input parameter
	//  from the message list. This is usually done in response to getting
	//  an ACK from the client. Once a message has been confirmed received
	//  by the intended client, it is no longer necessary to store it on the
	//  server.
	//
	// Method:    removeReceivedMessageFromList
	// FullName:  server::removeReceivedMessageFromList
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage& inMessage
	//--------------------------------------------------------------------------
	void removeReceivedMessageFromList(
		const dataMessage& inMessage);


	//------------------------------------------------- processClientSendMessage
	// Brief Description
	//  Determines if the message a client sent should be stored on this server
	//  or if it should be routed to a connected server instead. A message
	//  is only kept on this server if it the client it is destined for is
	//  connected to this server. If the client it is destined for is on another
	//  server, it will forward it in that direction, and the servers in line
	//  will continually forward it until it arrives where it should.
	//
	// Method:    processClientSendMessage
	// FullName:  server::processClientSendMessage
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage& inMessage
	//--------------------------------------------------------------------------
	void processClientSendMessage(
		const dataMessage& inMessage);

	
	//------------------------------------------------ processServerRelayMessage
	// Brief Description
	//  Determines if a message that was forwarded from another server has
	//  reached its destination, or if it should continue forwarding.
	//  A message has reached its destination if the client it is destined
	//  for is connected to this server.
	//
	// Method:    processServerRelayMessage
	// FullName:  server::processServerRelayMessage
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage& inMessage
	//--------------------------------------------------------------------------
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

	//----------------------------------------------------------- attemptForward
	// Brief Description
	//  Periodically cycles through all the messages on this server and
	//  determines if they should be forwarded to another server, or kept
	//  on this server instead.
	//
	// Method:    attemptForward
	// FullName:  server::attemptForward
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void attemptForward();

	//--------------------------------------------------------- sendSyncPayloads
	// Brief Description
	//  The main sync loop between servers. This routinely forwards all known
	//  client lists for each server to the adjacent servers.
	//
	// Method:    sendSyncPayloads
	// FullName:  server::sendSyncPayloads
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void sendSyncPayloads();

	//----------------------------------------------------- sendSyncPayloadsLeft
	// Brief Description
	//  Helper function that forwards the client lists to the left adjacent
	//  server.
	//
	// Method:    sendSyncPayloadsLeft
	// FullName:  server::sendSyncPayloadsLeft
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void sendSyncPayloadsLeft();

	//---------------------------------------------------- sendSyncPayloadsRight
	// Brief Description
	//  Helper function that forwards the client lists to the right adjacent
	//  server.
	//
	// Method:    sendSyncPayloadsRight
	// FullName:  server::sendSyncPayloadsRight
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
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

	//------------------------------------ addToMessageListOfUnassociatedClients
	// Brief Description
	//  Adds a message to the list that contains all messages for which the
	//  server was unable to determine what server serves that client. This
	//  list is periodically checked and the server will attempt to
	//  find the server associated with a particular client.
	//
	// Method:    addToMessageListOfUnassociatedClients
	// FullName:  server::addToMessageListOfUnassociatedClients
	// Access:    private 
	// Returns:   void
	// Parameter: dataMessage message
	//--------------------------------------------------------------------------
	void addToMessageListOfUnassociatedClients(
		dataMessage message);

	//------------------------------------------------------ findBluetoothRadios
	// Brief Description
	//  Searches for Bluetooth radios
	//
	// Method:    findBluetoothRadios
	// FullName:  server::findBluetoothRadios
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void findBluetoothRadios();

	// Member Variables
	boost::asio::ip::udp::socket m_UDPsocket;
	boost::asio::ip::udp::resolver m_resolver;
	boost::asio::io_service* m_ioService;
	int8_t m_index;
	boost::thread_group m_threads;
	SOCKET m_btSocket;

	bool m_terminate;
	int64_t m_sequenceNumber;

	std::list<dataMessage> m_messageList;
	std::list<dataMessage> m_messageListOfUnassociatedClients;

	std::vector<remoteConnection> m_connectedClients;

	int8_t m_leftAdjacentServerIndex;
	remoteConnection* m_leftAdjacentServerConnection;

	int8_t m_rightAdjacentServerIndex;
	remoteConnection* m_rightAdjacentServerConnection;

	std::vector<std::string> m_clientsServedByServerIndex[constants::numberOfServers];
};
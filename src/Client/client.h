#pragma once

// STL
#include <vector>

// Boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// Project
#include "../Common/dataMessage.h"

class client
{
	enum protocol
	{
		UNDEFINED = 0,
		UDP = 1,
		BLUETOOTH = 2
	};

public:
	
	//-------------------------------------------------------------- constructor
	// Brief Description
	//  Constructor for the client
	//
	// Method:    client
	// FullName:  client::client
	// Access:    public 
	// Returns:   
	// Parameter: const std::string & username
	// Parameter: boost::asio::io_service & ioService
	//--------------------------------------------------------------------------
	client(
		const std::string& username,
		boost::asio::io_service &ioService);

	//---------------------------------------------------------------------- run
	// Brief Description
	//  Creates a thread for each major function of the client. These functions
	//  loop indefinitely.
	//
	// Method:    run
	// FullName:  client::run
	// Access:    public 
	// Returns:   void
	//--------------------------------------------------------------------------
	void run();

private:
	
	//---------------------------------------------------------------- inputLoop
	// Brief Description
	//  Input loop for getting input from the user via command line. The input
	//  is parsed and coverted to a data message, which is sent to the server
	//  the client has established a connection with.
	//
	// Method:    inputLoop
	// FullName:  client::inputLoop
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void inputLoop();

	//-------------------------------------------------------------- sendOverUDP
	// Brief Description
	//  Sends messages to the server over UDP.
	//
	// Method:    sendOverUDP
	// FullName:  client::sendOverUDP
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage& message
	//--------------------------------------------------------------------------
	void sendOverUDP(
		const dataMessage& message);
	
	//-------------------------------------------------------- sendOverBluetooth
	// Brief Description
	//  Relays messages to the server over Bluetooth.
	//
	// Method:    sendOverBluetooth
	// FullName:  client::sendOverBluetooth
	// Access:    private 
	// Returns:   void
	// Parameter: const dataMessage & message
	//--------------------------------------------------------------------------
	void sendOverBluetooth(
		const dataMessage& message);

	//-------------------------------------------------------------- receiveLoop
	// Brief Description
	//  The client's receive loop, it receives the messages from the server 
	//  over the supported protocols.
	//
	// Method:    receiveLoop
	// FullName:  client::receiveLoop
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void receiveLoop();

	//----------------------------------------------------------- receiveOverUDP
	// Brief Description
	//  Receives messages from the server over UDP.
	//
	// Method:    receiveOverUDP
	// FullName:  client::receiveOverUDP
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void receiveOverUDP();

	//----------------------------------------------------- receiveOverBluetooth
	// Brief Description
	//  Receives messages from the server over Bluetooth.
	//
	// Method:    receiveOverBluetooth
	// FullName:  client::receiveOverBluetooth
	// Access:    private 
	// Returns:   void
	//--------------------------------------------------------------------------
	void receiveOverBluetooth();

	// Member Variables
	boost::asio::ip::udp::socket m_UDPsocket;
	boost::asio::ip::udp::resolver m_resolver;
	boost::asio::ip::udp::endpoint m_serverEndPoint;
	boost::thread_group m_threads;
	client::protocol m_activeProtocol;
	bool m_terminate;
	std::string m_username;
};
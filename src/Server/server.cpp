// STL
#include <cstdint>
#include <iostream>
#include <vector>

// Boost
#include <boost/array.hpp>
#include <boost/bind.hpp>

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
		const uint8_t arbitraryLength = 64;

		// Wait for connection
		boost::array<char, arbitraryLength> recv_buf;
		recv_buf.fill(' ');
		boost::system::error_code error;

		// remote_endpoint object is populated by receive_from()
		m_UDPsocket.receive_from(boost::asio::buffer(recv_buf),
			this->m_remoteEndPoint, 0, error);

		if(error && error != boost::asio::error::message_size)
		{
			throw boost::system::system_error(error);
		}

		this->addConnections(this->m_remoteEndPoint);

		std::cout << "Received message from: ";
		this->addToMessageQueue(recv_buf.data());

		// this->addToMessageQueue()
	}
}

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
	if(this->messageQueue.size() > 0)
	{
		std::vector<boost::asio::ip::udp::endpoint>::iterator it;
		std::vector<std::string>::iterator message_it;
		for(message_it = this->messageQueue.begin(); message_it < this->messageQueue.end(); message_it++)
		{
			for(it = this->connections.begin(); it < this->connections.end(); it++)
			{
				boost::system::error_code ignored_error;
				std::string message = *message_it;
				std::cout << *it << std::endl;

				this->m_UDPsocket.send_to(boost::asio::buffer(message),
					*it, 0, ignored_error);
				this->connections.erase(std::unique(this->connections.begin(), this->connections.end()), this->connections.end());
			}
			this->messageQueue.erase(this->messageQueue.begin()++);
		}
		
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

//--------------------------------------------------------------- addConnections
// Implementation notes:
//  Add new connections to connections list
//------------------------------------------------------------------------------
void server::addConnections(boost::asio::ip::udp::endpoint client)
{
	this->connections.push_back(client);
};

//--------------------------------------------------------------- addToMessageQueue
// Implementation notes:
//  Add new messages to message queue list
//------------------------------------------------------------------------------
void server::addToMessageQueue(std::string message)
{
	this->messageQueue.push_back(message);
};


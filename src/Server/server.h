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
#include "./remoteConnection.h"
#include "../Common/dataMessage.h"

class server
{
public:
	// #TODO_AH header comment blocks
	server(
		const uint16_t& inListeningPort,
		boost::asio::io_service& ioService);

	void run();

private:

	void listenLoop();

	void relayLoop();

	void relayUDP();

	void relayBluetooth();

	void addConnection(
		const std::string& inClientUsername,
		const boost::asio::ip::udp::endpoint& inClientEndpoint);

	void removeConnection(
		const std::string& inClientUsername);

	void addToMessageQueue(
		const dataMessage& message);

	// Member Variables
	boost::asio::ip::udp::socket m_UDPsocket;
	boost::thread_group m_threads;
	std::vector<remoteConnection> m_connectedClients;
	std::queue<dataMessage> m_messageQueue;
	bool m_terminate;
};
// Boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// STL
#include <vector>
#include <queue>
#include <string>
#include <utility>

// Project
#include "../Common/dataMessage.h"

class server
{
public:
	// #TODO_AH header comment blocks
	server(
		boost::asio::io_service &ioService);

	void run();

private:

	void listenLoop();

	void relayLoop();

	void relayUDP();

	void relayBluetooth();

	void addConnections(
		const boost::asio::ip::udp::endpoint& client);

	void addToMessageQueue(
		const std::string& message);

	// Member Variables
	boost::asio::ip::udp::socket m_UDPsocket;
	boost::thread_group m_threads;
	boost::asio::ip::udp::endpoint m_remoteEndPoint; // #TODO should probably be a list
	std::vector<std::pair<std::string, boost::asio::ip::udp::endpoint> > m_connectedClients;
	std::queue<dataMessage> m_messageQueue;
	bool m_terminate;
};
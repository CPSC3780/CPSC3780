// Boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>

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

	void addConnections(boost::asio::ip::udp::endpoint);

	void addToMessageQueue(std::string const);

	// Member Variables
	boost::asio::ip::udp::socket m_UDPsocket;
	boost::thread_group m_threads;
	boost::asio::ip::udp::endpoint m_remoteEndPoint; // #TODO should probably be a list
	bool m_terminate;
	std::vector<boost::asio::ip::udp::endpoint> connections;
	std::vector<std::string> messageQueue;
};
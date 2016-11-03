// Boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>

class client
{
	enum class protocol
	{
		Undefined,
		UDP,
		Bluetooth
	};

public:
	// #TODO_AH header comment blocks
	client(
		boost::asio::io_service &ioService);

	void run();

private:
	void inputLoop();

	void sendOverUDP(
		const std::string& message);

	void sendOverBluetooth(
		const std::string& message);

	void receiveLoop();

	void receiveOverUDP();

	void receiveOverBluetooth();

	void serializeUDPMessage(std::vector<std::string>);

	// Member Variables
	boost::asio::ip::udp::socket m_UDPsocket;
	boost::asio::ip::udp::resolver m_resolver;
	boost::asio::ip::udp::endpoint m_serverEndPoint;
	boost::thread_group m_threads;
	client::protocol m_activeProtocol;
	bool m_terminate;
};
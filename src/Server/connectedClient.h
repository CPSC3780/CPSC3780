#pragma once

// STL
#include <string>

// Boost
#include <boost/asio.hpp>
#include <boost/chrono.hpp>

class connectedClient
{
public:
	// #TODO_AH header comment blocks
	connectedClient(
		const std::string inClientUsername,
		const boost::asio::ip::udp::endpoint inClientEndpoint);

	const std::string& viewUsername() const;
	const boost::asio::ip::udp::endpoint& viewEndpoint() const;
	const boost::chrono::system_clock::time_point& viewTimeOfLastActivity() const;
	void refreshTimeOfLastActivity();

private:
	std::string m_username;
	boost::asio::ip::udp::endpoint m_endpoint;
	boost::chrono::system_clock::time_point m_timeOfLastActivity;	
};
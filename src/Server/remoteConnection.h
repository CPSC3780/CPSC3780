#pragma once

// STL
#include <string>

// Boost
#include <boost/asio.hpp>
#include <boost/chrono.hpp>

class remoteConnection
{
public:
	// #TODO_AH header comment blocks
	remoteConnection(
		const std::string inIdentifier,
		const boost::asio::ip::udp::endpoint inEndpoint);

	const std::string& viewIdentifier() const;
	const boost::asio::ip::udp::endpoint& viewEndpoint() const;
	const boost::chrono::system_clock::time_point& viewTimeOfLastActivity() const;
	void refreshTimeOfLastActivity();

private:
	std::string m_identifier;
	boost::asio::ip::udp::endpoint m_endpoint;
	boost::chrono::system_clock::time_point m_timeOfLastActivity;	
};
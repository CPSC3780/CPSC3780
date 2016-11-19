#pragma once

// STL
#include <string>
#include <cstdint>

// Boost
#include <boost/asio.hpp>
#include <boost/chrono.hpp>

class remoteConnection
{
public:

	//-------------------------------------------------------------- constructor
	// Brief Description
	//  Constructor for the remote connection
	//
	// Method:    remoteConnection
	// FullName:  remoteConnection::remoteConnection
	// Access:    public 
	// Returns:   
	// Parameter: const std::string& inIdentifier
	// Parameter: const boost::asio::ip::udp::endpoint& inEndpoint
	//--------------------------------------------------------------------------
	remoteConnection(
		const std::string& inIdentifier,
		const boost::asio::ip::udp::endpoint& inEndpoint);

	//----------------------------------------------------------- viewIdentifier
	// Brief Description
	//  Returns a const reference to the identifier string. This will either
	//  represent a client's username, or a server.
	//
	// Method:    viewIdentifier
	// FullName:  remoteConnection::viewIdentifier
	// Access:    public 
	// Returns:   const std::string&
	//--------------------------------------------------------------------------
	const std::string& viewIdentifier() const;

	//------------------------------------------------------------- viewEndpoint
	// Brief Description
	//  Returns a const reference to the endpoint of this connection, which can
	//  then be used to send a message.
	//
	// Method:    viewEndpoint
	// FullName:  remoteConnection::viewEndpoint
	// Access:    public 
	// Returns:   const boost::asio::ip::udp::endpoint&
	//--------------------------------------------------------------------------
	const boost::asio::ip::udp::endpoint& viewEndpoint() const;

	//--------------------------------------------------- viewTimeOfLastActivity
	// Brief Description
	//  Returns a const reference to the time of the last activity that this
	//  connection had with the server. Used for timeout purposes.
	//
	// Method:    viewTimeOfLastActivity
	// FullName:  remoteConnection::viewTimeOfLastActivity
	// Access:    public 
	// Returns:   const boost::chrono::system_clock::time_point&
	//--------------------------------------------------------------------------
	const boost::chrono::system_clock::time_point& viewTimeOfLastActivity() const;

	//------------------------------------------------ refreshTimeOfLastActivity
	// Brief Description
	//  Refreshes the time of the last activity to the current time. This is
	//  done periodically to prevent timeouts.
	//
	// Method:    refreshTimeOfLastActivity
	// FullName:  remoteConnection::refreshTimeOfLastActivity
	// Access:    public 
	// Returns:   void
	//--------------------------------------------------------------------------
	void refreshTimeOfLastActivity();

private:
	std::string m_identifier;
	boost::asio::ip::udp::endpoint m_endpoint;
	boost::chrono::system_clock::time_point m_timeOfLastActivity;	
};
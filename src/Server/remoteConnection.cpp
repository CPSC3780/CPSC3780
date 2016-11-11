// Project
#include "remoteConnection.h"

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Sets all relevant member variables
//------------------------------------------------------------------------------
remoteConnection::remoteConnection(
	const std::string inIdentifier,
	const boost::asio::ip::udp::endpoint inEndpoint)
{
	this->m_identifier = inIdentifier;
	this->m_endpoint = inEndpoint;
	this->m_timeOfLastActivity = boost::chrono::system_clock::now();
};

//--------------------------------------------------------------- viewIdentifier
// Implementation notes:
//  Returns a const reference to the identifier
//------------------------------------------------------------------------------
const std::string& remoteConnection::viewIdentifier() const
{
	return this->m_identifier;
};

//----------------------------------------------------------------- viewEndpoint
// Implementation notes:
//  Returns a const reference to the endpoint
//------------------------------------------------------------------------------
const boost::asio::ip::udp::endpoint& remoteConnection::viewEndpoint() const
{
	return this->m_endpoint;
};

//------------------------------------------------------- viewTimeOfLastActivity
// Implementation notes:
//  Returns a const reference to the timeOfLastActivity
//------------------------------------------------------------------------------
const boost::chrono::system_clock::time_point& remoteConnection::viewTimeOfLastActivity() const
{
	return this->m_timeOfLastActivity;
};

//---------------------------------------------------- refreshTimeOfLastActivity
// Implementation notes:
//  Sets the timeOfLastActivity to now
//------------------------------------------------------------------------------
void remoteConnection::refreshTimeOfLastActivity()
{
	this->m_timeOfLastActivity = boost::chrono::system_clock::now();
};
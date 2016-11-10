// Project
#include "connectedClient.h"

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Sets all relevant member variables
//------------------------------------------------------------------------------
connectedClient::connectedClient(
	const std::string inClientUsername, 
	const boost::asio::ip::udp::endpoint inClientEndpoint)
{
	this->m_username = inClientUsername;
	this->m_endpoint = inClientEndpoint;
	this->m_timeOfLastActivity = boost::chrono::system_clock::now();
};

//----------------------------------------------------------------- viewUsername
// Implementation notes:
//  Returns a const reference to the username
//------------------------------------------------------------------------------
const std::string& connectedClient::viewUsername() const
{
	return this->m_username;
};

//----------------------------------------------------------------- viewEndpoint
// Implementation notes:
//  Returns a const reference to the endpoint
//------------------------------------------------------------------------------
const boost::asio::ip::udp::endpoint& connectedClient::viewEndpoint() const
{
	return this->m_endpoint;
};

//------------------------------------------------------- viewTimeOfLastActivity
// Implementation notes:
//  Returns a const reference to the timeOfLastActivity
//------------------------------------------------------------------------------
const boost::chrono::system_clock::time_point& connectedClient::viewTimeOfLastActivity() const
{
	return this->m_timeOfLastActivity;
};

//---------------------------------------------------- refreshTimeOfLastActivity
// Implementation notes:
//  Sets the timeOfLastActivity to now
//------------------------------------------------------------------------------
void connectedClient::refreshTimeOfLastActivity()
{
	this->m_timeOfLastActivity = boost::chrono::system_clock::now();
};
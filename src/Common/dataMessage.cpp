// STL
#include <string>

// Project
#include "dataMessage.h"

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Used to create a data message object to send
//------------------------------------------------------------------------------
dataMessage::dataMessage(
	const std::string& inPayload, 
	const std::string& inSourceID,
	const std::string& inDestinationID = "broadcast")
{
	this->m_payload = inPayload;
	this->m_sourceID = inSourceID;
	this->m_destinationID = inDestinationID;
};

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Used to create a data message object from a received vector
//------------------------------------------------------------------------------
dataMessage::dataMessage(
	const std::vector<std::string>& receivedVector)
{
	this->m_payload = receivedVector[0];
	this->m_sourceID = receivedVector[1];
	this->m_destinationID = receivedVector[2];
};

//------------------------------------------------------------------ viewPayload
// Implementation notes:
//  Returns a const reference to the payload string
//------------------------------------------------------------------------------
const std::string& dataMessage::viewPayload() const
{
	return this->m_payload;
};

//----------------------------------------------------------------- viewSourceID
// Implementation notes:
//  Returns a const reference to the sourceID string
//------------------------------------------------------------------------------
const std::string& dataMessage::viewSourceID() const
{
	return this->m_sourceID;
};

//------------------------------------------------------------ viewDestinationID
// Implementation notes:
//  Returns a const reference to the destinationID string
//------------------------------------------------------------------------------
const std::string& dataMessage::viewDestinationID() const
{
	return this->m_destinationID;
};

//--------------------------------------------------------------------- asVector
// Implementation notes:
//  Returns a a vector containing the 3 strings in order of m_payload, 
//  m_sourceID, and m_destinationID
//------------------------------------------------------------------------------
std::vector<std::string> dataMessage::asVector() const
{
	return std::vector<std::string>({
		this->m_payload,
		this->m_sourceID,
		this->m_destinationID});
};

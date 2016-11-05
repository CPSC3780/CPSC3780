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
	this->m_payload = inPayload + "/?";
	this->m_sourceID = inSourceID + "/?";
	this->m_destinationID = inDestinationID + "/?";
};

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Used to create a data message object from a received vector
//------------------------------------------------------------------------------
dataMessage::dataMessage(
	boost::asio::mutable_buffer& receivedBuffer)
{

//	std::vector<char> sourceidvector =
//		boost::asio::buffer_cast<std::vector<char>>(receivedBuffer);

//	m_payload = std::string(
//		sourceidvector.begin(),
//		sourceidvector.end());
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

//---------------------------------------------------------------- asConstBuffer
// Implementation notes:
//  Returns all 3 strings in 3 vector<char> arrays as a const buffer
//------------------------------------------------------------------------------
boost::array<boost::asio::const_buffer, 3> dataMessage::asConstBuffer() const
{
	std::vector<char> payloadToSend(
		this->m_payload.begin(),
		this->m_payload.end());

	std::vector<char> sourceToSend(
		this->m_sourceID.begin(),
		this->m_sourceID.end());

	std::vector<char> destinationToSend(
		this->m_destinationID.begin(),
		this->m_destinationID.end());

	return boost::array<boost::asio::const_buffer, 3> {
		boost::asio::buffer(payloadToSend),
		boost::asio::buffer(sourceToSend),
		boost::asio::buffer(destinationToSend)};
};

//-------------------------------------------------------------- asMutableBuffer
// Implementation notes:
//  Returns all 3 strings in 3 vector<char> arrays as a mutable buffer
//------------------------------------------------------------------------------
boost::array<boost::asio::mutable_buffer, 3> dataMessage::asMutableBuffer() const
{
	std::vector<char> payloadToSend(
		this->m_payload.begin(),
		this->m_payload.end());

	std::vector<char> sourceToSend(
		this->m_sourceID.begin(),
		this->m_sourceID.end());

	std::vector<char> destinationToSend(
		this->m_destinationID.begin(),
		this->m_destinationID.end());

	return boost::array<boost::asio::mutable_buffer, 3> {
		boost::asio::buffer(payloadToSend)
	};
};

//-------------------------------------------------------------- asString
// Implementation notes:
//  Returns data message as a string, delimited by an int followed by a comma
//------------------------------------------------------------------------------
std::string dataMessage::asString() const
{
	std::string stringifiedMessage =
		m_payload + "/?" + m_sourceID + "/?" + m_destinationID + "/?";

	return stringifiedMessage;
};

//-------------------------------------------------------------- assignValues
// Implementation notes:
//  Takes a stringified message and assigns each part of the message to respective 
//  member variables
//------------------------------------------------------------------------------
void dataMessage::assign(std::string s)
{
	std::string delimiter = "/?";

	this->m_payload = s.substr(0, s.find(delimiter));
	s.erase(0, s.find(delimiter) + delimiter.length());
	this->m_sourceID = s.substr(0, s.find(delimiter));
	s.erase(0, s.find(delimiter) + delimiter.length());
	this->m_destinationID = s.substr(0, s.find(delimiter));
	s.erase(0, s.find(delimiter) + delimiter.length());
};
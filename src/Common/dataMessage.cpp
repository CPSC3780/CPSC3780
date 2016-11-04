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
	boost::array<boost::asio::mutable_buffer, 3>& receivedBuffers)
{
	// #TODO_AH this code does not like vectors, switch to char arrays?
	/*
	std::vector<char> payloadVector = 
		boost::asio::buffer_cast<std::vector<char>>(receivedBuffers[0]);

	this->m_payload = std::string(
		payloadVector.begin(), 
		payloadVector.end());

	std::vector<char> sourceIDVector =
		boost::asio::buffer_cast<std::vector<char>>(receivedBuffers[1]);

	this->m_sourceID = std::string(
		sourceIDVector.begin(),
		sourceIDVector.end());

	std::vector<char> destinationIDVector =
		boost::asio::buffer_cast<std::vector<char>>(receivedBuffers[2]);

	this->m_destinationID = std::string(
		destinationIDVector.begin(),
		destinationIDVector.end());
	*/
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
		boost::asio::buffer(payloadToSend),
			boost::asio::buffer(sourceToSend),
			boost::asio::buffer(destinationToSend)};
};
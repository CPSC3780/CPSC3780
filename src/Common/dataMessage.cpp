// STL
#include <string>
#include <iostream>

// Project
#include "dataMessage.h"
#include "constants.h"

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Used to create a data message object to send
//------------------------------------------------------------------------------
dataMessage::dataMessage(
	const std::string& inPayload,
	const std::string& inSourceID,
	const std::string& inDestinationID = "broadcast",
	const int inMessageType = constants::CHAT)
{
	this->m_payload = inPayload;
	this->m_sourceID = inSourceID;
	this->m_destinationID = inDestinationID;
	this->m_messageType = inMessageType;
};

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Used to create a data message object from a received vector<char>
//------------------------------------------------------------------------------
dataMessage::dataMessage(
	const std::vector<char>& inCharVector)
{
	std::string asString(
		inCharVector.begin(),
		inCharVector.end());

	this->m_payload = asString.substr(0, asString.find(constants::messageDelimiter()));
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	this->m_sourceID = asString.substr(0, asString.find(constants::messageDelimiter()));
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	this->m_destinationID = asString.substr(0, asString.find(constants::messageDelimiter()));
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	std::string messageType = asString.substr(0, asString.find(constants::messageDelimiter()));
	this->m_messageType = this->messageTypeStringToInt(messageType);
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());
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

//------------------------------------------------------------ viewMessageType
// Implementation notes:
//  Returns a const reference to the message type
//------------------------------------------------------------------------------
const int dataMessage::viewMessageType() const
{
	return this->m_messageType;
};

//------------------------------------------------------------ viewMessageTypeAsString
// Implementation notes:
//  Returns a const string reference to the message type
//------------------------------------------------------------------------------
const std::string dataMessage::viewMessageTypeAsString() const
{
	switch(this->m_messageType)
	{
	case constants::CONNECTION:
		return "connection";
	case constants::DISCONNECT:
		return "disconnect";
	case constants::PRIVATE:
		return "private";
	case constants::CHAT:
		return "chat";
	default:
		return "other";
	}
};

//------------------------------------------------------------ viewMessageTypeAsString
// Implementation notes:
//  Returns a const int reference to the message type
//------------------------------------------------------------------------------
const int dataMessage::messageTypeStringToInt(std::string& type) const
{
	if(type == "connection")
	{
		return constants::CONNECTION;
	}
	if(type == "private")
	{
		return constants::PRIVATE;
	}
	if(type == "disconnect")
	{
		return constants::DISCONNECT;
	}
	if(type == "chat")
	{
		return constants::CHAT;
	}
};

//----------------------------------------------------------------- asVectorChar
// Implementation notes:
//  Returns data message as a vector<char>
//------------------------------------------------------------------------------
std::vector<char> dataMessage::asCharVector() const
{
	const std::string messageAsString(
		this->m_payload + constants::messageDelimiter() 
		+ this->m_sourceID + constants::messageDelimiter()
		+ this->m_destinationID + constants::messageDelimiter()
		+ this->viewMessageTypeAsString() + constants::messageDelimiter());

	return std::vector<char>(
		messageAsString.begin(),
		messageAsString.end());
};
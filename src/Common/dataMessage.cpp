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
	const constants::MessageType& inMessageType = constants::MessageType::CHAT)
{
	this->m_payload = inPayload;
	this->m_sourceIdentifier = inSourceID;
	this->m_destinationIdentifier = inDestinationID;
	this->m_messageType = inMessageType;
};

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Used to create a data message object to send
//------------------------------------------------------------------------------
dataMessage::dataMessage(
	const std::vector<remoteConnection>& inServerSyncPayload,
	const std::string& inSourceID,
	const std::string& inDestinationID = "broadcast",
	const constants::MessageType& inMessageType = constants::MessageType::CHAT)
{
	this->m_payload = dataMessage::createServerSyncPayload(inServerSyncPayload);
	this->m_sourceIdentifier = inSourceID;
	this->m_destinationIdentifier = inDestinationID;
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

	this->m_sourceIdentifier = asString.substr(0, asString.find(constants::messageDelimiter()));
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	this->m_destinationIdentifier = asString.substr(0, asString.find(constants::messageDelimiter()));
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	std::string messageType = asString.substr(0, asString.find(constants::messageDelimiter()));
	this->m_messageType = this->stringToMessageType(messageType);
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

//--------------------------------------------------------- viewSourceIdentifier
// Implementation notes:
//  Returns a const reference to the sourceIdentifier string
//------------------------------------------------------------------------------
const std::string& dataMessage::viewSourceIdentifier() const
{
	return this->m_sourceIdentifier;
};

//---------------------------------------------------- viewDestinationIdentifier
// Implementation notes:
//  Returns a const reference to the destinationIdentifier string
//------------------------------------------------------------------------------
const std::string& dataMessage::viewDestinationIdentifier() const
{
	return this->m_destinationIdentifier;
};

//-------------------------------------------------------------- viewMessageType
// Implementation notes:
//  Returns a const reference to the message type
//------------------------------------------------------------------------------
const constants::MessageType& dataMessage::viewMessageType() const
{
	return this->m_messageType;
};

//------------------------------------------------------ viewMessageTypeAsString
// Implementation notes:
//  Returns a const string reference to the message type
//------------------------------------------------------------------------------
const std::string dataMessage::viewMessageTypeAsString() const
{
	std::string messageTypeAsString;

	switch(this->m_messageType)
	{
		case constants::MessageType::CONNECTION:
		{
			messageTypeAsString = "connection";
			break;
		}	
		case constants::MessageType::PRIVATE_MESSAGE:
		{
			messageTypeAsString = "private";
			break;
		}
		case constants::MessageType::DISCONNECT:
		{
			messageTypeAsString = "disconnect";
			break;
		}
		case constants::MessageType::CHAT:
		{
			messageTypeAsString = "chat";
			break;
		}
		case constants::MessageType::SYNC_LEFT:
		{
			messageTypeAsString = "sync_left";
			break;
		}
		case constants::MessageType::SYNC_RIGHT:
		{
			messageTypeAsString = "sync_right";
			break;
		}
		case constants::MessageType::PING:
		{
			messageTypeAsString = "ping";
			break;
		}
		default:
		{
			assert(false);
		}
	}

	return messageTypeAsString;
}

//------------------------------------------------------ createServerSyncPayload
// Implementation notes:
//  Creates a string containing all the client names connected to the server.
//------------------------------------------------------------------------------
std::string dataMessage::createServerSyncPayload(
	const std::vector<remoteConnection>& inSyncPayload)
{
	// #TODO_AH implement
	return "";
};

//--------------------------------------------------------- viewServerSyncPayload
// Implementation notes:
//  Converts the string to the corresponding messageType enum
//------------------------------------------------------------------------------
std::vector<std::string> dataMessage::viewServerSyncPayload() const
{
	return std::vector<std::string>();
};

//---------------------------------------------------------- stringToMessageType
// Implementation notes:
//  Converts the string to the corresponding messageType enum
//------------------------------------------------------------------------------
const constants::MessageType dataMessage::stringToMessageType(
	const std::string& type) const
{
	if(type == "connection")
	{
		return constants::MessageType::CONNECTION;
	}

	if(type == "private")
	{
		return constants::MessageType::PRIVATE_MESSAGE;
	}

	if(type == "disconnect")
	{
		return constants::MessageType::DISCONNECT;
	}

	if(type == "chat")
	{
		return constants::MessageType::CHAT;
	}

	if(type == "sync_right")
	{
		return constants::SYNC_RIGHT;
	}

	if(type == "sync_left")
	{
		return constants::SYNC_LEFT;
	}

	if(type == "ping")
	{
		return constants::PING;
	}

	assert(false);

	return constants::MessageType::UNDEFINED;
};

//----------------------------------------------------------------- asVectorChar
// Implementation notes:
//  Returns data message as a vector<char>
//------------------------------------------------------------------------------
std::vector<char> dataMessage::asCharVector() const
{
	const std::string messageAsString(
		this->m_payload + constants::messageDelimiter()
		+ this->m_sourceIdentifier + constants::messageDelimiter()
		+ this->m_destinationIdentifier + constants::messageDelimiter()
		+ this->viewMessageTypeAsString() + constants::messageDelimiter());

	return std::vector<char>(
		messageAsString.begin(),
		messageAsString.end());
};
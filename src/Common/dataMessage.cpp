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
	const int64_t& inSequenceNumber,
	const constants::MessageType& inMessageType,
	const std::string& inSourceID,
	const std::string& inDestinationID,
	const std::string& inPayload)
{
	this->m_sequenceNumber = inSequenceNumber;
	this->m_messageType = inMessageType;
	this->m_sourceIdentifier = inSourceID;
	this->m_destinationIdentifier = inDestinationID;
	this->m_payload = inPayload;
	this->m_serverSyncPayloadOriginIndex = -1;
};

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Used to create a data message object to send
//------------------------------------------------------------------------------
dataMessage::dataMessage(
	const int64_t& inSequenceNumber,
	const constants::MessageType& inMessageType,
	const std::string& inSourceID,
	const std::string& inDestinationID,
	const std::vector<std::string>& inServerSyncPayload,
	const int8_t& inServerSyncPayloadOriginIndex)
{
	this->m_sequenceNumber = inSequenceNumber;
	this->m_messageType = inMessageType;
	this->m_sourceIdentifier = inSourceID;
	this->m_destinationIdentifier = inDestinationID;
	this->m_payload = dataMessage::createServerSyncPayload(inServerSyncPayload);
	this->m_serverSyncPayloadOriginIndex = inServerSyncPayloadOriginIndex;
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

	std::string sequenceNumberAsString = asString.substr(0, asString.find(constants::messageDelimiter()));
	this->m_sequenceNumber = std::stoi(sequenceNumberAsString);
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	std::string messageType = asString.substr(0, asString.find(constants::messageDelimiter()));
	this->m_messageType = this->stringToMessageType(messageType);
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	this->m_sourceIdentifier = asString.substr(0, asString.find(constants::messageDelimiter()));
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	this->m_destinationIdentifier = asString.substr(0, asString.find(constants::messageDelimiter()));
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	this->m_payload = asString.substr(0, asString.find(constants::messageDelimiter()));
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());

	std::string serverSyncPayloadOriginIndexAsString = asString.substr(0, asString.find(constants::messageDelimiter()));
	this->m_serverSyncPayloadOriginIndex = std::stoi(serverSyncPayloadOriginIndexAsString);
	asString.erase(0, asString.find(constants::messageDelimiter()) + constants::messageDelimiter().length());
};

//----------------------------------------------------------- viewSequenceNumber
// Implementation notes:
//  #TODO_AH fix me
//------------------------------------------------------------------------------
const int64_t& dataMessage::viewSequenceNumber() const
{
	return this->m_sequenceNumber;
};

//-------------------------------------------------------------- viewMessageType
// Implementation notes:
//  Returns a const reference to the message type
//------------------------------------------------------------------------------
const constants::MessageType& dataMessage::viewMessageType() const
{
	return this->m_messageType;
};

//--------------------------------------------------------------- setMessageType
// Implementation notes:
//  Sets the messageType to the inMessageType for this object
//------------------------------------------------------------------------------
void dataMessage::setMessageType(
	const constants::MessageType& inMessageType)
{
	this->m_messageType = inMessageType;
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

//------------------------------------------------------------------ viewPayload
// Implementation notes:
//  Returns a const reference to the payload string
//------------------------------------------------------------------------------
const std::string& dataMessage::viewPayload() const
{
	return this->m_payload;
};

//--------------------------------------------- viewServerSyncPayloadOriginIndex
// Implementation notes:
//  Returns a const reference to the payload string
//------------------------------------------------------------------------------
const int8_t& dataMessage::viewServerSyncPayloadOriginIndex() const
{
	return this->m_serverSyncPayloadOriginIndex;
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
		case constants::MessageType::mt_CLIENT_CONNECT:
		{
			messageTypeAsString = "client connect";
			break;
		}	
		case constants::MessageType::mt_CLIENT_DISCONNECT:
		{
			messageTypeAsString = "client disconnect";
			break;
		}
		case constants::MessageType::mt_CLIENT_SEND:
		{
			messageTypeAsString = "client send";
			break;
		}
		case constants::MessageType::mt_CLIENT_GET:
		{
			messageTypeAsString = "client get";
			break;
		}
		case constants::MessageType::mt_CLIENT_ACK:
		{
			messageTypeAsString = "client ack";
			break;
		}
		case constants::MessageType::mt_SERVER_SEND:
		{
			messageTypeAsString = "server send";
			break;
		}
		case constants::MessageType::mt_SERVER_ACK:
		{
			messageTypeAsString = "server ack";
			break;
		}
		case constants::MessageType::mt_SERVER_SYNC:
		{
			messageTypeAsString = "server sync";
			break;
		}
		case constants::MessageType::mt_PING:
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
};

//---------------------------------------------------------- stringToMessageType
// Implementation notes:
//  Converts the string to the corresponding messageType enum
//------------------------------------------------------------------------------
const constants::MessageType dataMessage::stringToMessageType(
	const std::string& inMessageTypeAsString) const
{
	if(inMessageTypeAsString == "client connect")
	{
		return constants::MessageType::mt_CLIENT_CONNECT;
	}

	if(inMessageTypeAsString == "client disconnect")
	{
		return constants::MessageType::mt_CLIENT_DISCONNECT;
	}

	if(inMessageTypeAsString == "client send")
	{
		return constants::MessageType::mt_CLIENT_SEND;
	}

	if(inMessageTypeAsString == "client get")
	{
		return constants::MessageType::mt_CLIENT_GET;
	}

	if(inMessageTypeAsString == "client ack")
	{
		return constants::MessageType::mt_CLIENT_ACK;
	}

	if(inMessageTypeAsString == "server send")
	{
		return constants::MessageType::mt_SERVER_SEND;
	}

	if(inMessageTypeAsString == "server ack")
	{
		return constants::MessageType::mt_SERVER_ACK;
	}

	if(inMessageTypeAsString == "server sync")
	{
		return constants::mt_SERVER_SYNC;
	}

	if(inMessageTypeAsString == "ping")
	{
		return constants::mt_PING;
	}

	assert(false);

	return constants::MessageType::mt_UNDEFINED;
};

//------------------------------------------------------ createServerSyncPayload
// Implementation notes:
//  Creates a string containing all the client names connected to the server.
//------------------------------------------------------------------------------
std::string dataMessage::createServerSyncPayload(
	const std::vector<std::string>& inServerSyncPayload)
{
	std::string constructedPayload("");

	for(const std::string& currentClient : inServerSyncPayload)
	{
		constructedPayload += currentClient + constants::syncIdentifierDelimiter();
	}

	return constructedPayload;
};

//--------------------------------------------------------- viewServerSyncPayload
// Implementation notes:
//  Converts the string to the corresponding messageType enum
//------------------------------------------------------------------------------
std::vector<std::string> dataMessage::viewServerSyncPayload() const
{
	std::vector<std::string> outServerSyncPayload;

	std::string foundClient("");

	for(size_t i = 0; i < this->m_payload.size(); i++)
	{
		if(this->m_payload[i] != constants::syncIdentifierDelimiter())
		{
			foundClient += this->m_payload[i];
		}
		else
		{
			outServerSyncPayload.push_back(foundClient);
			foundClient = "";
		}
	}

	return outServerSyncPayload;
};

//----------------------------------------------------------------- asVectorChar
// Implementation notes:
//  Returns data message as a vector<char>
//------------------------------------------------------------------------------
std::vector<char> dataMessage::asCharVector() const
{
	const std::string messageAsString(
		std::to_string(this->m_sequenceNumber) + constants::messageDelimiter()
		+ this->viewMessageTypeAsString() + constants::messageDelimiter()
		+ this->m_sourceIdentifier + constants::messageDelimiter()
		+ this->m_destinationIdentifier + constants::messageDelimiter()
		+ this->m_payload + constants::messageDelimiter()
		+ std::to_string(this->m_serverSyncPayloadOriginIndex) + constants::messageDelimiter());

	return std::vector<char>(
		messageAsString.begin(),
		messageAsString.end());
};
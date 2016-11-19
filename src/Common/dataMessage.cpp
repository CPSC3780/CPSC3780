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
	const constants::MessageType& inMessageType = constants::MessageType::mt_RELAY_CHAT)
{
	this->m_payload = inPayload;
	this->m_sourceIdentifier = inSourceID;
	this->m_destinationIdentifier = inDestinationID;
	this->m_messageType = inMessageType;
	this->m_relayToAdjacentServers = true;
};

//------------------------------------------------------------------ constructor
// Implementation notes:
//  Used to create a data message object to send
//------------------------------------------------------------------------------
dataMessage::dataMessage(
	const std::vector<remoteConnection>& inServerSyncPayload,
	const std::string& inSourceID,
	const std::string& inDestinationID = "broadcast",
	const constants::MessageType& inMessageType = constants::MessageType::mt_RELAY_CHAT)
{
	this->m_payload = dataMessage::createServerSyncPayload(inServerSyncPayload);
	this->m_sourceIdentifier = inSourceID + " Server";
	this->m_destinationIdentifier = inDestinationID;
	this->m_messageType = inMessageType;
	this->m_relayToAdjacentServers = true;
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

	std::string relayStatusAsString = asString.substr(0, asString.find(constants::messageDelimiter()));
	this->m_relayToAdjacentServers = static_cast<bool>(std::stoi(relayStatusAsString));
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
}

//--------------------------------------------------------------- setMessageType
// Implementation notes:
//  Sets the messageType to the inMessageType for this object
//------------------------------------------------------------------------------
void dataMessage::setMessageType(
	const constants::MessageType& inMessageType)
{
	this->m_messageType = inMessageType;
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
		case constants::MessageType::mt_CLIENT_PRIVATE_CHAT:
		{
			messageTypeAsString = "private chat";
			break;
		}
		case constants::MessageType::mt_CLIENT_TARGET_NOT_FOUND:
		{
			messageTypeAsString = "target not found";
			break;
		}
		case constants::MessageType::mt_RELAY_CHAT:
		{
			messageTypeAsString = "relay chat";
			break;
		}
		case constants::MessageType::mt_SERVER_SYNC_RIGHT:
		{
			messageTypeAsString = "sync right";
			break;
		}
		case constants::MessageType::mt_SERVER_SYNC_LEFT:
		{
			messageTypeAsString = "sync left";
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
}

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

	if(inMessageTypeAsString == "private chat")
	{
		return constants::MessageType::mt_CLIENT_PRIVATE_CHAT;
	}

	if(inMessageTypeAsString == "target not found")
	{
		return constants::MessageType::mt_CLIENT_TARGET_NOT_FOUND;
	}

	if(inMessageTypeAsString == "relay chat")
	{
		return constants::MessageType::mt_RELAY_CHAT;
	}

	if(inMessageTypeAsString == "sync right")
	{
		return constants::mt_SERVER_SYNC_RIGHT;
	}

	if(inMessageTypeAsString == "sync left")
	{
		return constants::mt_SERVER_SYNC_LEFT;
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
	const std::vector<remoteConnection>& inSyncPayload)
{
	std::string constructedPayload("");

	for(const remoteConnection& currentConnection : inSyncPayload)
	{
		constructedPayload += currentConnection.viewIdentifier() 
			+ constants::syncIdentifierDelimiter();
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
}

//------------------------------------------------------- relayToAdjacentServers
// Implementation notes:
//  Returns a const reference to the serverRelayStatus
//------------------------------------------------------------------------------
const bool& dataMessage::relayToAdjacentServers() const
{
	return this->m_relayToAdjacentServers;
};

//--------------------------------------------------------- setServerRelayStatus
// Implementation notes:
//  Sets the serverRelayStatus to the inServerRelayStatus
//------------------------------------------------------------------------------
void dataMessage::setServerRelayStatus(
	const bool& inServerRelayStatus)
{
	this->m_relayToAdjacentServers = inServerRelayStatus;
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
		+ this->viewMessageTypeAsString() + constants::messageDelimiter()
		+ std::to_string(this->m_relayToAdjacentServers) + constants::messageDelimiter());

	return std::vector<char>(
		messageAsString.begin(),
		messageAsString.end());
};
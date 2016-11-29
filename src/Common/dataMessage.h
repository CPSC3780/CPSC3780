#pragma once

// STL
#include <string>
#include <vector>

// Boost
#include <boost/array.hpp>
#include <boost/asio.hpp>

// Project
#include "../Common/constants.h"
#include "../Common/remoteConnection.h"

class dataMessage
{
public:
	

	//-------------------------------------------------------------- dataMessage
	// Brief Description
	//  Constructor for the data message. Primarily used by the client to create a 
	//  message to send to the server, but can also be used by the server
	//  to send it a message following a get request.
	//
	// Method:    dataMessage
	// FullName:  dataMessage::dataMessage
	// Access:    public 
	// Returns:   
	// Parameter: const int64_t& inSequenceNumber
	// Parameter: const constants::MessageType& inMessageType
	// Parameter: const std::string& inSourceID
	// Parameter: const std::string& inDestinationID
	// Parameter: const std::string& inPayload
	//--------------------------------------------------------------------------
	dataMessage(
		const int64_t& inSequenceNumber,
		const constants::MessageType& inMessageType,
		const std::string& inSourceID,
		const std::string& inDestinationID,
		const std::string& inPayload);


	//-------------------------------------------------------------- dataMessage
	// Brief Description
	//  Constructor for the data message. This constructor is used explicitly
	//  by servers to send each other sync payloads.
	//
	// Method:    dataMessage
	// FullName:  dataMessage::dataMessage
	// Access:    public 
	// Returns:   
	// Parameter: const int64_t& inSequenceNumber
	// Parameter: const constants::MessageType& inMessageType
	// Parameter: const std::string& inSourceID
	// Parameter: const std::string& inDestinationID
	// Parameter: const std::vector<std::string>& inServerSyncPayload
	// Parameter: const int8_t& inServerSyncPayloadOriginIndex
	//--------------------------------------------------------------------------
	dataMessage(
		const int64_t& inSequenceNumber,
		const constants::MessageType& inMessageType,
		const std::string& inSourceID,
		const std::string& inDestinationID,
		const std::vector<std::string>& inServerSyncPayload,
		const int8_t& inServerSyncPayloadOriginIndex);

	//-------------------------------------------------------------- constructor
	// Brief Description
	//  Constructor for the data message. Used primarily for receiving
	//  messages. Messages arrive as a vector of chars, and this constructor 
	//  turns it back to the more usable data message object.
	//
	// Method:    dataMessage
	// FullName:  dataMessage::dataMessage
	// Access:    public 
	// Returns:   
	// Parameter: const std::vector<char>& inCharVector
	//--------------------------------------------------------------------------
	dataMessage(
		const std::vector<char>& inCharVector);


	//------------------------------------------------------- viewSequenceNumber
	// Brief Description
	//  Returns a const reference to the sequence number. This can be used
	//  by the server and client to verify if a message was received or not
	//  if the same sequence number is sent back in an ACK.
	//
	// Method:    viewSequenceNumber
	// FullName:  dataMessage::viewSequenceNumber
	// Access:    public 
	// Returns:   const int64_t&
	//--------------------------------------------------------------------------
	const int64_t& viewSequenceNumber() const;

	//---------------------------------------------------------- viewMessageType
	// Brief Description
	//  Returns a const reference to the message type. This tells the server
	//  what to do with a received message. (connection, disconnection, chat
	//  message, etc) Can also be used by the client to determine how to display
	//  a received message. (private vs broadcast)
	//
	// Method:    viewMessageType
	// FullName:  dataMessage::viewMessageType
	// Access:    public 
	// Returns:   const constants::MessageType&
	//--------------------------------------------------------------------------
	const constants::MessageType& viewMessageType() const;

	//----------------------------------------------------------- setMessageType
	// Brief Description
	//  Sets the message type to the inMessageType for this object.
	//
	// Method:    setMessageType
	// FullName:  dataMessage::setMessageType
	// Access:    public 
	// Returns:   void
	// Parameter: const constants::MessageType& inMessageType
	//--------------------------------------------------------------------------
	void setMessageType(
		const constants::MessageType& inMessageType);
	//----------------------------------------------------- viewSourceIdentifier
	// Brief Description
	//  Returns a const reference to the source identifier string. This will
	//  primarily represent a client username, and will mostly be used on
	//  a receiving client to display who a message came from.
	//
	// Method:    viewSourceIdentifier
	// FullName:  dataMessage::viewSourceIdentifier
	// Access:    public 
	// Returns:   const std::string&
	//--------------------------------------------------------------------------
	const std::string& viewSourceIdentifier() const;

	//------------------------------------------------ viewDestinationIdentifier
	// Brief Description
	//  Returns a const reference to the destination identifier string. This
	//  should always represent a specific client that a private message
	//  is intended for. It is used by the server to determine which connected
	//  client to relay the private message to.
	//
	// Method:    viewDestinationIdentifier
	// FullName:  dataMessage::viewDestinationIdentifier
	// Access:    public 
	// Returns:   const std::string&
	//--------------------------------------------------------------------------
	const std::string& viewDestinationIdentifier() const;

	//-------------------------------------------------------------- viewPayload
	// Brief Description
	//  Returns a const reference to the payload string. This should always
	//  represent the chat message a client wants to send.
	//
	// Method:    viewPayload
	// FullName:  dataMessage::viewPayload
	// Access:    public 
	// Returns:   const std::string&
	//--------------------------------------------------------------------------
	const std::string& viewPayload() const;


	//----------------------------------------- viewServerSyncPayloadOriginIndex
	// Brief Description
	//  This is used by servers when receiving a sync payload from another
	//  server. It specifies what server the list of clients in the payload
	//  for this message belongs to.
	//
	// Method:    viewServerSyncPayloadOriginIndex
	// FullName:  dataMessage::viewServerSyncPayloadOriginIndex
	// Access:    public 
	// Returns:   const int8_t&
	//--------------------------------------------------------------------------
	const int8_t& viewServerSyncPayloadOriginIndex() const;
	
	//------------------------------------------------------ stringToMessageType
	// Brief Description
	//  Converts an input string (usually from the command line) to the
	//  equivalent message type enum.
	//
	// Method:    stringToMessageType
	// FullName:  dataMessage::stringToMessageType
	// Access:    public 
	// Returns:   const constants::MessageType
	// Parameter: const std::string & inMessageTypeAsString
	//--------------------------------------------------------------------------
	const constants::MessageType stringToMessageType(
		const std::string& inMessageTypeAsString) const;

	//-------------------------------------------------- viewMessageTypeAsString
	// Brief Description
	//  Returns the string equivalent to the messageType of this object.
	//
	// Method:    viewMessageTypeAsString
	// FullName:  dataMessage::viewMessageTypeAsString
	// Access:    public 
	// Returns:   const std::string
	//--------------------------------------------------------------------------
	const std::string viewMessageTypeAsString() const;

	//-------------------------------------------------- createServerSyncPayload
	// Brief Description
	//  Creates the server sync payload as a string with delimiters different 
	//  from the message delimiters
	//
	// Method:    createServerSyncPayload
	// FullName:  dataMessage::createServerSyncPayload
	// Access:    public 
	// Returns:   void
	// Parameter: const std::vector<remoteConnection>& inServerSyncPayload
	//--------------------------------------------------------------------------
	static std::string createServerSyncPayload(
		const std::vector<std::string>& inServerSyncPayload);

	//---------------------------------------------------- viewServerSyncPayload
	// Brief Description
	//  Extracts a vector of strings representing the clients on the adjacent
	//  server from the string payload.
	//
	// Method:    viewServerSyncPayload
	// FullName:  dataMessage::viewServerSyncPayload
	// Access:    public 
	// Returns:   std::vector<std::string>
	//--------------------------------------------------------------------------
	std::vector<std::string> viewServerSyncPayload() const;

	//------------------------------------------------------------- asCharVector
	// Brief Description
	//  Returns a vector of chars that represents this dataMessage object. This
	//  is used to send messages both ways between the client and server through
	//  the boost library functions.
	//
	// Method:    asCharVector
	// FullName:  dataMessage::asCharVector
	// Access:    public 
	// Returns:   std::vector<char>
	//--------------------------------------------------------------------------
	std::vector<char> asCharVector() const;

private:	
	// Member Variables
	int64_t m_sequenceNumber;
	constants::MessageType m_messageType;
	std::string m_sourceIdentifier;
	std::string m_destinationIdentifier;
	std::string m_payload;
	int8_t m_serverSyncPayloadOriginIndex;
};
#pragma once

// STL
#include <string>
#include <vector>

// Boost
#include <boost/array.hpp>
#include <boost/asio.hpp>

// Project
#include "../Common/constants.h"

class dataMessage
{
public:
	
	//-------------------------------------------------------------- constructor
	// Brief Description
	//  Constructor for the data message. Used primarily for creating a message
	//  on the client side to send to the server. The parameters are populated
	//  based on the parsed string from the command line.
	//
	// Method:    dataMessage
	// FullName:  dataMessage::dataMessage
	// Access:    public 
	// Returns:   
	// Parameter: const std::string& inPayload
	// Parameter: const std::string& inSourceID
	// Parameter: const std::string& inDestinationID
	// Parameter: const constants::MessageType& inType
	//--------------------------------------------------------------------------
	dataMessage(
		const std::string& inPayload,
		const std::string& inSourceID,
		const std::string& inDestinationID,
		const constants::MessageType& inType);

	//-------------------------------------------------------------- constructor
	// Brief Description
	//  Constructor for the data message. Used primarily for creating messages
	//  on the server side. A received message arrive as a vector of chars, and
	//  this constructor turns it back to the more usable data message object.
	//
	// Method:    dataMessage
	// FullName:  dataMessage::dataMessage
	// Access:    public 
	// Returns:   
	// Parameter: const std::vector<char>& inCharVector
	//--------------------------------------------------------------------------
	dataMessage(
		const std::vector<char>& inCharVector);

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

	//------------------------------------------------------ stringToMessageType
	// Brief Description
	//  Converts an input string (usually from the command line) to the
	//  equivalent message type enum.
	//
	// Method:    stringToMessageType
	// FullName:  dataMessage::stringToMessageType
	// Access:    public 
	// Returns:   const constants::MessageType
	// Parameter: const std::string&
	//--------------------------------------------------------------------------
	const constants::MessageType stringToMessageType(
		const std::string&) const;

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

	//----------------------------------------------------- setServerSyncPayload
	// Brief Description
	//  Sets the server sync payload as a string with delimiters different from
	//  the message delimiters
	//
	// Method:    setServerSyncPayload
	// FullName:  dataMessage::setServerSyncPayload
	// Access:    public 
	// Returns:   void
	// Parameter: const std::vector<std::string>& inSyncPayload
	//--------------------------------------------------------------------------
	void setServerSyncPayload(
		const std::vector<std::string>& inSyncPayload);

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
	std::string m_payload;
	std::string m_sourceIdentifier;
	std::string m_destinationIdentifier;
	constants::MessageType m_messageType;
};
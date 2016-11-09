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
	// #TODO_AH header comment blocks
	dataMessage(
		const std::string& inPayload,
		const std::string& inSourceID,
		const std::string& inDestinationID,
		const int inType);

	dataMessage(
		const std::vector<char>& inCharVector);

	const std::string& viewPayload() const;
	const std::string& viewSourceID() const;
	const std::string& viewDestinationID() const;
	const constants::MessageType& viewMessageType() const;
	const constants::MessageType stringToMessageType(const std::string&) const;
	const std::string viewMessageTypeAsString() const;

	std::vector<char> asCharVector() const;

private:	
	// Member Variables
	std::string m_payload;
	std::string m_sourceID;
	std::string m_destinationID;
	int m_messageType;
};
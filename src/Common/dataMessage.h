#pragma once

// STL
#include <string>
#include <vector>

// Boost
#include <boost/array.hpp>
#include <boost/asio.hpp>

class dataMessage
{
public:
	// #TODO_AH header comment blocks
	dataMessage(
		const std::string& inPayload,
		const std::string& inSourceID,
		const std::string& inDestinationID);

	dataMessage(
		boost::array<boost::asio::mutable_buffer, 3>& receivedBuffers);

	const std::string& viewPayload() const;
	const std::string& viewSourceID() const;
	const std::string& viewDestinationID() const;

	boost::array<boost::asio::const_buffer, 3> asConstBuffer() const;
	boost::array<boost::asio::mutable_buffer, 3> asMutableBuffer() const;

private:	
	// Member Variables
	std::string m_payload;
	std::string m_sourceID;
	std::string m_destinationID;
};
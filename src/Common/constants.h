#pragma once

// STL
#include <string>
#include <cstdint>
#include <cctype>
#include <string>
#include <vector>

namespace constants
{
	const uint16_t updateIntervalMilliseconds = 50;
	const uint16_t syncIntervalMilliseconds = 750;

	const std::vector<uint16_t> serverListeningPorts(
	{8080, 8081, 8082, 8083, 8084});

	const std::vector<std::string> serverNames(
	{"Alpha", "Bravo", "Charlie", "Delta", "Echo"});

	//-------------------------------------------------------- charToServerIndex
	// Brief Description
	//  Converts the input char to the server index. The server index can later
	//  be used to determine the associated port via indexing at
	//  serverListeningPorts[index]
	//
	// Method:    charToServerIndex
	// FullName:  constants::charToServerIndex
	// Access:    public static 
	// Returns:   uint8_t
	// Parameter: const char& inChar
	//--------------------------------------------------------------------------
	static uint8_t charToServerIndex(
		const char& inChar)
	{
		const char inCharAsLower(
			std::tolower(inChar));

		return static_cast<uint8_t>(inCharAsLower - 'a');
	};

	//----------------------------------------------- serverIndexToListeningPort
	// Brief Description
	//  Returns the listening port associated with a server index.
	//
	// Method:    serverIndexToListeningPort
	// FullName:  constants::serverIndexToListeningPort
	// Access:    public static 
	// Returns:   uint16_t
	// Parameter: const uint8_t& inServerIndex
	//--------------------------------------------------------------------------
	static uint16_t serverIndexToListeningPort(
		const uint8_t& inServerIndex)
	{
		return constants::serverListeningPorts[inServerIndex];
	};

	//-------------------------------------------------- serverIndexToServerName
	// Brief Description
	//  Returns the name of the server associated with the given server index.
	//
	// Method:    portNumberToServerName
	// FullName:  constants::portNumberToServerName
	// Access:    public static 
	// Returns:   std::string
	// Parameter: const uint8_t& inServerIndex
	//--------------------------------------------------------------------------
	static std::string serverIndexToServerName(
		const uint8_t& inServerIndex)
	{
		return constants::serverNames[inServerIndex];;
	};

	//-------------------------------------------------------- identifierIsValid
	// Brief Description
	//  Used to determine if the user specified server identifier is valid.
	//  (As per the specification, only a-e are supported. Input can be
	//  uppercase, as char is converted to lowercase before comparison is done.
	//
	// Method:    identifierIsValid
	// FullName:  constants::identifierIsValid
	// Access:    public static 
	// Returns:   bool
	// Parameter: const char & inChar
	//--------------------------------------------------------------------------
	static bool identifierIsValid(
		const char& inChar)
	{
		const char inCharAsLower(
			std::tolower(inChar));

		return (inCharAsLower >= 'a') && (inCharAsLower <= 'e');
	};

	//--------------------------------------------------------- messageDelimiter
	// Brief Description
	//  The character sequence used to delimit messages sent both ways between
	//  server and client.
	//
	// Method:    messageDelimiter
	// FullName:  constants::messageDelimiter
	// Access:    public static 
	// Returns:   std::string
	//--------------------------------------------------------------------------
	static inline std::string messageDelimiter()
	{
		return "/?";
	};

	//----------------------------------------------------- syncMessageDelimiter
	// Brief Description
	//  The character sequence used to delimit sync messages sent between
	//  servers.
	//
	// Method:    messageDelimiter
	// FullName:  constants::messageDelimiter
	// Access:    public static 
	// Returns:   std::string
	//--------------------------------------------------------------------------
	static inline std::string syncMessageDelimiter()
	{
		return "/!";
	};

	enum MessageType
	{
		UNDEFINED = 0,
		CONNECTION = 1,
		PRIVATE_MESSAGE = 2,
		DISCONNECT = 3,
		CHAT = 4,
		SYNC_LEFT = 5,
		SYNC_RIGHT = 6,
		PING = 7,
	};
}
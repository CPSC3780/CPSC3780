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
	const uint16_t syncIntervalMilliseconds = 2000;

	const std::vector<uint16_t> serverListeningPorts(
	{8080, 8081, 8082, 8083, 8084});

	const int8_t highestServerIndex = 
		(static_cast<int8_t>(constants::serverListeningPorts.size()) - 1);

	const std::vector<std::string> serverNames(
	{"Alpha", "Bravo", "Charlie", "Delta", "Echo"});

	static std::string serverHostName(
		const uint8_t& inServerIndex)
	{
		return constants::serverNames[inServerIndex]
			+ "ServerCPSC3780";
	}

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


	static bool leftAdjacentServerIndexIsValid(
		const int8_t& inLeftAdjacentServerIndex)
	{
		return inLeftAdjacentServerIndex >= 0;
	};

	static bool rightAdjacentServerIndexIsValid(
		const int8_t& inRightAdjacentServerIndex)
	{
		return inRightAdjacentServerIndex <= constants::highestServerIndex;
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

	//-------------------------------------------------- syncIdentifierDelimiter
	// Brief Description
	//  The character used to delimit identifiers in sync messages
	//  sent between servers.
	//
	// Method:    syncIdentifierDelimiter
	// FullName:  constants::syncIdentifierDelimiter
	// Access:    public static 
	// Returns:   char
	//--------------------------------------------------------------------------
	static inline char syncIdentifierDelimiter()
	{
		return ',';
	};

	enum MessageType
	{
		mt_UNDEFINED = 0,
		mt_CLIENT_CONNECT = 1,
		mt_CLIENT_DISCONNECT = 2,
		mt_CLIENT_PRIVATE_CHAT = 3,
		mt_CLIENT_TARGET_NOT_FOUND = 4,
		mt_RELAY_CHAT = 5,
		mt_SERVER_RELAY_CHAT = 6,
		mt_SERVER_SYNC_LEFT = 7,
		mt_SERVER_SYNC_RIGHT = 8,
		mt_PING = 9,
	};
}
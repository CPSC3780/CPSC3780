#pragma once

// STL
#include <string>
#include <cstdint>
#include <cctype>
#include <string>

namespace constants
{
	const uint16_t updateIntervalMilliseconds = 50;
	const uint16_t syncIntervalMilliseconds = 750;

	const uint16_t listeningPortAlpha = 8080;
	const uint16_t listeningPortBravo = 8081;
	const uint16_t listeningPortCharlie = 8082;
	const uint16_t listeningPortDelta = 8083;
	const uint16_t listeningPortEcho = 8084;

	//------------------------------------- serverIdentifierToPortNumberAndIndex
	// Brief Description
	//  #TODO fill in description
	//
	// Method:    serverIdentifierToPortNumberAndIndex
	// FullName:  constants::serverIdentifierToPortNumberAndIndex
	// Access:    public static 
	// Returns:   void
	// Parameter: const char& inChar
	// Parameter: uint16_t& outListeningPort
	// Parameter: uint8_t& outServerIndex
	//--------------------------------------------------------------------------
	static void serverIdentifierToPortNumberAndIndex(
		const char& inChar,
		uint16_t& outListeningPort,
		uint8_t& outServerIndex)
	{
		const char inCharAsLower(
			std::tolower(inChar));

		outServerIndex = static_cast<uint8_t>('a' - inCharAsLower);

		switch(inCharAsLower)
		{
			case 'a':
			{
				outListeningPort =
					constants::listeningPortAlpha;
			}
			break;
			case 'b':
			{
				outListeningPort =
					constants::listeningPortBravo;
			}
			break;
			case 'c':
			{
				outListeningPort =
					constants::listeningPortCharlie;
			}
			break;
			case 'd':
			{
				outListeningPort =
					constants::listeningPortDelta;
			}
			break;
			case 'e':
			{
				outListeningPort =
					constants::listeningPortEcho;
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
	};

	//--------------------------------------------------- portNumberToServerName
	// Brief Description
	//  Returns the name of the server associated with the given port number.
	//
	// Method:    portNumberToServerName
	// FullName:  constants::portNumberToServerName
	// Access:    public static 
	// Returns:   std::string
	// Parameter: const uint16_t inPort
	//--------------------------------------------------------------------------
	static std::string portNumberToServerName(
		const uint16_t& inPort)
	{
		std::string serverName;

		switch(inPort)
		{
			case constants::listeningPortAlpha:
			{
				serverName = "Alpha";
			}
			break;
			case constants::listeningPortBravo:
			{
				serverName = "Bravo";
			}
			break;
			case constants::listeningPortCharlie:
			{
				serverName = "Charlie";
			}
			break;
			case constants::listeningPortDelta:
			{
				serverName = "Delta";
			}
			break;
			case constants::listeningPortEcho:
			{
				serverName = "Echo";
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}

		return serverName;
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

	enum MessageType
	{
		UNDEFINED = 0,
		CONNECTION = 1,
		PRIVATE_MESSAGE = 2,
		DISCONNECT = 3,
		CHAT = 4,
		SYNC = 5,
		PING = 6,
	};
}
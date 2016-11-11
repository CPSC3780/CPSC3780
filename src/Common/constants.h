#pragma once

// STL
#include <string>
#include <cstdint>
#include <string>

namespace constants
{
	const uint16_t updateIntervalMilliseconds = 50;

	const uint16_t listeningPortAlpha = 8080;
	const uint16_t listeningPortBravo = 8081;
	const uint16_t listeningPortCharlie = 8082;
	const uint16_t listeningPortDelta = 8083;
	const uint16_t listeningPortEcho = 8084;

	//---------------------------------------- briefServerIdentifierToPortNumber
	// Brief Description
	//  Converts a brief server identifier (eg: A, B, C, etc.) to the port 
	//  number associated with it.
	//
	// Method:    briefServerIdentifierToPortNumber
	// FullName:  constants::briefServerIdentifierToPortNumber
	// Access:    public static 
	// Returns:   uint16_t
	// Parameter: const char & inChar
	//--------------------------------------------------------------------------
	static uint16_t briefServerIdentifierToPortNumber(
		const char& inChar)
	{
		uint16_t listeningPort;

		switch(inChar)
		{
			case 'A':
			case 'a':
			{
				listeningPort =
					constants::listeningPortAlpha;
			}
			break;
			case 'B':
			case 'b':
			{
				listeningPort =
					constants::listeningPortBravo;
			}
			break;
			case 'C':
			case 'c':
			{
				listeningPort =
					constants::listeningPortCharlie;
			}
			break;
			case 'D':
			case 'd':
			{
				listeningPort =
					constants::listeningPortDelta;
			}
			break;
			case 'E':
			case 'e':
			{
				listeningPort =
					constants::listeningPortEcho;
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}

		return listeningPort;
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
	//  (As per the specification, only A-E are supported. Lower and uppercase
	//  are supported for ease of use.
	//
	// Method:    identifierIsValid
	// FullName:  constants::identifierIsValid
	// Access:    public static 
	// Returns:   bool
	// Parameter: const char & inIdentifier
	//--------------------------------------------------------------------------
	static bool identifierIsValid(
		const char& inIdentifier)
	{
		return ((inIdentifier >= 'A') && (inIdentifier <= 'E'))
			|| ((inIdentifier >= 'a') && (inIdentifier <= 'e'));
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
	};
}
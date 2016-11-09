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

	static uint16_t identifierToPortNumber(
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

	static std::string portNumberToServerName(
		const uint16_t inPort)
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

	static bool identifierIsValid(
		const char& inIdentifier)
	{
		return ((inIdentifier >= 'A') && (inIdentifier <= 'E'))
			|| ((inIdentifier >= 'a') && (inIdentifier <= 'e'));
	};

	static inline std::string messageDelimiter()
	{
		return "/?";
	};

	enum MessageType
	{
		CONNECTION = 1,
		PRIVATE_MESSAGE = 2,
		DISCONNECT = 3,
		CHAT = 4,
		UNDEFINED = 5,
	};
}
#include <string>

namespace constants
{
	static inline std::string messageDelimiter()
	{
		return "/?";
	}

	enum MessageType
	{
		CONNECTION = 1,
		PRIVATE_MESSAGE = 2,
		DISCONNECT = 3,
		CHAT = 4,
	};
}
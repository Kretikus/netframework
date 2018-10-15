#pragma once

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <string>
#include <vector>

namespace coreproto {
	const int headerSize = 6;
	const int magicSize = 2;
	const uint32_t maxPacketSize = 0x3FFFFFFF; // 1GB

	std::vector<char> createPacket(const std::vector<char>& data) {
		std::vector<char> ret = { 'R', 'H', 0, 0, 0, 0 };
		uint32_t* szPtr = (uint32_t*)&ret[magicSize];
		*szPtr = htonl(data.size());
		ret.insert(ret.end(), data.begin(), data.end());
		return ret;
	}

	bool isPacketComplete(const std::vector<char>& data) {
		uint32_t sz = data.size();
		if (sz < headerSize) return false;
		if (!(data[0] == 'R' && data[1] == 'H')) return false;
		const uint32_t* szPtr = (uint32_t*)&data[magicSize];
		const unsigned int packetSize = ntohl(*szPtr);
		if (packetSize > maxPacketSize) return false;
		if (sz < packetSize + headerSize) return false;
		return true;
	}
}

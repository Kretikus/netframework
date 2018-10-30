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

    inline std::vector<char> createPacket(const std::string& data) {
        std::vector<char> ret = { 'N', 'F', 0, 0, 0, 0 };
        uint32_t* szPtr = reinterpret_cast<uint32_t*>(&ret[magicSize]);
        *szPtr = htonl(data.size());
        ret.insert(ret.end(), data.begin(), data.end());
        return ret;
    }

    inline std::vector<char> createPacket(const std::vector<char>& data) {
        std::vector<char> ret = { 'N', 'F', 0, 0, 0, 0 };
        uint32_t* szPtr = reinterpret_cast<uint32_t*>(&ret[magicSize]);
        *szPtr = htonl(data.size());
        ret.insert(ret.end(), data.begin(), data.end());
        return ret;
    }

    template<int size>
    inline bool isPacketStart(const char (&data)[size]) {
        if (size < headerSize) return false;
        if (!(data[0] == 'N' && data[1] == 'F')) return false;
        return true;
    }

    inline bool isPacketComplete(const std::vector<char>& data, uint32_t& pkgSize) {
        std::size_t sz = data.size();
        if (sz < headerSize) return false;
        if (!(data[0] == 'N' && data[1] == 'F')) return false;
        const uint32_t* szPtr = reinterpret_cast<const uint32_t*>(&data[magicSize]);
        pkgSize = ntohl(*szPtr);
        if (pkgSize > maxPacketSize) return false;
        if (sz < pkgSize + headerSize) return false;
        return true;
    }

    template<int size>
    inline bool isPacketComplete(const char (&data)[size], uint32_t& pkgSize) {
        static_assert (headerSize < size, "Size must be larger than the header size!");
        if (!(data[0] == 'N' && data[1] == 'F')) return false;
        const uint32_t* szPtr = reinterpret_cast<const uint32_t*>(&data[magicSize]);
        pkgSize = ntohl(*szPtr);
        if (pkgSize > maxPacketSize) return false;
        if (size < pkgSize + headerSize) return false;
        return true;
    }

    inline std::string fetchData(const std::vector<char>& data, uint32_t bytes) {
        if(headerSize < data.size()) return std::string();
        if (bytes > data.size() + headerSize) return std::string();
        std::string ret(&data[headerSize], bytes);
        return ret;
    }

    template<int size>
    inline std::string fetchData(const char (&data)[size], uint32_t bytes) {
        static_assert (headerSize < size, "Size must be larger than the header size!");
        if (bytes > size + headerSize) return std::string();
        std::string ret(&data[headerSize], bytes);
        return ret;
    }
}

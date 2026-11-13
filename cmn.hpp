
#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>
#include <vector>

// Signature used to identify our hidden payload.
constexpr std::uint8_t MAGIC[4] = {'L', 'S', 'B', '1'};

// Header contains 4 signature bytes + 8 payload-length bytes.
constexpr std::size_t HEADER_SIZE = 12;

// Create the payload header.
std::vector<std::uint8_t> makePayloadHeader(std::uint64_t size);

// Read the hidden payload length from the header.
std::uint64_t readPayloadSize(
    const std::vector<std::uint8_t>& bytes
);

#endif


#include "cmn.hpp"

// Creates the metadata stored before the secret data.
std::vector<std::uint8_t> makePayloadHeader(std::uint64_t size)
{
    std::vector<std::uint8_t> header;

    // Store the signature: LSB1
    for (int i = 0; i < 4; i++)
    {
        header.push_back(MAGIC[i]);
    }

    // Store the secret file size using 8 bytes.
    // Big-endian representation: most significant byte first.
    for (int i = 7; i >= 0; i--)
    {
        std::uint8_t byte =
            static_cast<std::uint8_t>((size >> (i * 8)) & 0xFF);

        header.push_back(byte);
    }

    return header;
}

// Extracts the secret file size from the metadata.
std::uint64_t readPayloadSize(
    const std::vector<std::uint8_t>& bytes)
{
    std::uint64_t size = 0;

    // The first 4 bytes contain the signature.
    // The next 8 bytes contain the file size.
    for (std::size_t i = 4; i < 12; i++)
    {
        size = (size << 8) | bytes[i];
    }

    return size;
}

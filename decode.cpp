
#if 0

#include "decoder.hpp"
#include "common.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

// Read eight carrier LSBs and reconstruct one byte.
static std::uint8_t readByteLSB(
    const std::vector<std::uint8_t>& carrier,
    std::size_t& bitIndex)
{
    std::uint8_t value = 0;

    for (int bit = 0; bit < 8; bit++)
    {
        // Extract the carrier byte's least significant bit.
        std::uint8_t extractedBit =
            carrier[bitIndex] & 1u;

        // Append the bit to the reconstructed byte.
        value = static_cast<std::uint8_t>(
            (value << 1) | extractedBit);

        bitIndex++;
    }

    return value;
}

// Extract the hidden payload from a BMP image.
bool decodeFile(
    const BmpImage& image,
    std::vector<std::uint8_t>& secret,
    std::string& error)
{
    // The metadata occupies 12 bytes, or 96 carrier bits.
    if (image.pixelDataSize < HEADER_SIZE * 8u)
    {
        error = "BMP is too small to contain the payload header.";
        return false;
    }

    // Copy the carrier region from the BMP.
    std::vector<std::uint8_t> carrier(
        image.fileBytes.begin() +
            static_cast<std::ptrdiff_t>(image.pixelOffset),
        image.fileBytes.begin() +
            static_cast<std::ptrdiff_t>(
                image.pixelOffset + image.pixelDataSize));

    std::size_t bitIndex = 0;

    // Recover the 12-byte metadata header.
    std::vector<std::uint8_t> header(HEADER_SIZE);

    for (std::uint8_t& byte : header)
    {
        byte = readByteLSB(carrier, bitIndex);
    }

    // Check whether this image contains our payload format.
    if (!std::equal(
            MAGIC,
            MAGIC + 4,
            header.begin()))
    {
        error =
            "No compatible hidden payload found. "
            "The LSB1 signature is missing.";
        return false;
    }

    // Retrieve the original secret file size.
    const std::uint64_t payloadSize =
        readPayloadSize(header);

    // Remaining capacity after the metadata header.
    const std::uint64_t availableBytes =
        image.pixelDataSize / 8u - HEADER_SIZE;

    // Validate the length before allocating memory.
    if (payloadSize > availableBytes ||
        payloadSize >
            std::numeric_limits<std::size_t>::max())
    {
        error =
            "Invalid payload length or damaged encoded image.";
        return false;
    }

    // Allocate enough memory for the recovered secret.
    secret.resize(static_cast<std::size_t>(payloadSize));

    // Reconstruct each secret byte.
    for (std::uint8_t& byte : secret)
    {
        byte = readByteLSB(carrier, bitIndex);
    }

    return true;
}
#endif


#if 1

#include "decode.hpp"
#include "cmn.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

// Read eight carrier LSBs and reconstruct one byte.
static std::uint8_t readByteLSB(
    const std::vector<std::uint8_t>& carrier,
    std::size_t& bitIndex)
{
    std::uint8_t value = 0;

    for (int bit = 0; bit < 8; bit++)
    {
        // Collect the LSB from the next carrier byte.
        value = static_cast<std::uint8_t>(
            (value << 1) | (carrier[bitIndex] & 1u));

        bitIndex++;
    }

    return value;
}

// Extract the hidden secret from the BMP image.
bool decodeFile(
    const BmpImage& image,
    std::vector<std::uint8_t>& secret,
    std::string& error)
{
    // The header needs 12 bytes, with one carrier byte per bit.
    if (image.pixelDataSize < HEADER_SIZE * 8u)
    {
        error = "BMP is too small to contain a payload header.";
        return false;
    }

    // Copy the pixel data into a separate carrier buffer.
    std::vector<std::uint8_t> carrier(
        image.fileBytes.begin() +
            static_cast<std::ptrdiff_t>(image.pixelOffset),
        image.fileBytes.begin() +
            static_cast<std::ptrdiff_t>(
                image.pixelOffset + image.pixelDataSize));

    std::size_t bitIndex = 0;

    // Read the complete 12-byte metadata header.
    std::vector<std::uint8_t> header(HEADER_SIZE);

    for (std::size_t i = 0; i < HEADER_SIZE; i++)
    {
        header[i] = readByteLSB(carrier, bitIndex);
    }

    // Verify that this image contains our payload format.
    if (!std::equal(
            MAGIC, MAGIC + 4, header.begin()))
    {
        error =
            "No compatible hidden payload found: LSB1 signature missing.";
        return false;
    }

    // Read the original secret file length.
    const std::uint64_t payloadSize =
        readPayloadSize(header);

    // Calculate how many payload bytes could fit in the image.
    const std::uint64_t availablePayloadBytes =
        image.pixelDataSize / 8u - HEADER_SIZE;

    // Validate the length before allocating memory.
    if (payloadSize > availablePayloadBytes ||
        payloadSize >
            std::numeric_limits<std::size_t>::max())
    {
        error =
            "Hidden payload length is invalid or the image is damaged.";
        return false;
    }

    secret.resize(static_cast<std::size_t>(payloadSize));

    // Reconstruct the original secret bytes.
    for (std::size_t i = 0; i < secret.size(); i++)
    {
        secret[i] = readByteLSB(carrier, bitIndex);
    }

    return true;
}


#endif
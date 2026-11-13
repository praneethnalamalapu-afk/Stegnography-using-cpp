
#include "encode.hpp"
#include "cmn.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

using namespace std; 
static void writeByteLSB(vector<uint8_t>& carrier,size_t& bitIndex,uint8_t value){
    // Process the most significant bit first.
    for (int bit = 7; bit >= 0; bit--)
    {
        // Extract one bit from the secret byte.
        uint8_t secretBit =
            static_cast<uint8_t>((value >> bit) & 1u);

        // Clear the carrier byte's LSB and insert the secret bit.
        carrier[bitIndex] =
            static_cast<uint8_t>(
                (carrier[bitIndex] & 0xFEu) | secretBit);

        bitIndex++;
    }
}

// Embed the secret file into the BMP pixel data.
bool encodeFile(BmpImage& image,const std::vector<std::uint8_t>& secret,
    std::string& error)
{
    // Header + secret file size, measured in bytes.
    const std::uint64_t totalBytes =
        static_cast<std::uint64_t>(HEADER_SIZE) + secret.size();

    // Prevent overflow when converting bytes to bits.
    if (totalBytes >
        std::numeric_limits<std::size_t>::max() / 8u)
    {
        error = "Secret file is too large.";
        return false;
    }

    const std::size_t requiredBits =
        static_cast<std::size_t>(totalBytes * 8u);

    // Every carrier byte stores one secret bit.
    if (requiredBits > image.pixelDataSize)
    {
        error =
            "Secret file is too large for this BMP image.";
        return false;
    }

    // Copy the BMP pixel region so we can encode into it.
    std::vector<std::uint8_t> carrier(
        image.fileBytes.begin() +
            static_cast<std::ptrdiff_t>(image.pixelOffset),
        image.fileBytes.begin() +
            static_cast<std::ptrdiff_t>(
                image.pixelOffset + image.pixelDataSize));

    // Prepare the signature and secret file length.
    const std::vector<std::uint8_t> header =
        makePayloadHeader(
            static_cast<std::uint64_t>(secret.size()));

    std::size_t bitIndex = 0;

    // Encode the metadata first.
    for (std::uint8_t byte : header)
    {
        writeByteLSB(carrier, bitIndex, byte);
    }

    // Encode the actual secret bytes.
    for (std::uint8_t byte : secret)
    {
        writeByteLSB(carrier, bitIndex, byte);
    }

    // Copy the modified carrier data back into the BMP.
    std::copy(
        carrier.begin(),
        carrier.end(),
        image.fileBytes.begin() +
            static_cast<std::ptrdiff_t>(image.pixelOffset));

    return true;
}


#ifndef BMP_HANDLER_HPP
#define BMP_HANDLER_HPP

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

// Stores the BMP file and the location of its pixel data.
struct BmpImage
{
    std::vector<std::uint8_t> fileBytes;

    std::size_t pixelOffset = 0;
    std::size_t pixelDataSize = 0;
};

// Load and validate an uncompressed 24-bit BMP image.
bool loadBmp24(
    const std::string& path,
    BmpImage& image,
    std::string& error
);

// Load any binary file into memory.
bool loadBinaryFile(
    const std::string& path,
    std::vector<std::uint8_t>& data,
    std::string& error
);

// Save bytes to a binary file.
bool saveBinaryFile(
    const std::string& path,
    const std::vector<std::uint8_t>& data,
    std::string& error
);

#endif

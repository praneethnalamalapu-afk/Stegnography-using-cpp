
#include "bmp_handle.hpp"

#include <fstream>
#include <limits>
#include <cstdint>

// Read a 32-bit unsigned integer stored in little-endian format.
static std::uint32_t readU32LE(
    const std::vector<std::uint8_t>& bytes,
    std::size_t position)
{
    return static_cast<std::uint32_t>(bytes[position]) |
           (static_cast<std::uint32_t>(bytes[position + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[position + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[position + 3]) << 24);
}

// Read a signed 32-bit integer stored in little-endian format.
static std::int32_t readI32LE(
    const std::vector<std::uint8_t>& bytes,
    std::size_t position)
{
    return static_cast<std::int32_t>(readU32LE(bytes, position));
}

// Read a file as raw bytes.
bool loadBinaryFile(const std::string& path,std::vector<std::uint8_t>& data,std::string& error){
    std::ifstream input(path, std::ios::binary);

    if (!input){
        error = "Cannot open input file: " + path;
        return false;
    }

    input.seekg(0, std::ios::end);
    std::streampos end = input.tellg();

    if (end < 0){
        error = "Cannot determine file size: " + path;
        return false;
    }

    data.resize(static_cast<std::size_t>(end));
    input.seekg(0, std::ios::beg);

    if (!data.empty() &&!input.read(reinterpret_cast<char*>(data.data()),static_cast<std::streamsize>(data.size()))){
        error = "Failed to read file: " + path;
        return false;
    }

    return true;
}

// Write raw bytes to a file.
bool saveBinaryFile(
    const std::string& path,const std::vector<std::uint8_t>& data,std::string& error){
    std::ofstream output(path, std::ios::binary);

    if (!output){
        error = "Cannot create output file: " + path;
        return false;
    }

    if (!data.empty()){
        output.write(reinterpret_cast<const char*>(data.data()),static_cast<std::streamsize>(data.size()));
    }

    if (!output)
    {
        error = "Failed to write file: " + path;
        return false;
    }

    return true;
}

// Load and validate an uncompressed 24-bit BMP.
bool loadBmp24(const std::string& path,BmpImage& image,std::string& error){
    if (!loadBinaryFile(path, image.fileBytes, error)){
        return false;
    }

    const auto& bytes = image.fileBytes;

    if (bytes.size() < 54){
        error = "Incomplete BMP header.";
        return false;
    }

    const bool hasBmpSignature =(bytes[0] == 'B' || bytes[0] == 'b') &&(bytes[1] == 'M' || bytes[1] == 'm');

    if (!hasBmpSignature){
        error = "Input file is not a BMP (expected BM signature).";
        return false;
    }

    const std::uint32_t pixelOffset = readU32LE(bytes, 10);
    const std::uint32_t dibSize = readU32LE(bytes, 14);

    if (dibSize < 40 ||static_cast<std::uint64_t>(14) + dibSize > bytes.size()){
        error = "Unsupported or incomplete BMP header.";
        return false;
    }

    const std::int32_t width = readI32LE(bytes, 18);
    const std::int32_t height = readI32LE(bytes, 22);

    const std::uint16_t planes =static_cast<std::uint16_t>(bytes[26] |(static_cast<std::uint16_t>(bytes[27]) << 8));

    const std::uint16_t bitsPerPixel =static_cast<std::uint16_t>(bytes[28] |(static_cast<std::uint16_t>(bytes[29]) << 8));

    const std::uint32_t compression = readU32LE(bytes, 30);

    if (width <= 0 ||height == 0 ||height == std::numeric_limits<std::int32_t>::min()){
        error = "Invalid BMP dimensions.";
        return false;
    }

    // BI_RGB = 0 means uncompressed bitmap.
    if (planes != 1 ||bitsPerPixel != 24 ||compression != 0){
        error = "Only uncompressed 24-bit BMP images are supported.";
        return false;
    }

    const std::uint64_t absHeight =height < 0? static_cast<std::uint64_t>(-static_cast<std::int64_t>(height)): static_cast<std::uint64_t>(height);

    // BMP rows are padded to a multiple of four bytes.
    const std::uint64_t rowStride = ((static_cast<std::uint64_t>(width) * 3 + 3) / 4) * 4;

    const std::uint64_t pixelSize = rowStride * absHeight;

    // Confirm the pixel data stays within the actual file.
    if (pixelOffset > bytes.size() ||pixelSize > bytes.size() - pixelOffset){
        error = "BMP pixel data is incomplete or invalid.";
        return false;
    }

    image.pixelOffset = pixelOffset;
    image.pixelDataSize = static_cast<std::size_t>(pixelSize);

    return true;
}

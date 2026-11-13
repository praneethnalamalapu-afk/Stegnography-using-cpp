
#ifndef DECODER_HPP
#define DECODER_HPP

#include "bmp_handle.hpp"
#include <cstdint>
#include <string>
#include <vector>

// Extract hidden bytes from a BMP image.
bool decodeFile(
    const BmpImage& image,
    std::vector<std::uint8_t>& secret,
    std::string& error
);

#endif

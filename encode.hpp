
#ifndef ENCODER_HPP
#define ENCODER_HPP

#include "bmp_handle.hpp"
#include <string>
#include <vector>

// Embed secret bytes inside the BMP pixel data.
bool encodeFile(
    BmpImage& image,
    const std::vector<std::uint8_t>& secret,
    std::string& error
);

#endif


#include "bmp_handle.hpp"
#include "encode.hpp"
#include "decode.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void printUsage(const char* app)
{
    std::cout
        << "LSB Image Steganography (C++17)\n\n"
        << "Encode:\n"
        << app
        << " -e <source.bmp> <secret.txt> <encoded.bmp>\n\n"
        << "Decode:\n"
        << app
        << " -d <encoded.bmp> <recovered.txt>\n";
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    const std::string mode = argv[1];
    std::string error;

    // Encoding mode.
    if (mode == "-e")
    {
        if (argc != 5)
        {
            printUsage(argv[0]);
            return 1;
        }

        BmpImage image;
        std::vector<std::uint8_t> secret;

        if (!loadBmp24(argv[2], image, error) ||
            !loadBinaryFile(argv[3], secret, error) ||
            !encodeFile(image, secret, error) ||
            !saveBinaryFile(argv[4], image.fileBytes, error))
        {
            std::cerr << "Error: " << error << '\n';
            return 1;
        }

        std::cout << "Encoding successful!\n";
        std::cout << "Output image: " << argv[4] << '\n';
        std::cout << "Secret bytes embedded: "
                  << secret.size() << '\n';

        return 0;
    }

    // Decoding mode.
    if (mode == "-d")
    {
        if (argc != 4)
        {
            printUsage(argv[0]);
            return 1;
        }

        BmpImage image;
        std::vector<std::uint8_t> secret;

        if (!loadBmp24(argv[2], image, error) ||
            !decodeFile(image, secret, error) ||
            !saveBinaryFile(argv[3], secret, error))
        {
            std::cerr << "Error: " << error << '\n';
            return 1;
        }

        std::cout << "Decoding successful!\n";
        std::cout << "Recovered file: " << argv[3] << '\n';
        std::cout << "Recovered bytes: "
                  << secret.size() << '\n';

        return 0;
    }

    printUsage(argv[0]);
    return 1;
}


/*
compile with:
g++ -std=c++17 -o lsb_stegnography src/main.cpp src/bmp_handle.cpp src/encode.cpp src/decode.cpp

run:
./a.exe -e <source.bmp> <secret.txt> <encoded.bmp>
./a.exe -d <encoded.bmp> <recovered.txt>
./a.exe -h

*/
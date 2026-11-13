# LSB Image Steganography

A small C++17 command-line program that hides a file in an uncompressed
24-bit BMP image using least-significant-bit (LSB) steganography. It can also
extract a payload previously embedded by this program.

## Features

- Embeds arbitrary file bytes, including text or binary data.
- Stores an `LSB1` signature and the original payload length in the image.
- Validates the BMP format and checks that the image has enough capacity.
- Reports errors for unsupported images, missing payloads, and invalid payload
  lengths.

## Requirements

- A C++17 compiler, such as GCC, Clang, or MinGW-w64.
- An uncompressed 24-bit BMP image (`BI_RGB`) to use as the carrier.

## Build

Run this command from the project directory:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic \
  src/main.cpp src/bmp_handle.cpp src/cmn.cpp src/encode.cpp src/decode.cpp \
  -o lsb_stegnography
```

On Windows with MinGW, use `-o lsb_stegnography.exe` as the output option.

## Usage

### Hide a file

```sh
./lsb_stegnography -e <source.bmp> <secret-file> <encoded.bmp>
```

For example:

```sh
./lsb_stegnography -e src/test.bmp secret.txt encoded.bmp
```

### Extract a file

```sh
./lsb_stegnography -d <encoded.bmp> <recovered-file>
```

For example:

```sh
./lsb_stegnography -d encoded.bmp recovered.txt
```

The recovered file is written as raw bytes, so it can also be given a binary
file extension when appropriate. Running the program without valid arguments
prints the usage summary.

## How it works

The encoder uses one pixel-data byte to store each payload bit, replacing only
that byte's least significant bit. It writes the payload most-significant bit
first. Before the payload, it stores a 12-byte header: the four-byte `LSB1`
signature followed by an eight-byte, big-endian payload length. The decoder
checks the signature and length before extracting the payload.

The maximum payload size is:

```text
floor(BMP pixel-data size in bytes / 8) - 12 bytes
```

The 12 bytes account for the header. BMP row padding is included in the pixel
data size used by this implementation.

## Limitations

- Only uncompressed 24-bit BMP images are supported; PNG, JPEG, compressed
  BMPs, and other bit depths are not.
- The payload is hidden, not encrypted. Anyone who knows the format can extract
  it; encrypt sensitive files separately before embedding them.
- Resizing, recompressing, or otherwise editing the carrier image can corrupt
  or remove the hidden payload.

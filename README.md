# LZ78 Compressor

Simple LZ78 compression tool written in C. Educational / WIP.

## Build
```bash
gcc main.c -o lz78
```

## Usage
```bash
./lz78 <input_file>
```

Creates `compressed.lz78`.

## Features
- Reads a file and builds an LZ78 dictionary using a linked list
- Writes bit-packed compressed data to `compressed.lz78`
- Prints the dictionary
- Includes a basic `decompress_string` test function

## Status
Compression works. Binary decoding is incomplete (`readBinaryFile`).  
`decompress_string` only supports single-digit references.  
Memory cleanup is incomplete.

## License
Educational use.

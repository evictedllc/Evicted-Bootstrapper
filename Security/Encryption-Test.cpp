#include <iostream>
#include <fstream>
#include <cstring>
#include <openssl/aes.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " [input_file] [output_file]" << endl;
        return 1;
    }

    // Open input file
    ifstream input_file(argv[1], ios::binary);
    if (!input_file.is_open()) {
        cout << "Error: failed to open input file" << endl;
        return 1;
    }

    // Open output file
    ofstream output_file(argv[2], ios::binary);
    if (!output_file.is_open()) {
        cout << "Error: failed to open output file" << endl;
        return 1;
    }

    // Generate encryption key and initialization vector
    unsigned char key[AES_BLOCK_SIZE];
    unsigned char iv[AES_BLOCK_SIZE];
    memset(key, 0x00, AES_BLOCK_SIZE);
    memset(iv, 0x00, AES_BLOCK_SIZE);

    // Set up AES encryption context
    AES_KEY aes_key;
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
        cout << "Error: failed to set up AES encryption context" << endl;
        return 1;
    }

    // Encrypt data in blocks
    unsigned char indata[AES_BLOCK_SIZE];
    unsigned char outdata[AES_BLOCK_SIZE];
    memset(indata, 0x00, AES_BLOCK_SIZE);
    memset(outdata, 0x00, AES_BLOCK_SIZE);

    while (input_file) {
        // Read block of input data
        input_file.read((char*)indata, AES_BLOCK_SIZE);
        int input_size = input_file.gcount();

        // Encrypt block of input data
        AES_cbc_encrypt(indata, outdata, input_size, &aes_key, iv, AES_ENCRYPT);

        // Write block of encrypted data to output file
        output_file.write((char*)outdata, input_size);
    }

    // Close input and output files
    input_file.close();
    output_file.close();

    cout << "Encryption complete" << endl;

    return 0;
}

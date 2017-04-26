#include <iostream>
#include <string>
#include <bitset>
#include <algorithm>

// Macros defined to reverse index of bitset<>
#define index_bits(a) (6 - a)
#define index_enc(a) (10 - a)

using namespace std;

// Functions for checking bits of encoded ascii data
bitset<1> checkBit1(bitset<11> encoded);

bitset<1> checkBit2(bitset<11> encoded);

bitset<1> checkBit4(bitset<11> encoded);

bitset<1> checkBit8(bitset<11> encoded);

bitset<7> decode(bitset<11> encoded);

int main() {

    string received;
    cout << "Enter received Code: ";
    cin >> received;

    bitset<11> encodedRec = bitset<11>(received);

    // Check all parity bits
    int bit1 = checkBit1(encodedRec)[0];
    int bit2 = checkBit2(encodedRec)[0];
    int bit4 = checkBit4(encodedRec)[0];
    int bit8 = checkBit8(encodedRec)[0];

    if(bit1 + bit2 + bit4 + bit8 == 0) { // All bits are 0. The data is correct.
        cout << "Received Code is correct!" << endl;
        cout << "Decoded code: " << decode(encodedRec) << endl;
    } else { // Some of the parity bits are 1. Calculate wrong position.
        int positionWrong = bit1 * 1 + bit2 * 2 + bit4 * 4 + bit8 * 8;
        cout << "Wrong bit is in position: " << positionWrong << endl;
        positionWrong--;

        // Correct code
        encodedRec.flip(index_enc(positionWrong));
        cout << "Correct Code is: " << encodedRec << endl;
        cout << "Decoded code: " << decode(encodedRec) << endl;
    }

    return 0;
}

// Check even bits for parity
bitset<1> checkBit1(bitset<11> encoded) {
    int counter = 0;
    for (size_t i = 0; i < 11; i += 2) {
        if (encoded.test(index_enc(i))) {
            counter += 1;
        }
    }

    return bitset<1>((unsigned long) (counter % 2 != 0));
}


bitset<1> checkBit2(bitset<11> encoded) {
    // Check these bits for parity
    // 2, 3, 6, 7, 10, 11
    // 1, 2, 5, 6, 9, 10 <- index 0
    int counter = encoded[index_enc(1)] + encoded[index_enc(2)] + encoded[index_enc(5)] + encoded[index_enc(6)]
                  + encoded[index_enc(9)] + encoded[index_enc(10)];
    return bitset<1>((unsigned long) (counter % 2 != 0));
}

bitset<1> checkBit4(bitset<11> encoded) {
    // Check these bits for parity
    // 4, 5, 6, 7
    // 3, 4, 5, 6 <- index 0
    int counter = encoded[index_enc(3)] + encoded[index_enc(4)] + encoded[index_enc(5)] + encoded[index_enc(6)];
    return bitset<1>((unsigned long) (counter % 2 != 0));
}

bitset<1> checkBit8(bitset<11> encoded) {
    // Check these bits for parity
    // 8 - 25
    // 7 - 14 <- index 0
    int counter = 0;
    for(int i = 7; i <= 14; i++) {
        counter += encoded[index_enc(i)];
    }
    return bitset<1>((unsigned long) (counter % 2 != 0));
}

// Get data bits from encoded ascii data (11 bits -> 7 bits)
bitset<7> decode(bitset<11> encoded) {
    bitset<7> decoded = bitset<7>(0);
    decoded[index_bits(0)] = encoded[index_enc(2)];
    decoded[index_bits(1)] = encoded[index_enc(4)];
    decoded[index_bits(2)] = encoded[index_enc(5)];
    decoded[index_bits(3)] = encoded[index_enc(6)];
    decoded[index_bits(4)] = encoded[index_enc(8)];
    decoded[index_bits(5)] = encoded[index_enc(9)];
    decoded[index_bits(6)] = encoded[index_enc(10)];
    return decoded;
}

/**
    Test Input: 1001001
    Correct: 11110011001
    Incorrect: 11110011101
**/
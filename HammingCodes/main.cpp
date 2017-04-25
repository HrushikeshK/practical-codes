#include <iostream>
#include <string>
#include <bitset>
#include <algorithm>

#define index_bits(a) (6 - a)
#define index_enc(a) (10 - a)

using namespace std;

bitset<1> checkBit1(bitset<11> encoded);

bitset<1> checkBit2(bitset<11> encoded);

bitset<1> checkBit4(bitset<11> encoded);

bitset<1> checkBit8(bitset<11> encoded);

bitset<7> decode(bitset<11> encoded);

int main() {

/*
    unsigned long n;
    cout << "Enter a 7 bit ASCII code: ";
    cin >> n;
    // std::reverse(n.begin(), n.end());
    bitset<7> bits = bitset<7>(n);
    // cout << "Binary Value is: " << bits.to_string() << endl;

    bitset<11> encoded = bitset<11>(0);

    encoded[index_enc(2)] = bits[index_bits(0)];
    encoded[index_enc(4)] = bits[index_bits(1)];

    for (int i = 5, j = 2; j < 7; i++, j++) {
        encoded[index_enc(i)] = bits[index_bits(j)];
    }

    encoded[index_enc(0)] = checkBit1(encoded)[0];
    encoded[index_enc(1)] = checkBit2(encoded)[0];
    encoded[index_enc(3)] = checkBit4(encoded)[0];
    encoded[index_enc(7)] = checkBit8(encoded)[0];

    cout << "Hamming Encoded Value: " << encoded.to_string() << endl;
*/

    string received;
    cout << "Enter received Code: ";
    cin >> received;

    bitset<11> encodedRec = bitset<11>(received);

    int bit1 = checkBit1(encodedRec)[0];
    int bit2 = checkBit2(encodedRec)[0];
    int bit4 = checkBit4(encodedRec)[0];
    int bit8 = checkBit8(encodedRec)[0];

    if(bit1 + bit2 + bit4 + bit8 == 0) {
        cout << "Received Code is correct!" << endl;
        cout << "Decoded code: " << decode(encodedRec) << endl;
    } else {
        int positionWrong = bit1 * 1 + bit2 * 2 + bit4 * 4 + bit8 * 8;
        cout << "Wrong bit is in position: " << positionWrong << endl;
        positionWrong--;

        encodedRec.flip(index_enc(positionWrong));
        cout << "Correct Code is: " << encodedRec << endl;
        cout << "Decoded code: " << decode(encodedRec) << endl;
    }

//    cout << "Check Bit 1: " << checkBit1(encodedRec).to_string() << endl;
//    cout << "Check Bit 2: " << checkBit2(encodedRec).to_string() << endl;
//    cout << "Check Bit 4: " << checkBit4(encodedRec).to_string() << endl;
//    cout << "Check Bit 8: " << checkBit8(encodedRec).to_string() << endl;
    return 0;
}

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
    // 2, 3, 6, 7, 10, 11
    // 1, 2, 5, 6, 9, 10
    int counter = encoded[index_enc(1)] + encoded[index_enc(2)] + encoded[index_enc(5)] + encoded[index_enc(6)]
                  + encoded[index_enc(9)] + encoded[index_enc(10)];
    return bitset<1>((unsigned long) (counter % 2 != 0));
}

bitset<1> checkBit4(bitset<11> encoded) {
    // 4, 5, 6, 7
    // 3, 4, 5, 6
    int counter = encoded[index_enc(3)] + encoded[index_enc(4)] + encoded[index_enc(5)] + encoded[index_enc(6)];
    return bitset<1>((unsigned long) (counter % 2 != 0));
}

bitset<1> checkBit8(bitset<11> encoded) {
    // 8 - 25
    // 7 - 14
    int counter = 0;
    for(int i = 7; i <= 14; i++) {
        counter += encoded[index_enc(i)];
    }
    return bitset<1>((unsigned long) (counter % 2 != 0));
}

bitset<7> decode(bitset<11> encoded) {
    bitset<7> decoded = bitset<7>(0);
    decoded[index_bits(0)] = encoded[index_enc(2)];
    decoded[index_bits(1)] = encoded[index_enc(4)];
    decoded[index_bits(2)] = encoded[index_enc(5)];
    decoded[index_bits(3)] = encoded[index_enc(6)];

    for(int i = 8, j = 4; j < 7; i++, j++)
        decoded[index_bits(i)] = encoded[index_enc(j)];
    return decoded;
}


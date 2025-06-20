#include<iostream>
using namespace std;

// Function to perform XOR division
string xorDivision(string encoded, const string& generator) {
    int genLen = generator.length();
    for (int i = 0; i <= encoded.length() - genLen; ) {
        if (encoded[i] == '1') {
            for (int j = 0; j < genLen; ++j) {
                // Perform bitwise XOR (as characters)
                encoded[i + j] = (encoded[i + j] == generator[j]) ? '0' : '1';
            }
        }
        // Move to the next '1'
        while (i < encoded.length() && encoded[i] != '1') ++i;
    }
    return encoded;
}

int main() {
    string data, generator;

    // Sender side
    cout << "------Sender Side------\n";
    cout << "Enter data (binary): ";
    cin >> data;
    cout << "Enter generator polynomial (binary): ";
    cin >> generator;

    int genLen = generator.length();
    string paddedData = data + string(genLen - 1, '0');

    string remainder = xorDivision(paddedData, generator);
    string checksum = remainder.substr(remainder.length() - genLen + 1);
    string transmitted = data + checksum;

    cout << "Checksum generated: " << checksum << endl;
    cout << "Transmitted message: " << transmitted << endl;

    // Receiver side
    cout << "------Receiver Side------\n";
    cout << "Enter received message: ";
    string received;
    cin >> received;

    string receiverRemainder = xorDivision(received, generator);
    string checkPart = receiverRemainder.substr(receiverRemainder.length() - genLen + 1);

    if (checkPart.find('1') != string::npos) {
        cout << "Error detected in transmission\n";
    } else {
        cout << "No error in transmission\n";
    }

    return 0;
}

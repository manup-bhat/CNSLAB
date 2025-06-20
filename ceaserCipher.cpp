#include <iostream>
#include <string>
using namespace std;

// Encrypt or decrypt based on shift direction
string caesar(string text, int shift) {
    string result = "";
    shift = (shift % 26 + 26) % 26; // Ensure shift is in [0,25]

    for (char ch : text) {
        if (isalpha(ch)) {
            char base = isupper(ch) ? 'A' : 'a';
            result += (ch - base + shift) % 26 + base;
        } else {
            result += ch; // Leave non-alphabet characters unchanged
        }
    }
    return result;
}

int main() {
    string text;
    int shift;

    cout << "Enter plain text: ";
    getline(cin, text);

    cout << "Enter shift value: ";
    cin >> shift;

    string encrypted = caesar(text, shift);
    cout << "Encrypted: " << encrypted << endl;

    string decrypted = caesar(encrypted, -shift);
    cout << "Decrypted: " << decrypted << endl;

    return 0;
}

#include <iostream>
#include <string>
//#include <cctype>
using namespace std;

string generateKey(const string &text, const string &key) {
    string newKey;
    int keyIndex = 0;
    for (char ch : text) {
        if (isalpha(ch)) {
            newKey += toupper(key[keyIndex % key.length()]);
            keyIndex++;
        }
    }
    return newKey;
}

string vigenereEncrypt(const string &text, const string &key) {
    string cipherText;
    string newKey = generateKey(text, key);
    int keyIndex = 0;
    
    for (char ch : text) {
        if (isalpha(ch)) {
            char base = isupper(ch) ? 'A' : 'a';
            cipherText += ((toupper(ch) - 'A' + (newKey[keyIndex] - 'A')) % 26) + base;
            keyIndex++;
        } else {
            cipherText += ch;
        }
    }
    return cipherText;
}

string vigenereDecrypt(const string &cipherText, const string &key) {
    string plainText;
    string newKey = generateKey(cipherText, key);
    int keyIndex = 0;
    
    for (char ch : cipherText) {
        if (isalpha(ch)) {
            char base = isupper(ch) ? 'A' : 'a';
            plainText += ((toupper(ch) - 'A' - (newKey[keyIndex] - 'A') + 26) % 26) + base;
            keyIndex++;
        } else {
            plainText += ch;
        }
    }
    return plainText;
}

int main() {
    string text, key;
    cout << "Enter the text: ";
    getline(cin, text);
    cout << "Enter the key: ";
    cin >> key;
    
    string encrypted = vigenereEncrypt(text, key);
    cout << "Encrypted Text: " << encrypted << endl;
    
    string decrypted = vigenereDecrypt(encrypted, key);
    cout << "Decrypted Text: " << decrypted << endl;
    
    return 0;
}

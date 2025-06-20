#include <iostream>
#include <vector>
#include <unordered_set>
#include <map>
#include <cctype>
using namespace std;

char matrix[5][5];
map<char, pair<int, int>> position;

// Function to build 5x5 matrix
void buildMatrix(string key) {
    unordered_set<char> used;
    key += "ABCDEFGHIKLMNOPQRSTUVWXYZ"; // 'J' is merged with 'I'
    int index = 0;

    for (char ch : key) {
        ch = toupper(ch);
        if (ch == 'J') ch = 'I';
        if (used.count(ch)) continue;
        used.insert(ch);
        matrix[index / 5][index % 5] = ch;
        position[ch] = {index / 5, index % 5};
        index++;
    }
}

// Prepare text: remove non-letters, convert to uppercase, insert 'X' between duplicates, pad end if odd
string prepareText(string text) {
    string clean;
    for (char ch : text) {
        if (isalpha(ch)) {
            ch = toupper(ch);
            clean += (ch == 'J') ? 'I' : ch;
        }
    }

    string prepared;
    for (size_t i = 0; i < clean.size(); i++) {
        prepared += clean[i];
        if (i + 1 < clean.size()) {
            if (clean[i] == clean[i + 1])
                prepared += 'X';
            else
                prepared += clean[++i];
        }
    }

    if (prepared.size() % 2 == 1)
        prepared += 'X';

    return prepared;
}

// Encrypt or Decrypt
string playfair(string text, bool encrypt) {
    string result;
    int shift = encrypt ? 1 : 4; // (4 ≡ -1 mod 5)

    for (size_t i = 0; i < text.size(); i += 2) {
        char a = text[i], b = text[i + 1];
        auto [r1, c1] = position[a];
        auto [r2, c2] = position[b];

        if (r1 == r2) { // Same row
            result += matrix[r1][(c1 + shift) % 5];
            result += matrix[r2][(c2 + shift) % 5];
        } else if (c1 == c2) { // Same column
            result += matrix[(r1 + shift) % 5][c1];
            result += matrix[(r2 + shift) % 5][c2];
        } else { // Rectangle
            result += matrix[r1][c2];
            result += matrix[r2][c1];
        }
    }
    return result;
}

int main() {
    string key, plaintext;

    cout << "Enter key: ";
    getline(cin, key);

    cout << "Enter plaintext: ";
    getline(cin, plaintext);

    buildMatrix(key);
    string prepared = prepareText(plaintext);
    string encrypted = playfair(prepared, true);
    string decrypted = playfair(encrypted, false);

    cout << "\nPrepared text: " << prepared << endl;
    cout << "Encrypted text: " << encrypted << endl;
    cout << "Decrypted text: " << decrypted << endl;

    return 0;
}

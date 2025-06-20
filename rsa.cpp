#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Brute-force modular inverse
int mod_inv(int e, int phi) {
    for (int d = 1; d < phi; d++)
        if ((e * d) % phi == 1)
            return d;
    return -1;
}

// (base^exp) % mod
int mod_exp(int base, int exp, int mod) {
    int result = 1;
    for (int i = 0; i < exp; i++)
        result = (result * base) % mod;
    return result;
}

// GCD
int gcd(int a, int b) {
    return b ? gcd(b, a % b) : a;
}

int main() {
    // Use slightly larger primes
    int p = 17, q = 11;
    int n = p * q;          // 187
    int phi = (p - 1) * (q - 1); // 160

    // Choose e such that gcd(e, phi) == 1
    int e = 7;
    while (gcd(e, phi) != 1) e++;

    // Compute modular inverse of e
    int d = mod_inv(e, phi);

    cout << "Public Key: (" << e << ", " << n << ")\n";
    cout << "Private Key: (" << d << ", " << n << ")\n";

    // Input
    string msg;
    cout << "Enter a message: ";
    getline(cin, msg);

    // Encrypt
    vector<int> encrypted;
    cout << "Encrypted values: ";
    for (char ch : msg) {
        int m = (int)ch;
        int c = mod_exp(m, e, n);
        encrypted.push_back(c);
        cout << c << " ";
    }

    // Decrypt
    cout << "\nDecrypted message: ";
    for (int c : encrypted) {
        int m = mod_exp(c, d, n);
        cout << (char)m;
    }
    cout << endl;

    return 0;
}

#include <iostream>
using namespace std;

// Modular exponentiation function: (base^exp) % mod
int mod_exp(int base, int exp, int mod) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result = (result * base) % mod;
    }
    return result;
}


int main() {
    int p, g; // Prime number and base
    cout << "Enter a prime number (p): ";
    cin >> p;
    cout << "Enter base (g): ";
    cin >> g;

    int a, b; // Secret numbers
    cout << "Enter Alice's secret number (a): ";
    cin >> a;
    cout << "Enter Bob's secret number (b): ";
    cin >> b;

    // Alice computes A = g^a % p and sends to Bob
    int A = mod_exp(g, a, p);
    cout << "Alice sends: " << A << endl;

    // Bob computes B = g^b % p and sends to Alice
    int B = mod_exp(g, b, p);
    cout << "Bob sends: " << B << endl;

    // Alice computes shared key = B^a % p
    int keyA = mod_exp(B, a, p);

    // Bob computes shared key = A^b % p
    int keyB = mod_exp(A, b, p);

    cout << "Alice's computed key: " << keyA << endl;
    cout << "Bob's computed key: " << keyB << endl;

    if (keyA == keyB) {
        cout << "Shared secret key = " << keyA << endl;
    } else {
        cout << "Error: Keys do not match!" << endl;
    }

    return 0;
}

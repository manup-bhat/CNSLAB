#include<iostream>
using namespace std;
string xorDivision(string encoded, string &generator)
{
    int genLen=generator.length();
    for(int i=0; i<=encoded.length()-genLen;){
        if(encoded[i]=='1'){
            for(int j=0;j<genLen;++j){
                encoded[i+j]=encoded[i+j]==generator[j] ? '0' : '1';
            }
        }
        while(i<encoded.length() && encoded[i] !='1')i++;
    }
    return encoded;
}

int main()
{
    string data,generator;
    cout<<"Sender side: \n";
    cout<<"Enter data:";
    cin>>data;
    cout<<"Enter generator polynomial:";
    cin>>generator;
    int genLen=generator.length();
    string encoded=data+string(genLen-1,'0');
    string remainder=xorDivision(encoded,generator);
    string checkSum=remainder.substr(remainder.length()-genLen+1);
    string transmitted=data+checkSum;
    cout<<"Transmitted message: "<<transmitted<<endl;
    cout<<"Receiver side:\n";
    cout<<"Enter received message: ";
    string received;
    cin>>received;
    string receivedRemainder=xorDivision(received,generator);
    string checkPart=receivedRemainder.substr(receivedRemainder.length()-genLen+1);
    if(checkPart.find('1')!=string::npos)
        cout<<"Error in transmission!";
    else
        cout<<"No Error in transmission!";
    return 0;

}
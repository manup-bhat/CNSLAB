#include<iostream>
#include<queue>
using namespace std;
void leakyBucket(int outputSize,int bucketSize){
    queue<int> bucket;
    int currentTime=0;
    while(true){
        int incomingPacket;
        cout << "Enter incoming packet:(0 to stop)";
        cin >> incomingPacket;
        if(incomingPacket==0)
            break;
        if(bucket.size()+incomingPacket<=bucketSize){
            for(int i=0;i<incomingPacket;i++)
                bucket.push(1);
        } else {
            cout << "Overflow!! Dropping packets"<<endl;
            while(!bucket.empty()) bucket.pop();
        }

        cout<<"------------------";
        cout << "Incoming packet :"<<incomingPacket<<endl;
        cout << "Transmission left:"<<bucket.size()<<endl;
        while(!bucket.empty()){
            for(int i=0;i<outputSize && !bucket.empty(); i++){
                bucket.pop();
            }
            cout << "Time:"<<currentTime<<"sec-Transmitted"<<outputSize<<" packets"<<endl;
            cout<<"Bytes remaining:"<<bucket.size()<<endl;
            currentTime++;
        }
    }
}

int main()
{
    int outputSize,bucketSize;
    cout << "Enter outputRate and bucketSize: ";
    cin>> outputSize;
    cin>>bucketSize;
    leakyBucket(outputSize,bucketSize);
}
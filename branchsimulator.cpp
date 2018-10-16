#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <stdio.h>
#include <map>
#include <string>
#include <sstream>

#define ST 3
#define WT 2
#define SN 0
#define WN 1

using namespace std;

int main(int argc, char* argv[])
{
    //Read parameters from config
    int m;
    int k;
    ifstream f;
    f.open("config_new.txt");
    while(!f.eof())  // read config file
    {
        f>>m;
        f>>k;
        
    }
    f.close();
    cout<<"m:\t"<<m<<endl;
    cout<<"k:\t"<<k<<endl;
    
    // Initialize the counter
    vector<long> vec;
    vec.resize(pow(2,m) , pow(2,k));
    int i;
    int j;
    for (i=0; i<pow(2,m); i++){
        for (j=0; j<pow(2,k); j++){
            vec[i,j] = WT;
        }
    }
    
    //Read/Write trace
    ifstream trace;
    ofstream traceout;
    
    string addrhex;
    unsigned int addr;
    int status;
    int bhr_index;
    int gcnt = 0;
    bitset<32> pc;
    bitset<20> index;
    int index_dec;
    long prediction;
    int miss;
    int state;
    
    miss = 0 ;
    // initialize bhr
    trace.open("trace.txt");
    traceout.open("trace.txt.out");
    
    string line;
    
    bitset<4> bhr(0000);
    int temp;
    
    if (trace.is_open()&&traceout.is_open()){
        while (getline(trace, line))
        {
            
            gcnt++;
            
            istringstream iss(line);
            if (!(iss >> addrhex >> status)) {
                cout<<"wrong format in trace"<<endl;
                break;
                
            } // error
            
            stringstream saddr(addrhex);
            saddr>>std::hex>>addr;
            
            pc = bitset<32> (addr);
            index = bitset<20> (pc.to_string().substr(32-m,m));
            index_dec = index.to_ulong();
            bhr_index = bhr.to_ulong();
            
            temp = vec[index_dec,bhr_index];
            if (temp == ST or temp == WT){
                prediction = 1;
            }
            else if (temp == SN or temp == WN){
                prediction = 0;
            }
            //cout<<"predicion:\t"<<prediction<<endl;
            
            //2-bit Saturating Counter
            if (status == 1){
                if(temp == ST or temp == WT){
                    temp = ST;
                    state = 0;
                }
                else if(temp == WN){
                    temp = ST;
                    state = 1;
                }
                else if(temp == SN){
                    temp = WN;
                    state = 1;
                }
            }
            else if (status == 0){
                if(temp == SN or temp == WN){
                    temp = SN;
                                        state = 0;
                }
                else if(temp == WT){
                   temp = SN;
                                        state = 1;
                }
                else if(temp == ST){
                    temp = WT;
                                        state = 1;
                }
            }
            
            //mispredict 
            if (state == 1){
                miss = miss+1;
            }

            //global hisory table with size k
            if (k != 0){
                for (i = k-1; i >= 0; i--){
                    bhr[i] = bhr[i-1];
                }
                bhr[0] = status;
            }else{
                bhr = 0000;
            }
            //cout<<"bhr:\t"<<bhr<<endl;
            vec[index_dec,bhr_index]= temp;
            traceout << prediction << endl;
            
        }
        trace.close();
        traceout.close();
    }
    else
        cout<<"unable to open file"<<endl;
    cout<<"total number:\t"<<gcnt<<endl;
    cout<<"miss:\t"<<miss<<endl;
    
    
}

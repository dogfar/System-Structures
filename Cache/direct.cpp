#include <bits/stdc++.h>
#define ull unsigned long long
#define V_Flag 0x1
#define D_Flag 0x2
#define TOTAL 128*1024
#define Block_Byte 3 //8 32 64-> 3 5 6
using namespace std;

unsigned char cachemeta[(TOTAL >> Block_Byte)][7];

vector<char> op;
vector<ull> v;

int ctoi(char c){
    if(c >= '0' && c <= '9')return c - '0';
    return c - 'a' + 10;
}

int readafile(const string& path){
    int cnt = 0;
    ifstream f(path);
    string tmp;
    while(getline(f, tmp)){
        cnt ++;
        ull t = 0;
        for(int i = 0; i < tmp.length(); i++){
            if(tmp[i] == 'x'){
                for(int j = i+1; j < tmp.length(); j++){
                    t =  t << 4;
                    t += ctoi(tmp[j]);
                }
                op.push_back(tmp[0]);
                v.push_back(t);
                break;
            }
        }
    }
    return cnt;
}

ull getidmod(){
    ull ans = 0;
    for(int i = 0; i < 17-Block_Byte; i++){
        ans = ans << 1;
        ans += 0x1;
    }
    return ans;
}

bool pdhit(const unsigned char cache[], ull tag){
    for(int i = 0; i < 5; i++){
        if(cache[i] != (tag & 0xff))return false;
        tag = tag >> 8;
    }
    return (cache[5] & 0x7f) == (tag & 0x7f);
}

void fillin(unsigned char cache[], ull tag){
    for(int i = 0; i < 5; i++){
        cache[i] = (tag & 0xff);
        tag = tag >> 8;
    }
    cache[5] = (tag & 0x7f);
}

int main(){
    cout << "Please choose a file from astar, bzip2, mcf and perlbench:" << endl;
    string filename;
    cin >> filename;
    string path = "./trace/"+filename+".trace";
    
    int cnt = readafile(path);
    memset(cachemeta, 0, sizeof(cachemeta));
    double hit = 0;
    double miss = 0;
    for(int i = 0; i < cnt; i++){
        int index = ((v[i] >> Block_Byte) & getidmod());
        ull tag = (v[i] >> 17);
        if((cachemeta[index][6] & V_Flag) && pdhit(cachemeta[index], tag)){
            hit += 1;
            if(op[i] == 'w')cachemeta[index][6] |= D_Flag;
        }
        else{
            miss += 1;
            memset(cachemeta[index], 0, sizeof(cachemeta[index]));
            fillin(cachemeta[index], tag);
            cachemeta[index][6] |= V_Flag;
            if(op[i] == 'w')cachemeta[index][6] |= D_Flag;
        }
    }
    double percent = hit / cnt;
    printf("Hit Ratio: %lf\n", percent);
    return 0;
}
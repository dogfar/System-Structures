#include <bits/stdc++.h>
#define ull unsigned long long
#define V_Flag 0x1
#define D_Flag 0x2
#define TOTAL 128*1024
#define Block_Byte 3 //8 32 64-> 3 5 6
using namespace std;

unsigned char cachemeta[(TOTAL >> Block_Byte)][7];
unsigned char lrustack[(TOTAL >> (Block_Byte + 2))];

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
    for(int i = 0; i < 15-Block_Byte; i++){
        ans = ans << 1;
        ans += 0x1;
    }
    return ans;
}

bool pdhit(const unsigned char cache[], ull tag){
    for(int i = 0; i < 6; i++){
        if(cache[i] != (tag & 0xff))return false;
        tag = tag >> 8;
    }
    return ((cache[6] >> 2) & 0x1) == (tag & 0x1);
}

void fillin(unsigned char cache[], ull tag){
    for(int i = 0; i < 6; i++){
        cache[i] = (tag & 0xff);
        tag = tag >> 8;
    }
    cache[6] = ((tag & 0x1) << 2);
}

void dolru(unsigned char& c, int k){
    if((c & 0x3) == k)return;
    else if(((c >> 2) & 0x3) == k){
        c = ((c & 0xf0) | ((c & 0x3) << 2) | (k & 0x3));
        return;
    }
    else if(((c >> 4) & 0x3) == k){
        c = ((c & 0xc0) | ((c & 0xf) << 2) | (k & 0x3));
        return;
    }
    else if(((c >> 6) & 0x3) == k){
        c = ((c << 2) | (k & 0x3));
        return;
    }
    else return;
}

int main(){
	cout << "Please choose a file from astar, bzip2, mcf and perlbench:" << endl;
	string filename;
	cin >> filename;
    string path = "./trace/"+filename+".trace";
    int cnt = readafile(path);
    memset(cachemeta, 0, sizeof(cachemeta));
    memset(lrustack, 0, sizeof(lrustack));
    double hit = 0;
    double miss = 0;
    for(int i = 0; i < cnt; i++){
        int lruid = ((v[i] >> Block_Byte) & getidmod());
        int index = (lruid << 2);
        ull tag = (v[i] >> 15);
        bool hit_flag = false;
        int first_blank = -1;
        for(int j = 0; j < 4; j++){
            if(cachemeta[index+j][6] & V_Flag){
                if(pdhit(cachemeta[index+j], tag)){
                    hit += 1;
                    hit_flag = true;
                    if(op[i] == 'w')cachemeta[index+j][6] |= D_Flag;
                    dolru(lrustack[lruid], j);
                    break;
                }
            }
            else{
                if(first_blank < 0)first_blank = j;
                break;
            }
        }
        if(!hit_flag){
            miss += 1;
            if(first_blank >= 0){
                fillin(cachemeta[index+first_blank], tag);
                cachemeta[index+first_blank][6] |= V_Flag;
                if(op[i] == 'w')cachemeta[index+first_blank][6] |= D_Flag;
                lrustack[lruid] = (lrustack[lruid] << 2) | (first_blank & 0x3);
            }
            else{
                int chosen = ((lrustack[lruid] >> 6) & 0x3); //
                dolru(lrustack[lruid], chosen);
                memset(cachemeta[index+chosen], 0, sizeof(cachemeta[index+chosen]));
                fillin(cachemeta[index+chosen], tag);
                cachemeta[index+chosen][6] |= V_Flag; //
                if(op[i] == 'w')cachemeta[index+chosen][6] |= D_Flag; //
            }
        }
    }
    double percent = hit / cnt;
    printf("Hit Ratio: %lf\n", percent);
    return 0;
}

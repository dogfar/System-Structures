#include <bits/stdc++.h>
#define ull unsigned long long
#define V_Flag 0x1
#define D_Flag 0x2
#define TOTAL 128*1024
#define Block_Byte 3 //8 32 64-> 3 5 6
using namespace std;

ull cachemeta[TOTAL >> Block_Byte];
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
void dolru(ull cache[], int k){
    for(int i = k-1; i >= 0; i--){
        cache[i+1] = cache[i];
    }
}

int main(){
    int num = (TOTAL >> Block_Byte);
    cout << "Please choose a file from astar, bzip2, mcf and perlbench:" << endl;
    string filename;
    cin >> filename;
    string path = "./trace/"+filename+".trace";
    int cnt = readafile(path);
    memset(cachemeta, 0, sizeof(cachemeta));
    double hit = 0;
    double miss = 0;
    for(int i = 0; i < cnt; i++){
        int first_blank = -1;
        bool hit_flag = false;
        for(int j = 0; j < num; j++){
            if(cachemeta[j] & V_Flag){
                if((cachemeta[j] >> 2) == (v[i] >> Block_Byte)){
                    hit++;
                    hit_flag = true;
                    dolru(cachemeta, j);
                    cachemeta[0] = ((v[i] >> Block_Byte) << 2) | V_Flag;
                    if(op[i] == 'w')cachemeta[0] |= D_Flag;
                    break;
                }
            }
            else{
                first_blank = j;
                break;
            }
        }
        if(!hit_flag){
            miss++;
            if(first_blank == -1)first_blank = num-1;
            dolru(cachemeta, first_blank);
            cachemeta[0] = ((v[i] >> Block_Byte) << 2) | V_Flag;
            if(op[i] == 'w')cachemeta[0] |= D_Flag;
        }
    }
    double percent = hit / cnt;
    printf("Hit Ratio: %lf\n", percent);
    return 0;
}
#include <bits/stdc++.h>
#define ull unsigned long long
#define V_Flag 0x1
#define D_Flag 0x2
#define LRU 0
#define TREE 1
#define RAND 2
#define TOTAL 128*1024
#define Block_Byte 3 // 3, 5, 6 -> 8B, 32B, 64B
using namespace std;

unsigned char cachemeta[(TOTAL >> Block_Byte)][7];
unsigned char lrustack[(TOTAL >> (Block_Byte+3))][3];
unsigned char lrutree[2048];
vector<char> op;
vector<ull> v;

int ctoi(char c){
    if(c >= '0' && c <= '9')return c - '0';
    return c - 'a' + 10;
}

ull getidmod(){
    ull ans = 0;
    for(int i = 0; i < 14-Block_Byte; i++){
        ans = ans << 1;
        ans += 0x1;
    }
    return ans;
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

bool pdhit(const unsigned char cache[], ull tag){
    for(int i = 0; i < 6; i++){
        if(cache[i] != (tag & 0xff))return false;
        tag = tag >> 8;
    }
    return ((cache[6] >> 2) & 0x3) == (tag & 0x3);
}

void fillin(unsigned char cache[], ull tag){
    for(int i = 0; i < 6; i++){
        cache[i] = (tag & 0xff);
        tag = tag >> 8;
    }
    cache[6] = ((tag & 0x3) << 2);
}

void dolru(unsigned char lru[], int k){
    if(k == (lru[0] & 0x7)) return;
    else if(k == ((lru[0] >> 3) & 0x7)){
        lru[0] = (lru[0] & 0xc0) | ((lru[0] & 0x7) << 3) | (k & 0x7);
        return;
    }
    else if(k == (((lru[1] & 0x1) << 2) | ((lru[0] >> 6) & 0x3))){
        lru[1] = (lru[1] & 0xfe) | ((lru[0] >> 5) & 0x1);
        lru[0] = (lru[0] << 3) | (k & 0x7);
        return;
    }
    else if(k == ((lru[1] >> 1) & 0x7)){
        lru[1] = (lru[1] & 0xf0) | ((lru[1] & 0x1) << 3) | ((lru[0] >> 5) & 0x7);
        lru[0] = (lru[0] << 3) | (k & 0x7);
        return;
    }
    else if(k == ((lru[1] >> 4) & 0x7)){
        lru[1] = (lru[1] & 0x80) | ((lru[1] & 0xf) << 3) | ((lru[0] >> 5) & 0x7);
        lru[0] = (lru[0] << 3) | (k & 0x7);
        return;
    }
    else if(k == (((lru[2] & 0x3) << 1) | ((lru[1] >> 7) & 0x1))){
        lru[2] = (lru[2] & 0xfc) | ((lru[1] >> 5) & 0x3);
        lru[1] = (lru[1] << 3) | ((lru[0] >> 5) & 0x7);
        lru[0] = (lru[0] << 3) | (k & 0x7);
        return;
    }
    else if(k == ((lru[2] >> 2) & 0x7)){
        lru[2] = (lru[2] & 0xe0) | ((lru[2] & 0x3) << 3) | ((lru[1] >> 5) & 0x7);
        lru[1] = (lru[1] << 3) | ((lru[0] >> 5) & 0x7);
        lru[0] = (lru[0] << 3) | (k & 0x7);
        return;
    }
    else if(k == ((lru[2] >> 5) & 0x7)){
        lru[2] = (lru[2] << 3) | ((lru[1] >> 5) & 0x7);
        lru[1] = (lru[1] << 3) | ((lru[0] >> 5) & 0x7);
        lru[0] = (lru[0] << 3) | (k & 0x7);
        return;
    }
    else return;
}
void dotree(unsigned char &c, int k){
    if(k == 0){c |= 0xb;}
    else if(k == 1){c = (c & 0xf7) | 0x3;}
    else if(k == 2){c = (c & 0xfd) | 0x11;}
    else if(k == 3){c = (c & 0xed) | 0x1;}
    else if(k == 4){c = (c & 0xfe) | 0x24;}
    else if(k == 5){c = (c & 0xde) | 0x4;}
    else if(k == 6){c = (c & 0xfa) | 0x40;}
    else if(k == 7){c = (c & 0xba);}
    else return;
}
int searchlrutree(unsigned char &c){
    if((c & 0x1) == 0){
        if((c & 0x2) == 0){
            if((c & 0x8) == 0)return 0;
            else return 1;
        }
        else{
            if((c & 0x10) == 0)return 2;
            else return 3;
        }
    }
    else{
        if((c & 0x4) == 0){
            if((c & 0x20) == 0)return 4;
            else return 5;
        }
        else{
            if((c & 0x40) == 0)return 6;
            else return 7;
        }
    }
}

int main(){
	cout << "Please choose a file from astar, bzip2, mcf and perlbench:" << endl;
	string filename;
	cin >> filename;
    string path = "./trace/"+filename+".trace";

    int policy = TREE;
    bool WriteBack = true;
    bool WriteAllocate = false;

    int cnt = readafile(path);
    ofstream outfile;
    outfile.open("./"+filename+".log");
    memset(cachemeta, 0, sizeof(cachemeta));
    memset(lrustack, 0, sizeof(lrustack));
    memset(lrutree, 0, sizeof(lrutree));
    double hit = 0;
    double miss = 0;
    for(int i = 0; i < cnt; i++){
        int lruid = ((v[i] >> Block_Byte) & getidmod());
        int index = (lruid << 3);
        ull tag = (v[i] >> 14);
        bool hit_flag = false;
        int first_blank = -1;
        for(int j = 0; j < 8; j++){
            if(cachemeta[index+j][6] & V_Flag){
                if(pdhit(cachemeta[index+j], tag)){
                    hit += 1;
                    outfile << "Hit" << endl;
                    hit_flag = true;
                    if(WriteBack && op[i] == 'w')cachemeta[index+j][6] |= D_Flag;
                    /* LRU STACK*/
                    if(policy == LRU) {
                        dolru(lrustack[lruid], j);
                    }
                        /* LRU Tree */
                    else if (policy == TREE){
                        dotree(lrutree[lruid], j);
                    }
                    /* Random: Do nothing */
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
            outfile << "Miss" << endl;
            if(!WriteAllocate && op[i] == 'w'){
                continue;
            }
            if(first_blank >= 0){
                fillin(cachemeta[index+first_blank], tag);
                cachemeta[index+first_blank][6] |= V_Flag;
                if(WriteBack && op[i] == 'w')cachemeta[index+first_blank][6] |= D_Flag;
                /* LRU Stack */
                if(policy == LRU){
                    lrustack[lruid][2] = (lrustack[lruid][2] << 3) | ((lrustack[lruid][1] >> 5) & 0x7);
                    lrustack[lruid][1] = (lrustack[lruid][1] << 3) | ((lrustack[lruid][0] >> 5) & 0x7);
                    lrustack[lruid][0] = (lrustack[lruid][0] << 3) | (first_blank & 0x7);
                }
                    /* LRU Tree */
                else if(policy == TREE) {
                    dotree(lrutree[lruid], first_blank);
                }
                /* Random: Do nothing*/
            }
            else{
                int chosen = 0;
                /* Choose one according to LRU Stack */
                if(policy == LRU){
                    chosen = ((lrustack[lruid][2] >> 5) & 0x7);
                    dolru(lrustack[lruid], chosen);
                }
                    /* Choose One according to LRU Tree */
                else if(policy == TREE){
                    chosen = searchlrutree(lrutree[lruid]);
                    dotree(lrutree[lruid], chosen);
                }
                    /* Choose One according to Random */
                else if(policy == RAND) {
                    chosen = rand() % 8;
                }
                memset(cachemeta[index+chosen], 0, sizeof(cachemeta[index+chosen]));
                fillin(cachemeta[index+chosen], tag);
                cachemeta[index+chosen][6] |= V_Flag;
                if(WriteBack && op[i] == 'w')cachemeta[index+chosen][6] |= D_Flag;
            }
        }
    }
    double percent = hit / cnt;
    printf("Hit Ratio: %lf\n", percent);
    return 0;
}
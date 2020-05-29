#include <bits/stdc++.h>
#define LD 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
using namespace std;

//1-6Ars 7-9Mrs 10-12LB
//add sub mult div [0]idofinstr [1]busyornot [2] operation [3-4]valueofsource1-2 [5-6]whichunit [7]timeremain
//ld [0]idxofstr [1]busyornot [2]address [3] timeramain

struct Instr{
    int op;
    int r1, r2, r3;
    int opnum;
};

vector<Instr> v;

int resevStation[13][8] = {0};
int regresult[32] = {0};
int regs[32] = {0};
bool finished = false;

int usingadder[3];
int usingmulter[2];
int usingloader[2];

int hextoint(string opint){
    int ans = 0;
    for(int i = 2; i < opint.length(); i++){
        ans = ans << 4;
        if(opint[i] >= 'A' && opint[i] <= 'F'){
            ans += (opint[i] - 'A'+ 10);
        }
        else{
            ans += (opint[i] - '0');
        }
    }
    return ans;
}

int getregnum(string regname){
    int ans = 0;
    for(int i = 1; i < regname.length(); i++){
        ans *= 10;
        ans += regname[i] - '0';
    }
    return ans;
}

int openfile(const string& filename){
    ifstream f(filename);
    string inputbuf;
    while(getline(f, inputbuf)){
        Instr ins;
        int comma = -1, comma2 = -1;
        if(inputbuf[0] == 'L'){
            ins.op = LD;
            for(int i = 3; i < inputbuf.length(); i++){
                if(inputbuf[i] == ','){
                    comma = i;
                    break;
                }
            }
            ins.r1 = getregnum(inputbuf.substr(3, comma-3));
            ins.opnum = hextoint(inputbuf.substr(comma+1, inputbuf.length()-comma-1));
        }
        else{
            for(int i = 4; i < inputbuf.length(); i++){
                if(inputbuf[i] == ','){
                    comma = i;
                    break;
                }
            }
            ins.r1 = getregnum(inputbuf.substr(4, comma-4));
            for(int i = comma+1; i < inputbuf.length(); i++){
                if(inputbuf[i] == ','){
                    comma2 = i;
                    break;
                }
            }
            ins.r2 = getregnum(inputbuf.substr(comma+1, comma2-comma-1));
            ins.r3 = getregnum(inputbuf.substr(comma2+1, inputbuf.length()-comma2-1));
            if(inputbuf[0] == 'A'){
                ins.op = ADD;
            }
            else if(inputbuf[0] == 'S'){
                ins.op = SUB;
            }
            else if(inputbuf[0] == 'M'){
                ins.op = MUL;
            }
            else{
                ins.op = DIV;
            }
        }
        v.push_back(ins);
    }
    return v.size();
}

void init(){
    for(int i = 0; i < 10; i++){
        resevStation[i][0] = resevStation[i][5] = resevStation[i][6] = resevStation[i][7] = -1;
    }
    for(int i = 10; i < 13; i++){
        resevStation[i][0] = resevStation[i][3] = -1;
    }
    for(int i = 0; i < 3; i++){
        usingadder[i] = -1;
    }
    for(int i = 0; i < 2; i++){
        usingloader[i] = usingmulter[i] = -1;
    }
}

void prettyprinter(int clk){
    printf("clk %d\n", clk);
    printf("Instructions Using Adder:\n");
    for(int i = 0; i < 3; i++){
        printf("%d ", usingadder[i]);
    }
    printf("\n");
    printf("Instructions Using Multer:\n");
    for(int i = 0; i < 2; i++){
        printf("%d ", usingmulter[i]);
    }
    printf("\n");
    printf("Instructions Using Loader:\n");
    for(int i = 0; i < 2; i++){
        printf("%d ", usingloader[i]);
    }
    printf("\n");

    printf("ResevStaion Table:\n");
    for(int i = 1; i < 10; i++){
        printf("%d ", i);
        for(int j = 0; j < 8; j++){
            printf("%d ", resevStation[i][j]);
        }
        printf("\n");
    }
    for(int i = 10; i < 13; i++){
        printf("%d ", i);
        for(int j = 0; j < 4; j++){
            printf("%d ", resevStation[i][j]);
        }
        printf("\n");
    }

    printf("Regresult Table:\n");
    for(int i = 0; i < 32; i++){
        printf("%d ", i);
    }
    printf("\n");
    for(int i = 0; i < 32; i++){
        printf("%d ", regresult[i]);
    }
    printf("\n");

    printf("Regs Table:\n");
    for(int i = 0; i < 32; i++){
        printf("%d ", i);
    }
    printf("\n");
    for(int i = 0; i < 32; i++){
        printf("%d ", regs[i]);
    }
    printf("\n");
}

int main(){
    string filename = "Big_test.nel";
    int cnt = openfile(filename);
    int status[cnt][3] = {0};
    int clk = 0;
    int now = 0; //下一条要issue的指令
    init();

    //clock_t start, end;
    //start = clock();
    /*todo: finished*/
    while(!finished){
        clk++;
        for(int i = 1; i < 13; i++){
            if(resevStation[i][1] == 1){
                if(i < 7){ // ADD OR SUB
                    //writeback
                    if(resevStation[i][7] == 0){
                        resevStation[i][7] = -1; //time reset
                        int result = 0;//算出结果以便写回和传递
                        if(resevStation[i][2] == ADD){
                            result = resevStation[i][3] + resevStation[i][4];
                        }
                        else{
                            result = resevStation[i][3] - resevStation[i][4];
                        }
                        //写回寄存器
                        if(regresult[v[resevStation[i][0]].r1] == i){
                            regs[v[resevStation[i][0]].r1] = result;
                            regresult[v[resevStation[i][0]].r1] = 0;
                        }
                        status[resevStation[i][0]][2] = clk;
                        resevStation[i][1] = resevStation[i][2] = resevStation[i][3] = resevStation[i][4] = 0;

                        //让出运算部件
                        int leftj = 100;
                        for (int j = 0; j < 3; j++) {
                            if (usingadder[j] == resevStation[i][0]) {
                                usingadder[j] = -1;
                                leftj = j;
                                break;
                            }
                        }
                        //结果传递给需要的人
                        for (int j = 1; j < 10; j++) {
                            if (resevStation[j][5] == i) {
                                resevStation[j][5] = -1;
                                resevStation[j][3] = result;
                            }
                            if (resevStation[j][6] == i) {
                                resevStation[j][6] = -1;
                                resevStation[j][4] = result;
                            }
                        }
                        resevStation[i][0] = -1;
                    }
                }
                else if(i < 10){ // MULT OR DIV
                    //writeback
                    if(resevStation[i][7] == 0){
                        resevStation[i][7] = -1; //time reset
                        int result = 0;//算出结果以便写回和传递
                        if(resevStation[i][2] == MUL){
                            result = resevStation[i][3] * resevStation[i][4];
                        }
                        else{
                            if(resevStation[i][4] == 0)result = resevStation[i][3];
                            else result = resevStation[i][3] / resevStation[i][4];
                        }
                        //写回寄存器
                        if(regresult[v[resevStation[i][0]].r1] == i){
                            regs[v[resevStation[i][0]].r1] = result;
                            regresult[v[resevStation[i][0]].r1] = 0;
                        }
                        status[resevStation[i][0]][2] = clk;
                        resevStation[i][1] = resevStation[i][2] = resevStation[i][3] = resevStation[i][4] = 0;

                        //让出unit 接下来还要把它给别人
                        int leftj = 100;
                        for(int j = 0; j < 2; j++){
                            if(usingmulter[j] == resevStation[i][0]){
                                usingmulter[j] = -1;
                                leftj = j;
                                break;
                            }
                        }
                        //结果传递给需要的人
                        for(int j = 1; j < 10; j++){
                            if(resevStation[j][5] == i){
                                resevStation[j][5] = -1;
                                resevStation[j][3] = result;
                            }
                            if(resevStation[j][6] == i){
                                resevStation[j][6] = -1;
                                resevStation[j][4] = result;
                            }
                        }
                        resevStation[i][0] = -1;
                    }
                }
                else{ // LOAD
                    //writeback
                    if(resevStation[i][3] == 0){
                        resevStation[i][3] = -1;
                        int result = resevStation[i][2];//算出结果以传值和写回
                        //写回寄存器
                        if(regresult[v[resevStation[i][0]].r1] == i){
                            regs[v[resevStation[i][0]].r1] = resevStation[i][2];
                            regresult[v[resevStation[i][0]].r1] = 0;
                        }
                        status[resevStation[i][0]][2] = clk;
                        resevStation[i][1] = resevStation[i][2] = 0;
                        //让出unit 接下来还要把它给别人
                        int leftj = 100;
                        for(int j = 0; j < 2; j++){
                            if(usingloader[j] == resevStation[i][0]){
                                usingloader[j] = -1;
                                leftj = j;
                                break;
                            }
                        }
                        //结果传递给需要的人
                        for(int j = 1; j < 10; j++){
                            if(resevStation[j][5] == i){
                                resevStation[j][5] = -1;
                                resevStation[j][3] = result;
                            }
                            if(resevStation[j][6] == i){
                                resevStation[j][6] = -1;
                                resevStation[j][4] = result;
                            }
                        }
                        resevStation[i][0] = -1;
                    }
                }
            }
        }

        for(int i = 1; i < 13; i++){
            if(resevStation[i][1] == 1){
                if(i < 7){
                    if(resevStation[i][7] > 0) { // time --
                        resevStation[i][7]--;
                        if (resevStation[i][7] == 0) {
                            status[resevStation[i][0]][1] = clk;
                        }
                    }
                }
                else if(i < 10){
                    if(resevStation[i][7] > 0){
                        resevStation[i][7]--;
                        if(resevStation[i][7] == 0){
                            status[resevStation[i][0]][1] = clk;
                        }
                    }
                }
                else{
                    if(resevStation[i][3] > 0){
                        resevStation[i][3]--;
                        if(resevStation[i][3] == 0){
                            status[resevStation[i][0]][1] = clk;
                        }
                    }
                }
            }
        }
        for(int i = 1; i < 10; i++){
            if(resevStation[i][1] == 1 && resevStation[i][5] < 0 && resevStation[i][6] < 0 && resevStation[i][7] < 0){
                if(resevStation[i][2] == ADD || resevStation[i][2] == SUB){
                    for(int j = 0; j < 3; j++){
                        if(usingadder[j] == -1){
                            usingadder[j] = resevStation[i][0];
                            resevStation[i][7] = 3;
                            break;
                        }
                    }
                }
                else if(resevStation[i][2] == MUL || resevStation[i][2] == DIV){
                    for(int j = 0; j < 2; j++){
                        if(usingmulter[j] == -1){
                            usingmulter[j] = resevStation[i][0];
                            if(resevStation[i][2] == DIV && resevStation[i][4] == 0){
                                resevStation[i][7] = 1;
                            }
                            else resevStation[i][7] = 4;
                            break;
                        }
                    }
                }
            }
        }
        for(int i = 10; i < 13; i++){
            if(resevStation[i][1] == 1 && resevStation[i][3] < 0){
                for(int j = 0; j < 2; j++){
                    if(usingloader[j] == -1){
                        usingloader[j] = resevStation[i][0];
                        resevStation[i][3] = 3;
                        break;
                    }
                }
            }
        }

        if(now < cnt){
            //issue nxt
            if(v[now].op == ADD || v[now].op == SUB){
                for(int i = 1; i < 7; i++){
                    if(resevStation[i][1] == 0){
                        //issue into resevstation
                        resevStation[i][0] = now;
                        resevStation[i][1] = 1;
                        resevStation[i][2] = v[now].op;
                        if(regresult[v[now].r2] == 0){
                            resevStation[i][3] = regs[v[now].r2];
                        }
                        else{
                            resevStation[i][5] = regresult[v[now].r2];
                        }
                        if(regresult[v[now].r3] == 0){
                            resevStation[i][4] = regs[v[now].r3];
                        }
                        else{
                            resevStation[i][6] = regresult[v[now].r3];
                        }
                        regresult[v[now].r1] = i;
                        status[now][0] = clk;
                        //try to transfer to fu
                        if(resevStation[i][5] < 0 && resevStation[i][6] < 0) {
                            for(int j = 0; j < 3; j++){
                                if(usingadder[j] == -1){
                                    usingadder[j] = now;
                                    resevStation[i][7] = 3;
                                    break;
                                }
                            }
                        }
                        now++;
                        break;
                    }
                }
            }
            else if(v[now].op == MUL || v[now].op == DIV){
                for(int i = 7; i < 10; i++){
                    if(resevStation[i][1] == 0){
                        //issue into resevstation
                        resevStation[i][0] = now;
                        resevStation[i][1] = 1;
                        resevStation[i][2] = v[now].op;
                        if(regresult[v[now].r2] == 0){
                            resevStation[i][3] = regs[v[now].r2];
                        }
                        else{
                            resevStation[i][5] = regresult[v[now].r2];
                        }
                        if(regresult[v[now].r3] == 0){
                            resevStation[i][4] = regs[v[now].r3];
                        }
                        else{
                            resevStation[i][6] = regresult[v[now].r3];
                        }
                        regresult[v[now].r1] = i;
                        status[now][0] = clk;
                        //try to transfer to fu
                        if(resevStation[i][5] < 0 && resevStation[i][6] < 0) {
                            for(int j = 0; j < 2; j++){
                                if(usingmulter[j] == -1){
                                    usingmulter[j] = now;
                                    if(resevStation[i][2] == DIV && resevStation[i][4] == 0){
                                        resevStation[i][7] = 1;
                                    }
                                    else {
                                        resevStation[i][7] = 4;
                                    }
                                    break;
                                }
                            }
                        }
                        now++;
                        break;
                    }
                }
            }
            else{ // Load
                for(int i = 10; i < 13; i++){
                    if(resevStation[i][1] == 0){
                        //issue into resevstation
                        resevStation[i][0] = now;
                        resevStation[i][1] = 1;
                        resevStation[i][2] = v[now].opnum;
                        regresult[v[now].r1] = i;
                        status[now][0] = clk;
                        //try to transfer to fu
                        for(int j = 0; j < 2; j++){
                            if(usingloader[j] == -1){
                                usingloader[j] = now;
                                resevStation[i][3] = 3;
                                break;
                            }
                        }
                        now++;
                        break;
                    }
                }
            }
        }

        //check if finish
        if(now == cnt){
            bool A = true;
            for(int i = 1; i < 13; i++){
                if(resevStation[i][1] == 1){
                    A = false;
                    break;
                }
            }
            if(A){
                finished = true;
            }
        }
        prettyprinter(clk);
    }
    /*
    end = clock();
    double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
    cout << "Time Usage: " << duration << "ms" << endl;*/
    ofstream outfile;
    outfile.open("./"+filename+".log");
    for(int i = 0; i < cnt; i++){
        outfile << status[i][0] << " " << status[i][1] << " " << status[i][2] << endl;
    }
    return 0;
}
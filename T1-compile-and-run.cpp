#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdlib.h>
#include <stack>
#include <bitset>
using namespace std;

#define MEM_SIZE 1048576

enum code {
    RM = 0,
    RR = 1,
    RI = 2,
    J = 3
};

enum codeCMD {
    HALT = 0,
    SYSCALL = 1,
    ADD = 2,
    ADDI = 3,
    SUB = 4,
    SUBI = 5,
    MUL = 6,
    MULI = 7,
    DIV = 8,
    DIVI = 9,
    LC = 12,
    SHL = 13,
    SHLI = 14,
    SHR = 15,
    SHRI = 16,
    AND = 17,
    ANDI = 18,
    OR = 19,
    ORI = 20,
    XOR = 21,
    XORI = 22,
    NOT = 23,
    MOV = 24,
    ADDD = 32,
    SUBD = 33,
    MULD = 34,
    DIVD = 35,
    ITOD = 36,
    DTOI = 37,
    PUSH = 38,
    POP = 39,
    CALL = 40,
    CALLI = 41,
    RET = 42,
    CMP = 43,
    CMPI = 44,
    CMPD = 45,
    JMP = 46,
    JNE = 47,
    JEQ = 48,
    JLE = 49,
    JL = 50,
    JGE = 51,
    JG = 52,
    LOAD = 64,
    STORE = 65,
    LOAD2 = 66,
    STORE2 = 67,
    LOADR = 68,
    LOADR2 = 69,
    STORER = 70,
    STORER2 = 71
};

// 0 - RM; 1 - RR; 2 - RI; 3 - J; 4 - None
int cmdFormat[72] = {2,2,1,2,1,2,1,2,1,2,4,4,2,1,2,1,2,1,2,1,2,1,2,2,1,4,4,4,4,4,4,4,1,1,1,1,1,1,2,2,1,3,3,1,2,1,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0,1,1,1,1};

int initCmdsMap(map<string, int> *cmdsMap) {
    cmdsMap->insert({"HALT", 0});
    cmdsMap->insert({"SYSCALL", 1});
    cmdsMap->insert({"ADD", 2});
    cmdsMap->insert({"ADDI", 3});
    cmdsMap->insert({"SUB", 4});
    cmdsMap->insert({"SUBI", 5});
    cmdsMap->insert({"MUL", 6});
    cmdsMap->insert({"MULI", 7});
    cmdsMap->insert({"DIV", 8});
    cmdsMap->insert({"DIVI", 9});
    cmdsMap->insert({"LC", 12});
    cmdsMap->insert({"SHL", 13});
    cmdsMap->insert({"SHLI", 14});
    cmdsMap->insert({"SHR", 15});
    cmdsMap->insert({"SHRI", 16});
    cmdsMap->insert({"AND", 17});
    cmdsMap->insert({"ANDI", 18});
    cmdsMap->insert({"OR", 19});
    cmdsMap->insert({"ORI", 20});
    cmdsMap->insert({"XOR", 21});
    cmdsMap->insert({"XORI", 22});
    cmdsMap->insert({"NOT", 23});
    cmdsMap->insert({"MOV", 24});
    cmdsMap->insert({"ADDD", 32});
    cmdsMap->insert({"SUBD", 33});
    cmdsMap->insert({"MULD", 34});
    cmdsMap->insert({"DIVD", 35});
    cmdsMap->insert({"ITOD", 36});
    cmdsMap->insert({"DTOI", 37});
    cmdsMap->insert({"PUSH", 38});
    cmdsMap->insert({"POP", 39});
    cmdsMap->insert({"CALL", 40});
    cmdsMap->insert({"CALLI", 41});
    cmdsMap->insert({"RET", 42});
    cmdsMap->insert({"CMP", 43});
    cmdsMap->insert({"CMPI", 44});
    cmdsMap->insert({"CMPD", 45});
    cmdsMap->insert({"JMP", 46});
    cmdsMap->insert({"JNE", 47});
    cmdsMap->insert({"JEQ", 48});
    cmdsMap->insert({"JLE", 49});
    cmdsMap->insert({"JL", 50});
    cmdsMap->insert({"JGE", 51});
    cmdsMap->insert({"JG", 52});
    cmdsMap->insert({"LOAD", 64});
    cmdsMap->insert({"STORE", 65});
    cmdsMap->insert({"LOAD2", 66});
    cmdsMap->insert({"STORE2", 67});
    cmdsMap->insert({"LOADR", 68});
    cmdsMap->insert({"LOADR2", 69});
    cmdsMap->insert({"STORER", 70});
    cmdsMap->insert({"STORER2", 71});
    return 0;
}

int initRegistersMap(map<string, int> *registersMap) {
    registersMap->insert({"r0", 0});
    registersMap->insert({"r1", 1});
    registersMap->insert({"r2", 2});
    registersMap->insert({"r3", 3});
    registersMap->insert({"r4", 4});
    registersMap->insert({"r5", 5});
    registersMap->insert({"r6", 6});
    registersMap->insert({"r7", 7});
    registersMap->insert({"r8", 8});
    registersMap->insert({"r9", 9});
    registersMap->insert({"r10", 10});
    registersMap->insert({"r11", 11});
    registersMap->insert({"r12", 12});
    registersMap->insert({"r13", 13});
    registersMap->insert({"r14", 14});
    registersMap->insert({"r15", 15});
    registersMap->insert({"flags", 16});
    return 0;
}

vector<vector<string>> splitLine(string line) {
    vector<vector<string>> res;
    vector<string> labels;
    vector<string> cmds;
    vector<string> args;

    int lastKeyStart = -1;
    bool foundCmdInLine = false;
    for (int i = 0; i < line.size() + 1; i++) {
        if (lastKeyStart == -1 && (line[i] == '\n' || line[i] == ';' || line[i] == '\0') ) break;

        if (lastKeyStart == -1 && line[i] != ' ' && line[i] != '\t') {
            lastKeyStart = i;
        }

        if (line[i] == ':') {
            string label = line.substr(lastKeyStart, i - lastKeyStart);
            labels.push_back(label);
            lastKeyStart = -1;
        } else if (lastKeyStart != -1 && (line[i] == ' ' || line[i] == ',' || line[i] == ';' || line[i] == '\n' || line[i] == '\0') ) {
            if (!foundCmdInLine) {
                string cmd = line.substr(lastKeyStart, i - lastKeyStart);
                for (auto & c: cmd) c = toupper(c);
                cmds.push_back(cmd);
                foundCmdInLine = true;
            } else {
                string arg = line.substr(lastKeyStart, i - lastKeyStart);
                args.push_back(arg);
            }
            lastKeyStart = -1;

            if(line[i] == ';' || line[i] == '\n') break;
        }

    }

    res.push_back(labels);
    res.push_back(cmds);
    res.push_back(args);

    return res;
}

int initLablesMap (map<string, int> *labelsMap, map<string, int> cmdsMap, map<string,int> registersMap) {
    ifstream file("input.fasm");
    string line;
    int pc = 0;
    int lineCounter = 1;
    while (getline(file, line)) {
        vector<vector<string>> splittedLine = splitLine(line); 
        
        for(int i = 0; i < splittedLine[0].size(); i++) {
            string newLabel = splittedLine[0][i];
            
            if ( (labelsMap->find(newLabel) != labelsMap->end()) || (cmdsMap.find(newLabel) != cmdsMap.end()) || (registersMap.find(newLabel) != registersMap.end())) {
                return lineCounter;
            } else {
                labelsMap->insert({newLabel, pc});
            }

        }
        
        if (splittedLine[1].size() > 0) 
            pc++;

        lineCounter++;
    }
    return 0;
}

int halt_func(int r1, int immS, int *mem, int *regs) {
    return immS;
}

int syscall_func(int r1, int immS, int *mem, int *regs) {
    if (immS == 0) {
            return 0;
    } else if (immS == 100) {
            int a;
            scanf("%d", &a);
            regs[r1] = a;
    } else if (immS == 101) {
            double b;
            scanf("%lf", &b);
            unsigned long long u = *(unsigned long long *)&b;
            regs[r1] = u & 0x00000000FFFFFFFF;
            regs[r1+1] = u >> 32;
    } else if (immS == 102) {
            printf("%d", regs[r1]);
    } else if (immS == 103) {
            long long op1l = (regs[r1] & 0x00000000FFFFFFFF) | ( ((long long) regs[r1+1]) << 32);
            double c = *(double *)&op1l;
            printf("%lg", c);
    } else if (immS == 104) {
            int d;
            d = getchar();
            regs[r1] = d;
    } else if (immS == 105) {
            putchar(regs[r1]);
    }
    return 0;
}

int add_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] += regs[r2] + immS;
    return 0;
}

int addi_func(int r1, int immS, int *mem, int *regs) {
    regs[r1] += immS;
    return 0;
}

int sub_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] -= (regs[r2] + immS);
    return 0;
}

int subi_func(int r1, int immS, int *mem, int *regs) {
    regs[r1] -= immS;
    return 0;
}

int mul_func(int r1, int r2, int immS, int *mem, int *regs) {
    long long res = ((long long) regs[r1]) * (regs[r2]);
    regs[r1] = res & 0x00000000FFFFFFFF;
    regs[r1+1] = (res >> 32) & 0x00000000FFFFFFFF;
    return 0;
}

int muli_func(int r1, int immS, int *mem, int *regs) {
    long long res = ((long long) regs[r1]) * immS;
    regs[r1] = res & 0x00000000FFFFFFFF;
    regs[r1+1] = (res >> 32) & 0x00000000FFFFFFFF;
    return 0;
}

int div_func(int r1, int r2, int immS, int *mem, int *regs) {
    long long op1 = (regs[r1] & 0x00000000FFFFFFFF) | ( ((long long) regs[r1+1]) << 32);
    long long op2 = (regs[r2] + immS);
    if(op2 == 0) halt_func(r1, 1, mem, regs);
    regs[r1] = (int) (op1/op2);
    regs[r1+1] = (int) (op1%op2);
    return 0;
}

int divi_func(int r1, int immS, int *mem, int *regs) {
    long long op1 = (regs[r1] & 0x00000000FFFFFFFF) | ( ((long long) regs[r1+1]) << 32);
    long long op2 = immS;
    if(op2 == 0) halt_func(r1, 1, mem, regs);
    regs[r1] = (int) (op1/op2);
    regs[r1+1] = (int) (op1%op2);
    return 0;
}

int lc_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] = immS;
    return 0;
}

int shl_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] = regs[r1] << regs[r2];
    return 0;
}

int shli_func(int r1, int immS, int *mem, int *regs) {
    regs[r1] = regs[r1] << immS;
    return 0;
}

int shr_func(int r1, int r2, int immS, int *mem, int *regs) {
    if(regs[r2] >= 32) {
        regs[r1] = 0;
    } else {
        regs[r1] = regs[r1] >> regs[r2];
    }
    return 0;
}

int shri_func(int r1, int immS, int *mem, int *regs) {
    if(immS >= 32) {
        regs[r1] = 0;
    } else {
        regs[r1] = regs[r1] >> immS;
    }
    return 0;
}

int and_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] &= (regs[r2] + immS);
    return 0;
}

int andi_func(int r1, int immS, int *mem, int *regs) {
    regs[r1] &= immS;
    return 0;
}

int or_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] |= (regs[r2] + immS);
    return 0;
}

int ori_func(int r1, int immS, int *mem, int *regs) {
    regs[r1] |= immS;
    return 0;
}

int xor_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] ^= (regs[r2] + immS);
    return 0;
}

int xori_func(int r1, int immS, int *mem, int *regs) {
    regs[r1] ^= immS;
    return 0;
}

int not_func(int r1, int immS, int *mem, int *regs) {
    regs[r1] = ~regs[r1]; 
    return 0;
}

int mov_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] = regs[r2] + immS;
    return 0;
}

int addd_func(int r1, int r2, int immS, int *mem, int *regs) {
    long long op1l = (regs[r1] & 0x00000000FFFFFFFF) | ( ((long long) regs[r1+1]) << 32);
    long long op2l = (regs[r2] & 0x00000000FFFFFFFF) | ( ((long long) regs[r2+1]) << 32);
    double op1 = *(double *)&op1l;
    double op2 = *(double *)&op2l;
    double res1 = op1 + op2;
    unsigned long long u = *(unsigned long long *)&res1;
    regs[r1] = u & 0x00000000FFFFFFFF;
    regs[r1+1] = u >> 32;
    return 0;
}

int subd_func(int r1, int r2, int immS, int *mem, int *regs) {
    long long op1l = (regs[r1] & 0x00000000FFFFFFFF) | ( ((long long) regs[r1+1]) << 32);
    long long op2l = (regs[r2] & 0x00000000FFFFFFFF) | ( ((long long) regs[r2+1]) << 32);
    double op1 = *(double *)&op1l;
    double op2 = *(double *)&op2l;
    double res1 = op1 - op2;
    unsigned long long u = *(unsigned long long *)&res1;
    regs[r1] = u & 0x00000000FFFFFFFF;
    regs[r1+1] = u >> 32;
    return 0;
}

int muld_func(int r1, int r2, int immS, int *mem, int *regs) {
    long long op1l = (regs[r1] & 0x00000000FFFFFFFF) | ( ((long long) regs[r1+1]) << 32);
    long long op2l = (regs[r2] & 0x00000000FFFFFFFF) | ( ((long long) regs[r2+1]) << 32);
    double op1 = *(double *)&op1l;
    double op2 = *(double *)&op2l;
    double res1 = op1 * op2;
    unsigned long long u = *(unsigned long long *)&res1;
    regs[r1] = u & 0x00000000FFFFFFFF;
    regs[r1+1] = u >> 32;
    return 0;
}

int divd_func(int r1, int r2, int immS, int *mem, int *regs) {
    long long op1l = (regs[r1] & 0x00000000FFFFFFFF) | ( ((long long) regs[r1+1]) << 32);
    long long op2l = (regs[r2] & 0x00000000FFFFFFFF) | ( ((long long) regs[r2+1]) << 32);
    double op1 = *(double *)&op1l;
    double op2 = *(double *)&op2l;
    double res1 = op1 / op2;
    unsigned long long u = *(unsigned long long *)&res1;
    regs[r1] = u & 0x00000000FFFFFFFF;
    regs[r1+1] = u >> 32;
    return 0;
}

int itod_func(int r1, int r2, int immS, int *mem, int *regs) {
    double a = (double) regs[r2];
    unsigned long long u = *(unsigned long long *)&a;
    regs[r1] = u & 0x00000000FFFFFFFF;
    regs[r1+1] = u >> 32;
    return 0;
}

int dtoi_func(int r1, int r2, int immS, int *mem, int *regs) {
    long long al = (regs[r2] & 0x00000000FFFFFFFF) | ( ((long long) regs[r2+1]) << 32);
    double a = *(double *)&al;
    if( (long long int) a > INT32_MAX || (long long int) a < INT32_MIN ) {
        halt_func(r1, 1, mem, regs);
    } else {
        regs[r1] = (int) a; 
    }
    return 0;
}

int push_func(int r1, int immS, int *mem, int *regs) {
    regs[14]--;
    mem[regs[14]] = regs[r1] + immS;
    return 0;
}

int pop_func(int r1, int immS, int *mem, int *regs) {
    regs[r1] = mem[regs[14]] + immS;
    regs[14]++;
    return 0;
}

int call_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[14]--;
    mem[regs[14]] = regs[15];
    regs[15] = regs[r2] + immS;
    regs[r1] = regs[r2] + immS;
    return 0;
}

int calli_func(unsigned int immU, int *mem, int *regs) {
    regs[14]--;
    mem[regs[14]] = regs[15];
    regs[15] = immU;
    return 0;
}

int ret_func(int r1, int immS, int *mem, int *regs) {
    regs[15] = mem[regs[14]];
    regs[14] += immS+1;
    return 0;
}

int cmp_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[16] = regs[r1] - regs[r2];
    return 0;
}

int cmpi_func(int r1, int immS, int *mem, int *regs) {
    regs[16] = regs[r1] - immS;
    return 0;
}

int cmpd_func(int r1, int r2, int immS, int *mem, int *regs) {
    long long op1l = (regs[r1] & 0x00000000FFFFFFFF) | ( ((long long) regs[r1+1]) << 32);
    long long op2l = (regs[r2] & 0x00000000FFFFFFFF) | ( ((long long) regs[r2+1]) << 32);
    double op1 = *(double *)&op1l;
    double op2 = *(double *)&op2l;
    if(op1 - op2 > 0) {
        regs[16] = 1;
    } else if(op1 - op2 < 0) {
        regs[16] = -1;
    } else {
        regs[16] = 0;
    }
    return 0;
}

int jmp_func(unsigned int immU, int *mem, int *regs) {
    regs[15] = immU;
    return 0;
}

int jne_func(unsigned int immU, int *mem, int *regs) {
    if(regs[16] != 0) 
        regs[15] = immU;
    return 0;
}

int jeq_func(unsigned int immU, int *mem, int *regs) {
    if(regs[16] == 0) 
        regs[15] = immU;
    return 0;
}

int jle_func(unsigned int immU, int *mem, int *regs) {
    if(regs[16] <= 0) {
        regs[15] = immU;
    }
    return 0;
}

int jl_func(unsigned int immU, int *mem, int *regs) {
    if(regs[16] < 0) {
        regs[15] = immU;
    }
    return 0;
}

int jge_func(unsigned int immU, int *mem, int *regs) {
    if(regs[16] >= 0) {
        regs[15] = immU;
    }
    return 0;
}

int jg_func(unsigned int immU, int *mem, int *regs) {
    if(regs[16] > 0) {
        regs[15] = immU;
    }
    return 0;
}

int load_func(int r1, unsigned int immU, int *mem, int *regs) {
    regs[r1] = mem[immU];
    return 0;
}

int store_func(int r1, unsigned int immU, int *mem, int *regs) {
    mem[immU] = regs[r1];
    return 0;
}

int load2_func(int r1, unsigned int immU, int *mem, int *regs) {
    regs[r1] = mem[immU];
    regs[r1+1] = mem[immU+1];
    return 0;
}

int store2_func(int r1, unsigned int immU, int *mem, int *regs) {
    mem[immU] = regs[r1];
    mem[immU+1] = regs[r1+1];
    return 0;
}

int loadr_func(int r1, int r2, int immS, int *mem, int *regs) {
    //r2 is an address
    regs[r1] = mem[regs[r2] + immS];
    return 0;
}

int storer_func(int r1, int r2, int immS, int *mem, int *regs) {
    mem[regs[r2] + immS] = regs[r1]; 
    return 0;
}

int loadr2_func(int r1, int r2, int immS, int *mem, int *regs) {
    regs[r1] = mem[regs[r2] + immS];
    regs[r1+1] = mem[regs[r2] + immS + 1];
    return 0;
}

int storer2_func(int r1, int r2, int immS, int *mem, int *regs) {
    mem[regs[r2] + immS] = regs[r1];
    mem[regs[r2] + immS + 1] = regs[r1+1];
    return 0;
}

int compileAndRun (map <string, int> cmdsMap, map<string, int> registersMap) {
    map<string, int> labelsMap;

    int err1 = initLablesMap(&labelsMap, cmdsMap, registersMap);
    
    if (err1 != 0) {
        printf("error in line %d\n", err1);
        return 1;
    }

    ifstream file("input.fasm");
    string line;
    int pc = 0;
    int lineCounter = 1;

    int *mem = (int *)malloc(sizeof(int) * MEM_SIZE);
    int *regs = (int *)malloc(sizeof(int) * 17);
    int *consts = (int *)malloc(sizeof(int) * 10);
    int *data = (int *)malloc(sizeof(int) * 10);
    
    for(int i = 0; i < 17; i++) regs[i] = 0;

    while (getline(file, line)) {
        vector<vector<string>> splittedLine = splitLine(line); 

        if (splittedLine[1].size() > 0) {
            string lex = splittedLine[1][0];
            if( (splittedLine[2].size() <= 0) ) {
                    printf("error in line %d\n", lineCounter);
                    return 1;
            }

            if (lex == "END") {
                if( labelsMap.find(splittedLine[2][0]) == labelsMap.end() ) {
                    printf("error in line %d\n", lineCounter);
                    return 1;
                }
                regs[15] = labelsMap[splittedLine[2][0]];
                break;
            } else if (lex == "WORD") {
                mem[pc] = (int) stoi(splittedLine[2][0]);
            } else if (lex == "DOUBLE") {
                mem[pc] = (float) stof(splittedLine[2][0]);
            } else {
                if (cmdsMap.find(lex) == cmdsMap.end()) {
                    printf("error in line %d\n", lineCounter);
                    return 1;
                }
                int cmdCode = cmdsMap[lex];
                int cmdFormatCode = cmdFormat[cmdsMap[lex]];

                unsigned int machineCmd = 0;
                if (cmdFormatCode == RM) {
                    if( (splittedLine[2].size() != 2) || (registersMap.find(splittedLine[2][0]) == registersMap.end()) ) {
                        printf("error in line %d\n", lineCounter);
                        return 1;
                    }
                    
                    string arg1str = splittedLine[2][0];
                    string arg2str = splittedLine[2][1];

                    int arg1 = registersMap[arg1str];
                    unsigned int arg2 = 0;

                    if ( ((int) arg2str.at(0)) >= 65 ) {
                        if ( labelsMap.find(arg2str) == labelsMap.end() ) {
                            printf("error in line %d\n", lineCounter);
                            return 1;
                        } else {
                            arg2 = (unsigned int) labelsMap[arg2str];
                        }
                    } else {
                        arg2 = (unsigned int) (stoul(arg2str) & 0xFFFFF);
                    }

                    machineCmd = (cmdCode << 24) | (arg1 << 20) | (arg2);
                } else if (cmdFormatCode == RR) {
                    if( (splittedLine[2].size() != 3) || (registersMap.find(splittedLine[2][0]) == registersMap.end()) || (registersMap.find(splittedLine[2][1]) == registersMap.end()) ) {
                        printf("error in line %d\n", lineCounter);
                        return 1;
                    }
                    int arg1 = (unsigned int) registersMap[splittedLine[2][0]];
                    int arg2 = (unsigned int) registersMap[splittedLine[2][1]];
                    int arg3 = (int) (stoi(splittedLine[2][2]) & 0xFFFF);
                    machineCmd = (cmdCode << 24) | (arg1 << 20) | (arg2 << 16) | (arg3);
                } else if (cmdFormatCode == RI) {
                    if( (splittedLine[2].size() != 2) || (registersMap.find(splittedLine[2][0]) == registersMap.end()) ) {
                        printf("error in line %d\n", lineCounter);
                        return 1;
                    }
                    int arg1 = (unsigned int) registersMap[splittedLine[2][0]];

                    string arg2str = splittedLine[2][1];
                    int arg2 = 0;

                    if ( ((int) arg2str.at(0)) >= 65 ) {
                        if ( labelsMap.find(arg2str) == labelsMap.end() ) {
                            printf("error in line %d\n", lineCounter);
                            return 1;
                        } else {
                            arg2 = (unsigned int) labelsMap[arg2str];
                        }
                    } else {
                        arg2 = (int) (stoi(splittedLine[2][1]) & 0xFFFFF);
                    }
                    machineCmd = (cmdCode << 24) | (arg1 << 20) | (arg2);
                } else if (cmdFormatCode == J) {
                    if( splittedLine[2].size() != 1 ) {
                        printf("error in line %d\n", lineCounter);
                        return 1;
                    }
                    string arg1str = splittedLine[2][0];
                    unsigned int arg1 = 0;

                    if ( ((int) arg1str.at(0)) >= 65 ) {
                        if ( labelsMap.find(arg1str) == labelsMap.end() ) {
                            printf("error in line %d\n", lineCounter);
                            return 1;
                        } else {
                            arg1 = (unsigned int) labelsMap[arg1str];
                        }
                    } else {
                        arg1 = (unsigned int) (stoul(arg1str) & 0xFFFFF);
                    }
                    machineCmd = (cmdCode << 24) | (arg1);
                }
                
                mem[pc] = machineCmd;

            }
            pc++;
        }
        lineCounter++;
    }

    file.close();

    //run
    
    int codeSize = pc;
    int addrStartStack = 1048575;

    regs[14] = addrStartStack;



    while(regs[15] < codeSize) {
        
        int machineCmd = mem[regs[15]];
        int cmdCode = (machineCmd >> 24);
        int cmdFormatCode = cmdFormat[cmdCode];
        int r1 = 0;
        int r2 = 0;
        int immS = 0;
        unsigned int immU = 0;

        regs[15]++;

        switch (cmdFormatCode) {
        case RM:
            r1 = (machineCmd & 0x00F00000) >> 20;
            immU = (machineCmd & 0x000FFFFF);
            break;
        case RR:
            r1 = (machineCmd & 0x00F00000) >> 20;
            r2 = (machineCmd & 0x000F0000) >> 16;
            immS = (machineCmd & 0x0000FFFF);
            break;
        case RI:
            r1 = (machineCmd & 0x00F00000) >> 20;
            immS = (machineCmd & 0x000FFFFF);
            break;
        case J:
            immU = (machineCmd & 0x000FFFFF);
            break; 
        }

        switch (cmdCode) {
        case HALT:
            halt_func(r1, immS, mem, regs);
            goto exit;
            break;
        case SYSCALL:
            syscall_func(r1, immS, mem, regs);
            if(immS == 0) goto exit;
            break;
        case ADD:
            add_func(r1, r2, immS, mem, regs);
            break;
        case ADDI:
            addi_func(r1, immS, mem, regs);
            break;
        case SUB:
            sub_func(r1, r2, immS, mem, regs);
            break;
        case SUBI:
            subi_func(r1, immS, mem, regs);
            break;
        case MUL:
            mul_func(r1, r2, immS, mem, regs);
            break;
        case MULI:
            muli_func(r1, immS, mem, regs);
            break;
        case DIV:
            div_func(r1, r2, immS, mem, regs);
            break;
        case DIVI:
            divi_func(r1, immS, mem, regs);
            break;
        case LC:
            lc_func(r1, r2, immS, mem, regs);
            break;
        case SHL:
            shl_func(r1, r2, immS, mem, regs);
            break;
        case SHLI:
            shli_func(r1, immS, mem, regs);
            break;
        case SHR:
            shr_func(r1, r2, immS, mem, regs);
            break;
        case SHRI:
            shri_func(r1, immS, mem, regs);
            break;
        case AND:
            and_func(r1, r2, immS, mem, regs);
            break;
        case ANDI:
            andi_func(r1, immS, mem, regs);
            break;
        case OR:
            or_func(r1, r2, immS, mem, regs);
            break;
        case ORI:
            ori_func(r1, immS, mem, regs);
            break;
        case XOR:
            xor_func(r1, r2, immS, mem, regs);
            break;
        case XORI:
            xori_func(r1, immS, mem, regs);
            break;
        case NOT:
            not_func(r1, immS, mem, regs);
            break;
        case MOV:
            mov_func(r1, r2, immS, mem, regs);
            break;
        case ADDD:
            addd_func(r1, r2, immS, mem, regs);
            break;
        case SUBD:
            subd_func(r1, r2, immS, mem, regs);
            break;
        case MULD:
            muld_func(r1, r2, immS, mem, regs);
            break;
        case DIVD:
            divd_func(r1, r2, immS, mem, regs);
            break;
        case ITOD:
            itod_func(r1, r2, immS, mem, regs);
            break;
        case DTOI:
            dtoi_func(r1, r2, immS, mem, regs);
            break;
        case PUSH:
            push_func(r1, immS, mem, regs);
            break;
        case POP:
            pop_func(r1, immS, mem ,regs);
            break;
        case CALL:
            call_func(r1, r2, immS, mem, regs);
            break;
        case CALLI:
            calli_func(immU, mem, regs);
            break;
        case RET:
            ret_func(r1, immU, mem, regs);
            break;
        case CMP:
            cmp_func(r1, r2, immS, mem, regs);
            break;
        case CMPI:
            cmpi_func(r1, immS, mem, regs);
            break;
        case CMPD:
            cmpd_func(r1, r2, immS, mem, regs);
            break;
        case JMP:
            jmp_func(immU, mem, regs);
            break;
        case JNE:
            jne_func(immU, mem, regs);
            break;
        case JEQ:
            jeq_func(immU, mem, regs);
            break;
        case JLE:
            jle_func(immU, mem, regs);
            break;
        case JL:
            jl_func(immU, mem, regs);
            break;
        case JGE:
            jge_func(immU, mem, regs);
            break;
        case JG:
            jg_func(immU, mem, regs);
            break;
        case LOAD:
            load_func(r1, immU, mem, regs);
            break;
        case STORE:
            store_func(r1, immU, mem, regs);
            break;
        case LOAD2:
            load2_func(r1, immU, mem ,regs);
            break;
        case STORE2:
            store2_func(r1, immU, mem, regs);
            break;
        case LOADR:
            loadr_func(r1, r2, immS, mem, regs);
            break;
        case STORER:
            storer_func(r1, r2, immS, mem, regs);
            break;
        case LOADR2:
            loadr2_func(r1, r2, immS, mem, regs);
            break;
        case STORER2:
            storer2_func(r1, r2, immS, mem, regs);
            break;
        }

    }
    exit:    

    free(mem);
    free(consts);
    free(data);
    free(regs);

    return 0;
}

int main() {
    map<string, int> cmdsMap;
    map<string, int> registersMap;
    initCmdsMap(&cmdsMap);
    initRegistersMap(&registersMap);

    compileAndRun(cmdsMap, registersMap);

    
    return 0;
}
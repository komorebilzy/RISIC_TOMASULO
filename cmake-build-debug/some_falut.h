#ifndef RISIC_V_TOMASULO_H
#define RISIC_V_TOMASULO_H

#include "parser.h"

int sum = 0;

bool flag = false;
enum state {
    waiting = 1,
    finished = 2,
    empty = 3,
    counting = 4,
};

enum counter {
    stronglyNotToken = 0,
    weaklyNotToken = 1,
    weaklyToken = 2,
    stronglyToken = 3,
};

class Predict {
private:
    const unsigned int init = 337;
    struct branch_target_buffer {
        int sc = weaklyNotToken;
    } BTB[4000];

    int getHashed(const unsigned &pc) { return (1ll * pc * init) % 4000; }

public:
    void changeState(bool result, const unsigned &pc) {
        int id = getHashed(pc);
        if (result && BTB[id].sc != stronglyToken) BTB[id].sc++;
        else if (!result && BTB[id].sc != stronglyNotToken) BTB[id].sc--;
    }

    bool jump(const unsigned &pc) {
        int id = getHashed(pc);
        if (BTB[id].sc == weaklyToken || BTB[id].sc == stronglyToken) return true;
        else return false;
    }
} predicter;

class RegisterStatus {
public:
    bool busy = false;
    int reorder;   //与ROB中entry相对应
} Status[32];

class CommonDataBase {
public:
    int entry;
    unsigned int result, pc;
} CDB;

class ReorderBuffer {
public:
    int entry; //指令编号
    bool ready;
    unsigned int instruction, des, value, address;  //des是寄存器编号 ,address是对应内存地址
    int type;
    unsigned int pc,pc_init;  //pc对应地址
};


class ReorderBuffers {
private:
    static const int len = 32;
    int siz = 0, topper = 0;
public:
    ReorderBuffer rob[len]{};

    void pop() {
        siz -= 1;
        topper = (topper + 1) % len;
    }

    ReorderBuffer top() {
        return rob[topper];
    }

    bool available() const {
        return siz != len;
    }

    int getEmpty() const {
        return (siz + topper) % len;
    }

    bool empty() const {
        return siz == 0;
    }

    void reset() {
        for (auto &i: rob) i.ready = false;
        siz = 0;
        topper = 0;
    }

    void insert(unsigned int order, unsigned int pc_now) {
        int rear = (topper + siz) % len;
        siz++;
        rob[rear].entry = rear;
        rob[rear].instruction = order;
        rob[rear].type = getType(order);
        rob[rear].pc = pc_now;
        if(rob[rear].type==B) rob[rear].pc_init=pc_now;
        rob[rear].ready = false;
        if (rob[rear].type != S && rob[rear].type != B) {
            rob[rear].des = getRd(order);
            if (rob[rear].des) {
                Status[rob[rear].des].reorder = rob[rear].entry;
                Status[rob[rear].des].busy = true;
            }
        }
    }

    void broadcast() {
        int rear = CDB.entry;
        rob[rear].ready = true;
        rob[rear].value = CDB.result;
        rob[rear].pc = CDB.pc;
//        unsigned int opcode= getOpcode(rob[rear].instruction);
//        if(rob[rear].type==B || opcode==jal || opcode==jalr)
    }
} ROB;


class ReservationStations {
public:
    static const int len = 32;
    struct ReservationStation {
        bool busy;
        int type, state;
        unsigned int op;
        unsigned int Vj, Vk;
        int Qj = -1, Qk = -1;
        unsigned int pc;
        int entry;
        unsigned int result;
    } rs[len];

    bool available() {
        for (auto &r: rs) {
            if (!r.busy) return true;
        }
        return false;
    }

    int getEmpty() {
        for (int i = 0; i < len; ++i) {
            if (!rs[i].busy) return i;
        }
    }

    void reset() {
        for (auto &r: rs) {
            r.busy = false;
        }
    }

    void insert(int entry, unsigned int order, unsigned int pc) {
        int rear = getEmpty();
        rs[rear].entry = entry;
        rs[rear].type = getType(order);
        rs[rear].busy = true;
        rs[rear].state = waiting;
        rs[rear].pc = pc;
        rs[rear].op = order;
        rs[rear].Qj = rs[rear].Qk = -1;
        if (rs[rear].type == R || rs[rear].type == I || rs[rear].type == B) {
            unsigned int rs1 = getRs1(order);
            if (Status[rs1].busy) {
                rs[rear].Qj = Status[rs1].reorder;
            } else rs[rear].Vj = reg[rs1];
        }
        if (rs[rear].type == R || rs[rear].type == B) {
            unsigned int rs2 = getRs2(order);
            if (Status[rs2].busy) {
                rs[rear].Qk = Status[rs2].reorder;
            } else rs[rear].Vk = reg[rs2];
        }
    }

    void execute() {
        int i;
        for (i = 0; i < len; ++i) { if (rs[i].state == waiting && rs[i].Qj == -1 && rs[i].Qk == -1) break; }
        if (i == len) return;
        rs[i].state = finished;
        if (rs[i].type == B) FormatB(rs[i].op, rs[i].Vj, rs[i].Vk, rs[i].pc);
        else if (rs[i].type == U) FormatU(rs[i].op, rs[i].result, rs[i].pc);
        else if (rs[i].type == J) FormatJ(rs[i].op, rs[i].pc, rs[i].result);
        else if (rs[i].type == R) FormatR(rs[i].op, rs[i].Vj, rs[i].Vk, rs[i].result);
        else if (rs[i].type == I) FormatI(rs[i].op, rs[i].Vj, rs[i].result, rs[i].pc);
    }

    bool write() {
        for (auto &r: rs) {
            if (r.state == finished) {
                r.state = empty;
                r.busy = false;
                CDB.entry = r.entry;
                if (r.type != B) CDB.result = r.result;
                CDB.pc = r.pc;
                return true;
            }
        }
        return false;
    }

    void broadcast() {
        for (auto &r: rs) {
            if (r.state == waiting) {  //RS通过匹配entry更新
                if (r.Qj == CDB.entry) r.Qj = -1, r.Vj = CDB.result;
                else if (r.Qk == CDB.entry) r.Qk = -1, r.Vk = CDB.result;
            }
        }
    }
} RS;

class LoadBuffers {
public:
    static const int len = 32;
    int topper = 0;

    struct LoadBuffer {
        bool busy;
        int type, state;
        unsigned int Vj, Vk, op;
        int Qj = -1, Qk = -1;
        int entry;
        unsigned int address, result;
        int clock = 2;
    } ls[len];

    bool available() {
        for (int i = topper;; i = (i + 1) % len) {
            if (!ls[i].busy) return true;
            if (i == (topper + len - 1) % len) break;
        }
        return false;
    }

    void pop() {
        topper = (topper + 1) % len;
    }

    int getEmpty() {
        for (int i = topper;; i = (i + 1) % len) {
            if (!ls[i].busy) return i;
            if (i == (topper + len - 1) % len) break;
        }
    }

    void reset() {
        for (int i = topper;; i = (i + 1) % len) {
            ls[i].busy = false;
            if (i == (topper + len - 1) % len) break;
        }
    }

    void insert(int entry, unsigned int order) {
        int rear = getEmpty();
        ls[rear].busy = true;
        ls[rear].entry = entry;
        ls[rear].type = getType(order);
        ls[rear].op = order;
        ls[rear].state = counting;
        ls[rear].clock = 2;
        ls[rear].Qj = ls[rear].Qk = -1;
        unsigned int rs1 = getRs1(order);
        if (Status[rs1].busy) {
            ls[rear].Qj = Status[rs1].reorder;
        } else ls[rear].Vj = reg[rs1];
        if (ls[rear].type == S) {
            unsigned int rs2 = getRs2(order);
            if (Status[rs2].busy) {
                ls[rear].Qk = Status[rs2].reorder;
            } else ls[rear].Vk = reg[rs2];
        }
    }

    void execute() {
        int i = topper;
        //检查load操作前面的ROB中是否有与当前store相同的des
        if (ls[i].state == counting && ls[i].busy && ls[i].Qk == -1 && ls[i].Qj == -1) {
            if (ls[i].clock == 0) {
                ls[i].state = finished;
                if (ls[i].type == S) {
                    FormatS(ls[i].op, ls[i].Vj, ls[i].Vk, ls[i].address, ls[i].result);
                    ROB.rob[ls[i].entry].value = ls[i].result;
                } else if (ls[i].type == I) getAddress(ls[i].op, ls[i].Vj, ls[i].address);
                ROB.rob[ls[i].entry].address = ls[i].address;
                ROB.rob[ls[i].entry].ready = true;
                ls[i].state = counting;
                ls[i].busy = false;
                pop();
            } else ls[i].clock--;
        }
    }


    void broadcast() {
        for (int i = topper;; i = (i + 1) % len) {
            if (ls[i].state == counting && ls[i].busy) {
                if (ls[i].Qj == CDB.entry) ls[i].Qj = -1, ls[i].Vj = CDB.result;
                else if (ls[i].Qk == CDB.entry) ls[i].Qk = -1, ls[i].Vk = CDB.result;
            }
            if (i == (topper + len - 1) % len) break;
        }
    }

} LS;

void reset() {
    PC = 0;
    for (auto &Statu: Status) Statu.busy = false;
    ROB.reset();
    RS.reset();
    LS.reset();
}

void issue() {

//    std::cout << "PC=" << PC << std::endl;
    if (flag) {  //阻断发射
        if (!ROB.empty()) return;
        else flag = false;
    }
    unsigned int order = 0;
    int get[] = {0, 8, 16, 24};
    for (int i = 0; i < 4; ++i) {
        order += mem[PC + i] << get[i];
    }
    unsigned int opcode = getOpcode(order);
    if (opcode == load || opcode == store) {
        if (!(LS.available() && ROB.available())) return;
        LS.insert(ROB.getEmpty(), order);
        ROB.insert(order, PC);
    } else {
        if (!(RS.available() && ROB.available())) return;
        if (opcode == jalr) flag = true;
        RS.insert(ROB.getEmpty(), order, PC);
        ROB.insert(order, PC);
    }
    if (getType(order) == J) PC += getImm(order);
    else if(getType(order)==B) {
        if (predicter.jump(PC)) PC += getImm(order) ;
        else PC += 4;
    }
    else if (!flag) PC += 4;
//    ++sum;
//    std::c
//    out << order << std::endl;
    //跳转指令需要stall 直到ROB为空（所有commit操作都已经完成） 才能继续进行
}

void execute() {
    LS.execute();
    RS.execute();
}

void writeBack() {
    if (!RS.write()) {
        return;
    }
    RS.broadcast();
    LS.broadcast();
    ROB.broadcast();
}

void commit() {
    ReorderBuffer order = ROB.top();
    if (!order.ready) return;
    ROB.pop();
    ++sum;
    std::cout << order.instruction << " ";
    if (order.instruction == 0x0ff00513u) {
//        std::cout << "reg: ";
//        for (int i = 0; i <= 31; ++i) std::cout << reg[i] << " ";
//        std::cout << std::endl;
        std::cout << (reg[10] & 255u) << std::endl;
        exit(0);
    }
    if (order.type == R || order.type == U) {
        reg[order.des] = order.value;
        //for debug
        if (getOpcode(order.instruction) == R) puts("R");
        else puts("U");
    } else if (order.type == I) {
        if (getOpcode(order.instruction) == jalr) {
            PC = order.pc;
            //
            puts("jalr");
        } else if (getOpcode(order.instruction) == load) {
            Load(order.instruction, order.address, order.value);
            puts("load");
        } else puts("I");
        reg[order.des] = order.value; //包括load和普通短立即数操作
//        if (Status[order.des].reorder == order.entry) Status[order.des].busy = false;
    } else if (order.type == S) {
        Store(order.instruction, order.address, order.value);
        puts("S");
    } else if (order.type == B) {
        unsigned int pc_predict;
        if (predicter.jump(order.pc_init))
            pc_predict = order.pc_init + getImm(order.instruction);
        else pc_predict = order.pc_init + 4;
        if (order.pc != pc_predict) {
            reset();
            PC = order.pc;
            predicter.changeState(false,order.pc_init);
        } else predicter.changeState(true,order.pc_init);
        puts("B");
    } else if (order.type == J) {
        reg[order.des] = order.value;
//        if (Status[order.des].reorder == order.entry) Status[order.des].busy = false;
        puts("j");
    }
    if (order.type != S && order.type != B) {
        if (Status[order.des].reorder == order.entry) {
            Status[order.des].busy = false;
        }
        for (auto &r: RS.rs) {
            if (r.state == waiting) {  //RS通过匹配entry更新
                if (r.Qj == order.entry) r.Qj = -1, r.Vj = reg[order.des];
                else if (r.Qk == order.entry) r.Qk = -1, r.Vk = reg[order.des];
            }
        }
        for (auto &r: LS.ls) {
            if (r.state == counting && r.busy) {  //RS通过匹配entry更新
                if (r.Qj == order.entry) r.Qj = -1, r.Vj = reg[order.des];
                else if (r.Qk == order.entry) r.Qk = -1, r.Vk = reg[order.des];
            }
        }
    }
    reg[0] = 0;
//    std::cout<<"reg: ";
//    for(int i=0;i<=31;++i) std::cout<<reg[i]<<" ";
//    std::cout<<std::endl;
}


#endif //RISIC_V_TOMASULO_H
#ifndef RISIC_V_PARSER_H
#define RISIC_V_PARSER_H

#include <string>
#include <iostream>


unsigned int mem[1000000], PC = 0, reg[32];

unsigned int get_num(const unsigned int &order, const int &l, const int &r) {
    return (order & ((1ll << (l + 1)) - 1)) >> r;
}

enum Type {
    R = 1, I = 2, S = 3, B = 4, U = 5, J = 6,
};

enum operatorType {
    lui = 0b0110111,
    auipc = 0b0010111,
    jal = 0b1101111,

    jalr = 0b1100111,

    beq = 0b000,
    bne = 0b001,
    blt = 0b100,
    bge = 0b101,
    bltu = 0b110,
    bgeu = 0b111,

    lb = 0b000,
    lh = 0b001,
    lw = 0b010,
    lbu = 0b100,
    lhu = 0b101,

    sb = 0b000,
    sh = 0b001,
    sw = 0b010,

    addi = 0b000,
    slti = 0b010,
    sltiu = 0b011,
    xori = 0b100,
    ori = 0b110,
    andi = 0b111,
    slli = 0b001,
    srli = 0b101,
    srai = 0b101 + (0b100 << 3),
    add = 0b000,
    sub = 0b000 + (0b100 << 3),
    sll = 0b001,
    slt = 0b010,
    sltu = 0b011,
    Xor = 0b100,
    srl = 0b101,
    sra = 0b101 + (0b100 << 3),
    Or = 0b110,
    And = 0b111,

    load = 0b0000011,
    store = 0b0100011


};

int getType(const unsigned int &order) {
    int type;
    unsigned int opcode = get_num(order, 6, 0);
    if (opcode == lui || opcode == auipc) type = U;
    else if (opcode == jal) type = J;
    else if (opcode == 0b1100011) type = B;
    else if (opcode == load || opcode == 0b0010011 || opcode == jalr) type = I;
    else if (opcode == 0b0100011) type = S;
    else if (opcode == 0b0110011) type = R;
    else type = -1;
    return type;
}

unsigned int getRd(const unsigned int &order) { return get_num(order, 11, 7); }

unsigned int getFunct3(const unsigned int &order) { return get_num(order, 14, 12); }

unsigned int getFunct7(const unsigned int &order) { return get_num(order, 31, 25); }

unsigned int getRs1(const unsigned int &order) { return get_num(order, 19, 15); }

unsigned int getRs2(const unsigned int &order) { return get_num(order, 24, 20); }

unsigned int getShamt(const unsigned int &order) { return get_num(order, 24, 20); }

unsigned int getImm(const unsigned int &order) {
    int type = getType(order);
    unsigned int imm;
    if (type == I ) {
        imm=get_num(order, 31, 20);
        if (imm >> 11) {
            for (int i = 31; i >= 12; --i) {
                imm += (1 << i);
            }
        }
        return imm;
    } else if (type == S) {
        return (get_num(order, 31, 25) << 5) + get_num(order, 11, 7);
    } else if (type == B) {
        imm= (get_num(order, 11, 8) << 1) + (get_num(order, 30, 25) << 5) + (get_num(order, 7, 7) << 11) +
               (get_num(order, 31, 31) << 12);
        if (imm >> 12) {for (int i = 31; i >= 13; --i) imm += (1 << i);}
        return imm;
    } else if (type == U) { return get_num(order, 31, 12) << 12; }
    else if (type == J) {
        imm= (get_num(order, 30, 21) << 1) + (get_num(order, 20, 20) << 11) + (get_num(order, 19, 12) << 12) +
               (get_num(order, 31, 31) << 20);
        if (imm >> 19) for (int i = 31; i >= 20; --i) {imm += (1 >> i);}
        return imm;
    }
}


unsigned int getOpcode(const unsigned int &order) { return get_num(order, 6, 0); }

void FormatR(unsigned int order, unsigned int rs1,unsigned int rs2, unsigned int &result) {
    unsigned int func = getFunct3(order)+ getFunct7(order);
    if (func == add) result = rs1 + rs2;
    else if (func == sub) result = rs1 - rs2;
    else if (func == sll) result = rs1 << (rs2 & 0b11111);
    else if (func == slt) result = (int) rs1 < (int) rs2 ? 1 : 0;
    else if (func == sltu) result = rs1 < rs2 ? 1 : 0;
    else if (func == Xor) result = rs1 ^ rs2;
    else if (func == srl) result = rs1 >> (rs2 & 0b11111);
    else if (func == sra) result = (int) rs1 >> (rs2 & 0b11111);
    else if (func == Or) result = rs1 | rs2;
    else if (func == And) result = rs1 & rs2;
}

void FormatI(unsigned int order,unsigned int rs1,unsigned int &result, unsigned int &pc) {
    unsigned int imm= getImm(order);  //已经符号位拓展的imm
    unsigned int func = getFunct3(order);
    unsigned int opcode = getOpcode(order);
    if (opcode == jalr) {
        unsigned int t = pc + 4;
        pc = (rs1 + imm) & ~1;
        result = t;
    } else {
        unsigned int shamt;
        if (func == addi) {
            result = rs1 + imm;
        } else if (func == slti) {
            result = (int) rs1 < (int) imm ? 1 : 0;
        } else if (func == sltiu) {
            result = rs1 < imm ? 1 : 0;
        } else if (func == xori) {
            result = rs1 ^ imm;
        } else if (func == ori) {
            result = rs1 | imm;
        } else if (func == andi) {
            result = rs1 & imm;
        } else if (func == slli) {
            shamt = getShamt(order);
            result = rs1 << shamt;
        } else if (func == srli) {
            shamt = getShamt(order);
            result = rs1 >> shamt;
        } else {
            if (func + get_num(order,31,25) == srai) {
                shamt = getShamt(order);
                result = (int) rs1 >> shamt;
            }
        }
    }
}

void FormatB(unsigned int order,unsigned int rs1,unsigned int rs2,unsigned int &pc) {
    unsigned int imm= getImm(order);
    unsigned int func = getFunct3(order);
    if (func == beq) {
        if (rs1 == rs2) pc += imm;
    } else if (func == bne) {
        if (rs1 != rs2) pc += imm;
    } else if (func == blt) {
        if ((int) rs1 < (int) rs2) pc += imm;
    } else if (func == bge) {
        if ((int) rs1 >=(int)rs2) pc+=imm;
    } else if (func == bltu) {
        if(rs1<rs2) pc+=imm;
    } else if (func == bgeu) {
        if(rs1>=rs2) pc+=imm;
    }

}

void FormatU(unsigned int order, unsigned int &result, unsigned int pc) {
    unsigned int imm= getImm(order);
    unsigned int opcode = getOpcode(order);
    if (opcode == lui) {
        result = imm;
    } else if (opcode == auipc) {
        result =  imm + pc;
    }
}

void FormatJ(unsigned int order, unsigned int &pc, unsigned int &result = reg[1]) {
    unsigned int imm= getImm(order);
    unsigned int opcode = getOpcode(order);
    if (opcode == jal) {
        result = pc + 4;
        pc += imm;
    }
}

void FormatS(unsigned int order,unsigned int rs1,unsigned int rs2, unsigned int &address,unsigned int &result) {
    unsigned int imm= getImm(order);
    if (imm >> 6) {
        for (int i = 31; i >= 7; --i) {
            imm += (1 << i);
        }
    }
    address=rs1+imm;
    result = rs2;
}


void Store(unsigned int order, unsigned int address,unsigned int result){
    unsigned int func = getFunct3(order);
    if (func == sb) {
        mem[address] = result & 255u;
    } else if (func == sh) {
        mem[address + 1] = (result >> 8) & 255u;
        mem[address] =result & 255u;
    } else if (func == sw) {
        mem[address] = result & 255u;
        mem[address + 1] = (result >> 8) & 255u;
        mem[address + 2] = (result >> 16) & 255u;
        mem[address + 3] = (result >> 24) & 255u;
    }
}

void getAddress(unsigned int order,unsigned int rs1, unsigned int &address){
    address=reg[rs1]+getImm(order);
}

void Load(unsigned int order,unsigned int address, unsigned int &result) {
    unsigned int func = getFunct3(order);
    if (func == lb) {
        unsigned int tmp = mem[address];
        if (tmp >> 7) {
            for (int i = 31; i >= 8; --i) tmp += (1 << i);
        }
        result = tmp;
    } else if (func == lh) {
        unsigned int tmp = (mem[address] << 4) + mem[address + 1];
        if (tmp >> 15) {
            for (int i = 31; i >= 16; --i) tmp += (1 << i);
        }
        result = tmp;
    } else if (func == lw) {
        unsigned int tmp = (mem[address] << 12) + (mem[address + 1] << 8) + (mem[address + 2] << 4) + mem[address + 3];
        result = tmp;
    } else if (func == lbu) {
        unsigned int tmp = mem[address];
        if (tmp >> 7) {
            for (int i = 31; i >= 8; --i) tmp += (0 << i);
        }
        result = tmp;
    } else if (func == lhu) {
        unsigned int tmp = (mem[address] << 4) + mem[address + 1];
        if (tmp >> 15) {
            for (int i = 31; i >= 16; --i) tmp += (1 << i);
        }
        result = tmp;
    }
}

unsigned int des_to_int(std::string num) { //地址
    int des = 0;
    int get[] = {28, 24, 20, 16, 12, 8, 4, 0};
    for (int i = 0; i < 8; ++i) {
        if ('0' <= num[i] <= '9')des += ((num[i]-'0') * (1 << get[i]));
        else des += ((num[i] - 'A') * (1 << get[i]));
    }
    return des;
}

void set_memory(char first, unsigned int des) {  //读入内存 改变mem[]
    int get[] = {4, 0, 4, 0, 4, 0, 4, 0};
    int num;
    for (int i = 0; i < 8; ++i) {
        if ('0' <= first <= '9') num = first - '0';
        else num = first - 'A';
        mem[des + i / 2] += (num * (1 << get[i]));
        if (i <= 6) std::cin >> first;
    }
}

void order_input() {   //输入指令
    std::string num;
    char fir;
    unsigned int des;
    while (std::cin >> fir) {
        if (fir == '@') {
            std::cin >> num;
            des = des_to_int(num);
        } else {
            set_memory(fir, des);
            des += 4;
        }
    }
}


#endif //RISIC_V_PARSER_H

#include "machine.h"

/**
 * Performs one execution cylce on the machine
 * @param {struct chip8_machine*} m The machine to progress step execution
 * @return {int} The outcome of the execution
 */
int chip8_step(struct chip8_machine *m) {
    if (m->delay_timer > 0) {
        m->delay_timer --;
    }

    if (m->sound_timer > 0) {
        m->sound_timer --;
    }

    // pull the next opcode
    uint16_t opcode = m->memory[m->pc] << 8 | m->memory[m->pc + 1];

    return chip8_execute(m, opcode);
}

/**
 * Executes the given opcode on the machine
 * @param {struct chip8_machine*} m The machine to execute the opcode on
 * @param {uint16_t} opcode The opcode to execute
 * @return {int} The outcome of the execution
 */
int chip8_execute(struct chip8_machine *m, uint16_t opcode) {
    if (m == NULL) {
        return -1;
    }

    // isolate the nibbles
    uint8_t n    = (opcode>>0) & 0xF;
    uint8_t y    = (opcode>>4) & 0xF;
    uint8_t x    = (opcode>>8) & 0xF;
    uint8_t nn   = (opcode>>0) & 0xFF;
    uint16_t nnn = (opcode>>0) & 0xFFF;
    uint8_t u    = (opcode>>12) & 0xF;

    switch (u) {
        case 0x0:
            switch (nn) {
                case 0xE0: // clear the display (cls)
                    memset(m->display, 0, sizeof(m->display));
                    m->pc += 2;
                    break;
                case 0xEE: // return from subroutine (ret)
                    m->pc = m->stack[m->sp];
                    m->sp--;
                    m->pc += 2;
                    break;
                default:
                    // call RCA 1802 program at address nnn
                    // not implemented
                    return -2;
            }
            break;

        case 0x1: // jump to address nnn (jp N)
            m->pc = nnn;
            break;

        case 0x2: // call subroutine at address nnn (call N))
            m->sp++;
            m->stack[m->sp] = m->pc;
            m->pc = nnn;
            break;

        case 0x3: // skip next instruction if Vx == nn (se Vx, byte)
            if (m->v[x] == nn) {
                m->pc += 4;
            } else {
                m->pc += 2;
            }
            break;

        case 0x4: // skip next instruction if Vx != nn (sne Vx, byte)
            if (m->v[x] != nn) {
                m->pc += 4;
            } else {
                m->pc += 2;
            }
            break;

        case 0x5: // skip next instruction if Vx == Vy (se Vx, Vy)
            if (m->v[x] == m->v[y]) {
                m->pc += 4;
            } else {
                m->pc += 2;
            }
            break;

        case 0x6: // set Vx = nn (ld Vx, byte)
            m->v[x] = nn;
            m->pc += 2;
            break;

        case 0x7: // set Vx = Vx + nn (add Vx, byte)
            m->v[x] += nn;
            m->pc += 2;
            break;

        case 0x8:
            switch (n) {
                case 0x0: // set Vx = Vy (ld Vx, Vy)
                    m->v[x] = m->v[y];
                    m->pc += 2;
                    break;

                case 0x1: // set Vx = Vx | Vy (or Vx, Vy)
                    m->v[x] |= m->v[y];
                    m->pc += 2;
                    break;

                case 0x2: // set Vx = Vx & Vy (and Vx, Vy)
                    m->v[x] &= m->v[y];
                    m->pc += 2;
                    break;

                case 0x3: // set Vx = Vx ^ Vy (xor Vx, Vy)
                    m->v[x] ^= m->v[y];
                    m->pc += 2;
                    break;

                case 0x4: // set Vx = Vx + Vy, set VF = carry (add Vx, Vy)
                    if (m->v[x] + m->v[y] > 0xFF) {
                        m->v[0xF] = 1;
                    } else {
                        m->v[0xF] = 0;
                    }

                    m->v[x] += m->v[y];
                    m->pc += 2;
                    break;

                case 0x5: // set Vx = Vx - Vy, set VF = NOT borrow (sub Vx, Vy)
                    if (m->v[x] > m->v[y]) {
                        m->v[0xF] = 1;
                    } else {
                        m->v[0xF] = 0;
                    }

                    m->v[x] -= m->v[y];
                    m->pc += 2;
                    break;

                case 0x6: // set Vx = Vx >> 1 (shr Vx {, Vy})
                    m->v[0xF] = m->v[x] & 0x1;
                    m->v[x] >>= 1;
                    m->pc += 2;
                    break;

                case 0x7: // set Vx = Vy - Vx, set VF = NOT borrow (subn Vx, Vy)
                    if (m->v[y] > m->v[x]) {
                        m->v[0xF] = 1;
                    } else {
                        m->v[0xF] = 0;
                    }

                    m->v[x] = m->v[y] - m->v[x];
                    m->pc += 2;
                    break;

                case 0xE: // set Vx = Vx << 1 (shl Vx {, Vy})
                    m->v[0xF] = m->v[x] >> 7;
                    m->v[x] <<= 1;
                    m->pc += 2;
                    break;

                default:
                    // unknown opcode
                    return -1;
            }

            break;

        case 0x9: // skip next instruction if Vx != Vy (sne Vx, Vy)
            if (m->v[x] != m->v[y]) {
                m->pc += 4;
            } else {
                m->pc += 2;
            }
            break;

        case 0xA: // set I = nnn (ld I, addr)
            m->i = nnn;
            m->pc += 2;
            break;

        case 0xB: // jump to address nnn + V0 (jp V0, addr)
            m->pc = nnn + m->v[0];
            break;

        case 0xC: // set Vx = random byte AND nn (rnd Vx, byte)
            m->v[x] = (rand() % 0xFF) & nn;
            m->pc += 2;
            break;

        case 0xD: // draw n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision (drw Vx, Vy, nibble)
            m->v[0xF] = 0;

            int height = n;
            int rX = m->v[x];
            int rY = m->v[y];

            for (int yy = 0; yy < height; yy ++) {
                uint8_t curPix = m->memory[m->i + yy];
                for (int xx = 0; xx < 8; xx ++) {
                    if (curPix & 0x80) {
                        int curX = (rX + xx) % 64;
                        int curY = (rY + yy) % 32;
                        int curPos = curY * 64 + curX;

                        if (m->display[curPos] == 1) {
                            m->v[0xF] = 1;
                        }

                        m->display[curPos] ^= 1;
                    }
                    curPix <<= 1;
                }
            }

            m->draw_flag = 1;
            m->pc += 2;
            break;

        case 0xE:
            switch (nn) {
                case 0x9E: // skip next instruction if key with the value of Vx is pressed (skp Vx)
                    if (m->key[m->v[x]] == 1) {
                        m->pc += 4;
                    } else {
                        m->pc += 2;
                    }
                    break;

                case 0xA1: // skip next instruction if key with the value of Vx is not pressed (sknp Vx)
                    if (m->key[m->v[x]] == 0) {
                        m->pc += 4;
                    } else {
                        m->pc += 2;
                    }
                    break;

                default:
                    // unknown opcode
                    return -1;
            }

            break;

        case 0xF:
            switch (nn) {
                case 0x07: // set Vx = delay timer value (ld Vx, DT)
                    m->v[x] = m->delay_timer;
                    m->pc += 2;
                    break;

                case 0x0A: // wait for a key press, store the value of the key in Vx (ld Vx, K)
                    for (int i = 0; i < 16; i++) {
                        if (m->key[i] == 1) {
                            m->v[x] = i;
                            m->pc += 2;
                            break;
                        }
                    }
                    break;

                case 0x15: // set delay timer = Vx (ld DT, Vx)
                    m->delay_timer = m->v[x];
                    m->pc += 2;
                    break;

                case 0x18: // set sound timer = Vx (ld ST, Vx)
                    m->sound_timer = m->v[x];
                    m->pc += 2;
                    break;

                case 0x1E: // set I = I + Vx (add I, Vx)
                    m->i += m->v[x];
                    m->pc += 2;
                    break;

                case 0x29: // set I = location of sprite for digit Vx (ld F, Vx)
                    m->i = m->v[x] * 5;
                    m->pc += 2;
                    break;

                case 0x33: // store BCD representation of Vx in memory locations I, I+1, and I+2 (ld B, Vx)
                    m->memory[m->i] = m->v[x] / 100;
                    m->memory[m->i + 1] = (m->v[x] / 10) % 10;
                    m->memory[m->i + 2] = (m->v[x] % 100) % 10;
                    m->pc += 2;
                    break;

                case 0x55: // store registers V0 through Vx in memory starting at location I (ld [I], Vx)
                    for (int i = 0; i <= x; i++) {
                        m->memory[m->i + i] = m->v[i];
                    }
                    m->pc += 2;
                    break;

                case 0x65: // read registers V0 through Vx from memory starting at location I (ld Vx, [I])
                    for (int i = 0; i <= x; i++) {
                        m->v[i] = m->memory[m->i + i];
                    }
                    m->pc += 2;
                    break;

                default:
                    // unknown opcode
                    return -1;
            }
            break;
        default:
            // unknown opcode
            return -1;
    }

    return 0;
}
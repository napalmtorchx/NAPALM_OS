#pragma once
#include <lib/types.h>
#include <hal/ints/idt.h>

void ps2_init(void);
int ps2_wait_in(void);
int ps2_wait_out(void);
void ps2_setsample(uint8_t rate);
void ps2_sendcmd(uint8_t cmd);
void ps2_sendcmdex(uint8_t cmd, uint8_t arg);
uint8_t ps2_comm(uint8_t cmd);
void ps2_wrmouse(uint8_t data);
void ps2_wrkbd(uint8_t data);
void ps2_callback(irq_context_t* context);
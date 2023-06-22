#pragma once
#include <lib/ptrlist.h>
#include <core/threading/thread.h>
#include <hal/ints/idt.h>

void taskmgr_init(void);
void taskmgr_init_kthread(void);
void taskmgr_schedule(bool unlock);
void taskmgr_callback(irq_context_t* context);
void taskmgr_load(thread_t* thread);
void taskmgr_unload(thread_t* thread);
void taskmgr_start(thread_t* thread);
void taskmgr_kill(thread_t* thread);
void taskmgr_toggle(bool enabled);
void taskmgr_print(bool cmd);

uint32_t taskmgr_generate_uid(void);
bool taskmgr_exists(thread_t* thread);
thread_t* taskmgr_from_uid(uint32_t uid);
ptrlist_t* taskmgr_list(void);

void lock();
void unlock();
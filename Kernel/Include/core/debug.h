#pragma once
#include <lib/types.h>

#define DEBUG_INFO   "[  \x1b[36m>>\x1b[0m  ]"
#define DEBUG_OK     "[  \x1b[32mOK\x1b[0m  ]"
#define DEBUG_WARN   "[  \x1b[33m??\x1b[0m  ]"
#define DEBUG_ERROR  "[  \x1b[31m!!\x1b[0m  ]"

#define DEBUG_MMAP   "[ \x1b[36mMMAP\x1b[0m ]"
#define DEBUG_MUNMAP "[\x1b[33mMUNMAP\x1b[0m]"

#define DEBUG_MREQ   "[ \x1b[36mMREQ\x1b[0m ]"
#define DEBUG_MCHG   "[ \x1b[36mMCHG\x1b[0m ]"
#define DEBUG_MREL   "[ \x1b[33mMREL\x1b[0m ]"

#define DEBUG_MALLOC "[\x1b[36mMALLOC\x1b[0m]"
#define DEBUG_FREE   "[ \x1b[33mFREE\x1b[0m ]"

#define DEBUG_THREAD   "[\x1b[36mTHREAD\x1b[0m]"
#define DEBUG_VM       "[  \x1b[32mVM\x1b[0m  ]"

void debug_out(const char* fmt, ...);
void debug_exception(const char* fmt, ...);
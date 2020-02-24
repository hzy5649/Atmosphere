/*
 * Copyright (c) 2019 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

//#include "exceptions.h"
struct ExceptionStackFrame;
//#include "gdb/hvisor_context.h"
#include "transport_interface.h"

//extern GDBContext g_gdbContext;

typedef enum DebugEventType {
    DBGEVENT_NONE = 0,
    DBGEVENT_DEBUGGER_BREAK,
    DBGEVENT_EXCEPTION,
    DBGEVENT_CORE_ON,
    DBGEVENT_CORE_OFF,
    DBGEVENT_EXIT,
    DBGEVENT_OUTPUT_STRING,
} DebugEventType;

typedef struct OutputStringDebugEventInfo {
    uintptr_t address;
    size_t size;
} OutputStringDebugEventInfo;

typedef struct DebugEventInfo {
    DebugEventType type;
    bool preprocessed;
    bool handled;
    u32 coreId;
    struct ExceptionStackFrame *frame;
    union {
        OutputStringDebugEventInfo outputString;
    };
} DebugEventInfo;

void debugManagerPauseSgiHandler(void);

void debugManagerInit(TransportInterfaceType gdbIfaceType, u32 gdbIfaceId, u32 gdbIfaceFlags);

void debugManagerSetReportingEnabled(bool enabled);

// Hypervisor interrupts will be serviced during the pause-wait
bool debugManagerHandlePause(void);

DebugEventInfo *debugManagerGetDebugEvent(u32 coreId);
bool debugManagerHasDebugEvent(u32 coreId);

// Note: these functions are not reentrant EXCEPT debugPauseCores(1 << currentCoreId)
// "Pause" makes sure all cores reaches the pause function before proceeding.
// "Unpause" doesn't synchronize, it just makes sure the core resumes & that "pause" can be called again.
void debugManagerPauseCores(u32 coreList);
void debugManagerUnpauseCores(u32 coreList);
void debugManagerSetSingleStepCoreList(u32 coreList);

void debugManagerSetSteppingRange(u32 coreId, uintptr_t startAddr, uintptr_t endAddr);

u32 debugManagerGetPausedCoreList(void);


void debugManagerReportEvent(DebugEventType type, ...);

void debugManagerBreakCores(u32 coreList);
void debugManagerContinueCores(u32 coreList);

static inline bool debugManagerIsCorePaused(u32 coreId)
{
    return (debugManagerGetPausedCoreList() & BIT(coreId)) != 0;
}

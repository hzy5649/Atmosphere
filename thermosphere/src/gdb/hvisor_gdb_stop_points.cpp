/*
 * Copyright (c) 2019-2020 Atmosphère-NX
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

/*
*   This file is part of Luma3DS.
*   Copyright (C) 2016-2019 Aurora Wright, TuxSH
*
*   SPDX-License-Identifier: (MIT OR GPL-2.0-or-later)
*/

#include "hvisor_gdb_defines_internal.hpp"
#include "hvisor_gdb_packet_data.hpp"

#include "../hvisor_hw_breakpoint_manager.hpp"
#include "../hvisor_sw_breakpoint_manager.hpp"
#include "../hvisor_watchpoint_manager.hpp"


namespace ams::hvisor::gdb {

    GDB_DEFINE_HANDLER(ToggleStopPoint)
    {
        bool add = m_commandLetter == 'Z';

        auto [nread, kind, addr, size] = ParseHexIntegerList<3>(m_commandData, ';');
        if (nread == 0) {
            return ReplyErrno(EILSEQ);
        }
        m_commandData.remove_prefix(nread);
    
        // We don't support cond_list
        bool persist = m_commandData == "cmds:1";

        // In theory we should reject leading zeroes in "kind". Oh well...

        int res;
        static const cpu::DebugRegisterPair::LoadStoreControl kinds[3] = {
            cpu::DebugRegisterPair::Store,
            cpu::DebugRegisterPair::Load,
            cpu::DebugRegisterPair::LoadStore,
        };

        auto &hwBpMgr = HwBreakpointManager::GetInstance();
        auto &swBpMgr = SwBreakpointManager::GetInstance();
        auto &wpMgr = WatchpointManager::GetInstance();

        switch(kind) {
            // Software breakpoint
            case 0: {
                if(size != 4) {
                    return ReplyErrno(EINVAL);
                }
                res = add ? swBpMgr.Add(addr, persist) : swBpMgr.Remove(addr, false);
                return res == 0 ? ReplyOk() : ReplyErrno(-res);
            }

            // Hardware breakpoint
            case 1: {
                if(size != 4) {
                    return ReplyErrno(EINVAL);
                }
                res = add ? hwBpMgr.Add(addr) : hwBpMgr.Remove(addr);
                return res == 0 ? ReplyOk() : ReplyErrno(-res);
            }

            // Watchpoints
            case 2:
            case 3:
            case 4: {
                res = add ? wpMgr.Add(addr, size, kinds[kind - 2]) : wpMgr.Remove(addr, size, kinds[kind - 2]);
                return res == 0 ? ReplyOk() : ReplyErrno(-res);
            }
            default: {
                return ReplyEmpty();
            }
        }
    }

}

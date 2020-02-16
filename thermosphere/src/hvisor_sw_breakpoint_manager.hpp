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

#pragma once

#include "defines.hpp"
#include "hvisor_irq_manager.hpp"

#define MAX_SW_BREAKPOINTS  16

namespace ams::hvisor {

    class SwBreakpointManager : public IInterruptTask {
        SINGLETON(SwBreakpointManager);
        private:
            struct Breakpoint {
                uintptr_t address;
                u32 savedInstruction;
                u16 uid;
                bool persistent;
                bool applied;
            };

        private:
            mutable RecursiveSpinlock m_lock{};
            mutable Barrier m_applyBarrier{};

            u32 m_bpUniqueCounter = 0;
            size_t m_numBreakpoints = 0;
            std::array<Breakpoint, MAX_SW_BREAKPOINTS> m_breakpoints{};

        private:
            size_t FindClosest(uintptr_t addr) const;

            bool DoApply(size_t id);
            bool DoRevert(size_t id);

            bool ApplyOrRevert(size_t id, bool apply);

        public:
            int Add(uintptr_t addr, bool persistent);
            int Remove(uintptr_t addr, bool keepPersistent);
            int RemoveAll(bool keepPersistent);

            std::optional<bool> InterruptTopHalfHandler(u32 irqId, u32) final;
            void Initialize()
            {
                IrqManager::GetInstance().Register(*this, IrqManager::ApplyRevertSwBreakpointSgi, false);
            }
        public: 
            constexpr SwBreakpointManager() = default;
    };
}

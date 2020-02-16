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
#include "cpu/hvisor_cpu_debug_register_pair.hpp"
#include "hvisor_irq_manager.hpp"

namespace ams::hvisor {

    class HwStopPointManager : public IInterruptTask {
        NON_COPYABLE(HwStopPointManager);
        NON_MOVEABLE(HwStopPointManager);
        protected:
            static constexpr size_t maxStopPoints = std::max(MAX_BCR, MAX_WCR);

        protected:
            mutable RecursiveSpinlock m_lock{};
            mutable Barrier m_reloadBarrier{};

            u16 m_freeBitmap;
            u16 m_usedBitmap = 0;
            std::array<cpu::DebugRegisterPair, maxStopPoints> m_stopPoints{};
            IrqManager::ThermosphereSgi m_irqId;

        protected:
            void DoReloadOnAllCores() const;
            cpu::DebugRegisterPair *Allocate();
            void Free(size_t pos);
            const cpu::DebugRegisterPair *Find(uintptr_t addr, size_t size, cpu::DebugRegisterPair::LoadStoreControl dir) const;

            virtual bool FindPredicate(const cpu::DebugRegisterPair &pair, uintptr_t addr, size_t size, cpu::DebugRegisterPair::LoadStoreControl direction) const = 0;
            virtual void Reload() const = 0;

            int AddImpl(uintptr_t addr, size_t size, cpu::DebugRegisterPair preconfiguredPair);
            int RemoveImpl(uintptr_t addr, size_t size, cpu::DebugRegisterPair::LoadStoreControl direction);

        protected:
            constexpr HwStopPointManager(size_t numStopPoints, IrqManager::ThermosphereSgi irqId) :
                m_freeBitmap(MASK(numStopPoints)), m_irqId(irqId)
            {
            }

        public:
            void RemoveAll();
            std::optional<bool> InterruptTopHalfHandler(u32 irqId, u32) final;

            void ReloadOnAllCores() const
            {
                m_lock.lock();
                DoReloadOnAllCores();
                m_lock.unlock();
            }

            void Initialize()
            {
                IrqManager::GetInstance().Register(*this, m_irqId, false);
            }
    };
}

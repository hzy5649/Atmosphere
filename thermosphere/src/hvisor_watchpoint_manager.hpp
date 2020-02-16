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

#include "hvisor_hw_stop_point_manager.hpp"

namespace ams::hvisor {

    class WatchpointManager final : public HwStopPointManager {
        SINGLETON(WatchpointManager);
        private:
            bool FindPredicate(const cpu::DebugRegisterPair &pair, uintptr_t addr, size_t size, cpu::DebugRegisterPair::LoadStoreControl direction) const final;
            void Reload() const final;

        public:
            virtual void ReloadOnAllCores() const;
            static void ReloadOnAllCoresSgiHandler();

            cpu::DebugRegisterPair RetrieveWatchpointConfig(uintptr_t addr, cpu::DebugRegisterPair::LoadStoreControl direction) const;
            int Add(uintptr_t addr, size_t size, cpu::DebugRegisterPair::LoadStoreControl direction);
            int Remove(uintptr_t addr, size_t size, cpu::DebugRegisterPair::LoadStoreControl direction);

        public:
            constexpr WatchpointManager() : HwStopPointManager(MAX_WCR, IrqManager::ReloadWatchpointsSgi) {}
    };
}

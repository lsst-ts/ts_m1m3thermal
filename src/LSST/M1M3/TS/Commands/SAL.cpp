/*
 * SAL command.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <Commands/SAL.h>
#include <Events/SummaryState.h>
#include <cRIO/ControllerThread.h>

#include <spdlog/spdlog.h>

using namespace LSST::M1M3::TS::Commands;
using namespace LSST::M1M3::TS::Events;
using namespace MTM1M3TS;

bool SAL_start::validate() {
    if (params.settingsToApply.empty()) {
        return false;
    }
    return true;
}

void SAL_start::execute() {
    SPDLOG_INFO("Starting");
    SummaryState::setState(MTM1M3TS_shared_SummaryStates_DisabledState);
    ackComplete();
    SPDLOG_INFO("Started");
}

void SAL_enable::execute() {
    SummaryState::setState(MTM1M3TS_shared_SummaryStates_EnabledState);
    ackComplete();
}

void SAL_disable::execute() {
    SummaryState::setState(MTM1M3TS_shared_SummaryStates_DisabledState);
    ackComplete();
}

void SAL_standby::execute() {
    SummaryState::setState(MTM1M3TS_shared_SummaryStates_StandbyState);
    ackComplete();
}

void SAL_exitControl::execute() {
    LSST::cRIO::ControllerThread::setExitRequested();
    ackComplete();
}

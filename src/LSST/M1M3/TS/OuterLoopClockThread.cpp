/*
 * Thread to catch outer loop clock interrupts.
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

#include <cRIO/ControllerThread.h>
#include <OuterLoopClockThread.h>
#include <Commands/Update.h>
#include <Events/SummaryState.h>

#include <chrono>
#include <spdlog/spdlog.h>

using namespace std::chrono_literals;
using namespace LSST::M1M3::TS;

void OuterLoopClockThread::run() {
    SPDLOG_INFO("OuterLoopClockThread: Run");
    std::unique_lock<std::mutex> lock(runMutex);
    cRIO::ControllerThread::instance().enqueue(new Commands::Update());
    while (keepRunning) {
        runCondition.wait_for(lock, 20ms);
        if (Events::SummaryState::instance().active()) {
            cRIO::ControllerThread::instance().enqueue(new Commands::Update());
        }
    }
    SPDLOG_INFO("OuterLoopClockThread: Completed");
}

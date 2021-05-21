/*
 * Abstract FPGA interface.
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

#ifndef __TS_IFPGA__
#define __TS_IFPGA__

#include <cRIO/FPGA.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Abstract FPGA Interface. Provides common parent for real and simulated FPGA. Singleton.
 */
class IFPGA : public cRIO::FPGA {
public:
    IFPGA() : cRIO::FPGA(cRIO::fpgaType::TS) {}
    virtual ~IFPGA() {}
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !__TS_IFPGA__

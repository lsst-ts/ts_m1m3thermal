/*
 * Command line Thermal System client.
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

#include "ThermalFPGA.h"

#include <cRIO/ThermalILC.h>
#include <cRIO/PrintILC.h>
#include <cRIO/FPGACliApp.h>

#include <iostream>
#include <iomanip>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

class M1M3TScli : public FPGACliApp {
public:
    M1M3TScli(const char* name, const char* description);

    int setPower(command_vec cmds);

protected:
    virtual FPGA* newFPGA(const char* dir) override;
    virtual ILCUnits getILCs(command_vec cmds) override;
};

class PrintThermalILC : public ThermalILC, public PrintILC {
public:
    PrintThermalILC() : ThermalILC(1), PrintILC(1) {}

protected:
    void processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature, uint8_t fanRPM,
                              float absoluteTemperature) override;
};

class PrintTSFPGA : public ThermalFPGA {
public:
    PrintTSFPGA(const char* dir) : ThermalFPGA(dir) {}

    void writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void writeRequestFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
};

M1M3TScli::M1M3TScli(const char* name, const char* description) : FPGACliApp(name, description) {
    addCommand("power", std::bind(&M1M3TScli::setPower, this, std::placeholders::_1), "i", NEED_FPGA, "<0|1>",
               "Power off/on ILC bus");
    addILC(std::make_shared<PrintThermalILC>());
}

int M1M3TScli::setPower(command_vec cmds) {
    uint16_t net = 1;
    uint16_t aux = 0;
    switch (cmds.size()) {
        case 0:
            break;
        case 1:
            net = aux = CliApp::onOff(cmds[0]);
            break;
        case 2:
            net = CliApp::onOff(cmds[0]);
            aux = CliApp::onOff(cmds[1]);
            break;
        default:
            std::cerr << "Invalid number of arguments to power command." << std::endl;
            return -1;
    }
    uint16_t pa[16] = {65, aux, 66, aux, 67, aux, 68, aux, 69, net, 70, net, 71, net, 72, net};
    getFPGA()->writeCommandFIFO(pa, 16, 0);
    return 0;
}

FPGA* M1M3TScli::newFPGA(const char* dir) { return new PrintTSFPGA(dir); }

ILCUnits M1M3TScli::getILCs(command_vec cmds) {
    ILCUnits units;
    int ret = -2;

    for (auto c : cmds) {
        try {
            int address = std::stoi(c);
            if (address <= 0 || address > NUM_TS_ILC) {
                std::cerr << "Invalid address " << c << std::endl;
                ret = -1;
                continue;
            }
            units.push_back(ILCUnit(getILC(0), address));
        } catch (std::logic_error& e) {
            std::cerr << "Non-numeric address: " << c << std::endl;
            ret = -1;
        }
    }

    if (ret == -2) {
        std::cout << "Info for all ILC" << std::endl;
        for (int i = 1; i <= NUM_TS_ILC; i++) {
            units.push_back(ILCUnit(getILC(0), i));
        }
        ret = 0;
    }
    return units;
}

void PrintThermalILC::processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature,
                                           uint8_t fanRPM, float absoluteTemperature) {
    printBusAddress(address);
    std::cout << "Thermal status: " << std::to_string(status) << std::endl
              << "Differential temperature: " << std::to_string(differentialTemperature) << std::endl
              << "Fan RPM: " << std::to_string(fanRPM) << std::endl;
}

M1M3TScli cli("M1M3TS", "M1M3 Thermal System Command Line Interface");

void _printBuffer(std::string prefix, uint16_t* buf, size_t len) {
    if (cli.getDebugLevel() == 0) {
        return;
    }

    std::cout << prefix;
    for (size_t i = 0; i < len; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(4) << buf[i] << " ";
    }
    std::cout << std::endl;
}

void PrintTSFPGA::writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    _printBuffer("C> ", data, length);
    ThermalFPGA::writeCommandFIFO(data, length, timeout);
}

void PrintTSFPGA::writeRequestFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    _printBuffer("R> ", data, length);
    ThermalFPGA::writeRequestFIFO(data, length, timeout);
}

void PrintTSFPGA::readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    ThermalFPGA::readU16ResponseFIFO(data, length, timeout);
    _printBuffer("R< ", data, length);
}

int main(int argc, char* const argv[]) { return cli.run(argc, argv); }

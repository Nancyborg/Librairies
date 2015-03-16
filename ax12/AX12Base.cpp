#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "AX12Base.h"

AX12Base::AX12Base(int id, int baud /* = 1000000 */, int timeout /* = 1000 */)
    : timeout(timeout), debugOn(false)
{
    setCurrentID(id);
    setCurrentBaud(baud);
}

void AX12Base::pingWait() {
    while (!ping());
}

bool AX12Base::reset() {
    return writePacket(AX12_INSTR_RESET, 0, NULL);
}

bool AX12Base::ping() {
    return writePacket(AX12_INSTR_PING, 0, NULL);
}

bool AX12Base::changeID(uint8_t new_id) {
    bool ret = writeRegister1(AX12_ROM_ID, new_id);

    if (ret && id != AX12_BROADCAST)
        setCurrentID(new_id);

    return ret;
}

bool AX12Base::changeBaud(int new_baud) {
    uint8_t val = round((2000000 / (double) new_baud) - 1);
    bool ret = writeRegister1(AX12_ROM_BAUD_RATE, val);

    if (ret) {
        setCurrentBaud(new_baud);
    }

    return ret;
}

float AX12Base::getPresentPosition() {
    int ret = readRegister2(AX12_RAM_PRESENT_POSITION);
    if (ret < 0)
        return -42;

    return regValToDeg(ret);
}

float AX12Base::getPresentSpeed() {
    int ret = readRegister2(AX12_RAM_PRESENT_SPEED);

    if (ret < 0)
        return -42;

    return regValToFloat(ret);
}

bool AX12Base::setGoalPosition(float angle, bool block /* = false */) {
    bool ret = writeRegister2(AX12_RAM_GOAL_POSITION, degToRegVal(angle));

    if (ret && block) {
        while (isMoving())
            ;
    }

    return ret;
}

bool AX12Base::setGoalSpeed(float speed) {
    return writeRegister2(AX12_RAM_MOVING_SPEED, floatToRegVal(speed));
}

bool AX12Base::setCWLimit(float angle) {
    return writeRegister2(AX12_ROM_CW_ANGLE_LIMIT, degToRegVal(angle));
}

float AX12Base::getCWLimit() {
    int ret = readRegister2(AX12_ROM_CW_ANGLE_LIMIT);
    if (ret < 0) {
        return -42;
    }
    return regValToDeg(ret);
}

float AX12Base::getCCWLimit() {
    int ret = readRegister2(AX12_ROM_CCW_ANGLE_LIMIT);
    if (ret < 0) {
        return -42;
    }
    return regValToDeg(ret);
}

bool AX12Base::setCCWLimit(float angle) {
    return writeRegister2(AX12_ROM_CCW_ANGLE_LIMIT, degToRegVal(angle));
}

bool AX12Base::setTorqueEnable(bool enable) {
    return writeRegister1(AX12_RAM_TORQUE_ENABLE, enable);
}

bool AX12Base::isMoving() {
    return readRegister1(AX12_RAM_MOVING);
}

int AX12Base::readRegister1(uint8_t reg_start) {
    if (!readData(reg_start, 1))
        return -1;
    
    return getData()[0];
}

int AX12Base::readRegister2(uint8_t reg_start) {
    if (!readData(reg_start, 2))
        return -1;
    
    return getData()[0] | (getData()[1] << 8);
}

bool AX12Base::writeRegister1(uint8_t reg_start, uint8_t val) {
    uint8_t buffer[] = { val };
    return writeData(reg_start, 1, buffer);
}

bool AX12Base::writeRegister2(uint8_t reg_start, uint16_t val) {
    uint8_t buffer[] = { (uint8_t) (val & 0xFF), (uint8_t) (val >> 8) };
    return writeData(reg_start, 2, buffer);
}

bool AX12Base::readData(uint8_t reg_start, uint8_t len) {
    uint8_t buffer[] = { reg_start, len };

    bool ret = writePacket(AX12_INSTR_READ_DATA, 2, buffer);

    if (ret && getDataSize() != len) {
        debug("readData: returned data length (%d) different than requested (%d)\n", getDataSize(), len);
    }

    return ret;
}

bool AX12Base::writeData(uint8_t reg_start, uint8_t len, const uint8_t data[]) {
    uint8_t buffer[32] = { reg_start };
    memcpy(buffer + 1, data, len);
    
    return writePacket(AX12_INSTR_WRITE_DATA, 1 + len, buffer);
}

// Instruction packet : FF FF <ID> <LEN> <INSTR> <PAR0>..<PARN> <CKSUM>
bool AX12Base::writePacket(uint8_t instr, uint8_t len, uint8_t data[]) {
    uint8_t send_packet[6 + len];
    bool ret = false;

    beginComm();
    setCommError(AX12_COMM_ERROR_NONE);
    flushInput();

    int pos = 0;
    send_packet[pos++] = 0xFF;
    send_packet[pos++] = 0xFF;
    send_packet[pos++] = id;
    send_packet[pos++] = len + 2;
    send_packet[pos++] = instr;

    memcpy(send_packet + pos, data, len);
    pos += len;

    send_packet[pos++] = checksum(&send_packet[2], len + 3);

    debug("writePacket: ");
    dumpHex(send_packet, pos);

    if (writeBytes(send_packet, pos) != pos)
        goto end;

    if (comm_error)
        goto end;

    if (id != AX12_BROADCAST)
        ret = readPacket();
    else
        ret = true;

end:
    endComm();

    return ret;
}

// Status packet : FF FF <ID> <LEN> <ERROR> <PAR0>...<PARN> <CKSUM>
bool AX12Base::readPacket() {
    uint8_t cksum1, cksum2;
    int len = 0;

    if (readBytes(recv_packet, 4, 5000) < 4)
        goto end;

    if (recv_packet[0] != 0xFF || recv_packet[1] != 0xFF) {
        setCommError(AX12_COMM_ERROR_FORMAT);
        goto end;
    }

    if (recv_packet[2] != id) {
        setCommError(AX12_COMM_ERROR_ID);
        debug("invalid id: %d\n", recv_packet[2]);
        goto end;
    }

    len = recv_packet[3];

    if (readBytes(recv_packet + 4, len, timeout) != len)
        goto end;

    cksum1 = checksum(recv_packet + 2, len + 1);
    cksum2 = recv_packet[len + 3];

    if (cksum1 != cksum2) {
        debug("checksum error, calculated=%x, received=%x\n", cksum1, cksum2);
        comm_error |= AX12_COMM_ERROR_CHECKSUM;
        goto end;        
    }

    if (getErrors()) {
        debug("error field=%x\n", getErrors());
    }

end:
    debug("read packet : ");
    dumpHex(recv_packet, len + 4);
    return !comm_error;
}

uint8_t AX12Base::checksum(uint8_t data[], uint8_t len) {
    uint8_t res = 0;

    for (uint8_t i = 0; i < len; i++)
        res += data[i];

    return (~res) & 0xFF;
}

void AX12Base::setCommError(int comm_error) {
    this->comm_error = comm_error;
    if (comm_error)
        debug("comm_error=%d\n", comm_error);
}


void AX12Base::debug(const char *format, ...) {
    if (!debugOn)
        return;
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}

void AX12Base::dumpHex(const uint8_t *buffer, int len) {
    for (int i = 0; i < len; i++) {
        debug("%02x ", buffer[i]);
    }
    debug("\n");
}


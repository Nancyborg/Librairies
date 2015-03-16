/**
 * \file    AX12Mbed.h
 * \author  Mickaël Thomas
 * \version 1.0
 * \date    02/04/2014
 * \brief   MBED implementation of AX12Base
 */
#ifndef AX12_MBED_H
#define AX12_MBED_H

#include "mbed.h"
#include "AX12Base.h"
#include "SerialHalfDuplex.h"


/** \brief MBED implementation if AX12Base
 */
template <class SerialType=Serial>
class AX12Mbed : public AX12Base {
public:
    AX12Mbed(PinName tx, PinName rx, int id, int baud = 1000000)
        : AX12Base(id, baud), ax12(tx, rx)
    {
        ax12.baud(baud);
    }

    void setCurrentBaud(int new_baud) {
        AX12Base::setCurrentBaud(new_baud);

        ax12.baud(new_baud);
    }

private:
    int readBytes(uint8_t *bytes, int n, int timeout) {
        Timer timer;
        timer.start();
        debug("readBytes(n=%d, timeout=%d)\n", n, timeout);

        for (int i = 0; i < n; i++) {
            if (timeout >= 0) {
                while (!ax12.readable() && timer.read_us() <= timeout) {
                    debug("wait (%d/%d)...", timer.read_us(), timeout);
                }

                if (!ax12.readable()) {
                    debug("got timeout (%d >= %d)\n", timer.read_us(),  timeout);
                    setCommError(AX12_COMM_ERROR_TIMEOUT);
                    return -1;
                }
            }
            bytes[i] = ax12.getc();
        }

        debug("elapsed: %d\n", timer.read_us());

        return n;
    }

    int writeBytes(const uint8_t *bytes, int n) {
        for (int i = 0; i < n; i++)
            ax12.putc(bytes[i]);

        return n;
    }

    void flushInput() {
        while (ax12.readable())
            ax12.getc();
    }

    SerialType ax12;
};

typedef AX12Mbed<Serial> AX12;

#endif

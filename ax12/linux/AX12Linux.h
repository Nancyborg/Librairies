/**
 * \file    AX12Mbed.h
 * \author  Mickaël Thomas
 * \version 1.0
 * \date    16/04/2014
 * \brief   Linux implementation of AX12Base
 */
#ifndef AX12_MBED_H
#define AX12_MBED_H

#include "../AX12Base.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifndef AX12
#define AX12 AX12Linux
#endif

#define ERROR(what)                          \
    do {                                     \
        perror("AX12: " what);               \
        setCommError(AX12_COMM_ERROR_SYSTEM);\
        sys_error = errno;                   \
    } while (0)

/** \brief Linux implementation of AX12Base
 */
class AX12Linux: public AX12Base {
public:
    AX12Linux(const char *devpath, int id, int baud = 1000000)
        : AX12Base(id, baud)
    {
        int baudcst = rate_to_constant(baud);

        if (!baudcst) {
            printf("Baud rate not supported : %d\n", baud);
            setCommError(AX12_COMM_ERROR_SYSTEM);
            sys_error = EINVAL;
            return;
        }

        fd = open(devpath, O_RDWR | O_NOCTTY);

        if (fd < 0) {
            ERROR("open");
            return;
        }

        tcgetattr(fd, &oldtio); /* save current port settings */
        memset(&newtio, 0, sizeof(newtio));
        cfsetspeed(&newtio, baudcst);
        cfmakeraw(&newtio);
        newtio.c_cflag |= (CLOCAL | CREAD);
        tcsetattr(fd, TCSANOW, &newtio);

        timeout = 999;
    }

    ~AX12Linux() {
        tcsetattr(fd, TCSANOW, &oldtio);
        close(fd);
    }

    /** \brief Get the latest system error code (errno) */
    int getSysError() {
        if (getCommError() & AX12_COMM_ERROR_SYSTEM)
            return sys_error;
        else
            return 0;
    }

    void setCurrentBaud(int new_baud) {
        AX12Base::setCurrentBaud(new_baud);

        int baudcst = rate_to_constant(new_baud);
        cfsetspeed(&newtio, baudcst);
        tcsetattr(fd, TCSANOW, &newtio);
    }
private:

    static int rate_to_constant(int baudrate) {
#define B(x) case x: return B##x
        switch(baudrate) {
            B(50);     B(75);     B(110);    B(134);    B(150);
            B(200);    B(300);    B(600);    B(1200);   B(1800);
            B(2400);   B(4800);   B(9600);   B(19200);  B(38400);
            B(57600);  B(115200); B(230400); B(460800); B(500000); 
            B(576000); B(921600); B(1000000);B(1152000);B(1500000); 
        default: return 0;
        }
#undef B
    }

    int readBytes(uint8_t *bytes, int len, int timeoutval) {
        fd_set set;
        struct timeval timeout;
        int rv;
        int n = 0;

        timeout.tv_sec = 0;
        timeout.tv_usec = 1000 * timeoutval;

        FD_ZERO(&set);
        FD_SET(fd, &set);

        rv = select(fd + 1, &set, NULL, NULL, &timeout);

        if (rv == -1) {
            ERROR("select");
            return -1;
        } else if (rv == 0) {
            setCommError(AX12_COMM_ERROR_TIMEOUT);
            return -1;
        }

        while (n < len) {
            rv = read(fd, bytes + n, len - n);
            if (rv <= 0) {
                ERROR("read");
                return -1;
            }
            n += rv;
        }
        return n;
    }

    int writeBytes(const uint8_t *bytes, int len) {
        int ret;
        int n = 0;
        while (n < len) {
            if ((ret = write(fd, bytes + n, len - n)) <= 0) {
                ERROR("write");
                return -1;
            }
            n += ret;
        }
        printf("writeBytes(%s, %d) = %d\n", bytes, n, ret);
        return n;
    }

    void flushInput() {
        tcflush(fd,TCIOFLUSH);
    }

    int fd;
    int sys_error;
    struct termios oldtio, newtio;
};

#undef ERROR
#endif


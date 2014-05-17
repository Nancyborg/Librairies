#include "AX12Mbed.h"

#define PIN_TX p9
#define PIN_RX p10

Serial pc(USBTX, USBRX);

int baud = 1000000, id = 1;

char *readAll(Serial& s) {
    static char buff[64];
    int len = 0;
    while (s.readable()) {
        buff[len++] = s.getc();
    }
    buff[len] = 0;
    return buff;
}

void setBaudRate() {
    printf("Baud:\n");
    scanf("%d", &baud);
    printf("Baud rate changed to %d\n", baud);
}

void setID() {
    printf("ID (-1 for broadcast):\n");
    scanf("%d", &id);
    if (id == -1)
        id = AX12::AX12_BROADCAST;

    printf("ID rate changed to %d\n", id);
}

void baudRateScan(void) {
    AX12 ax(PIN_TX, PIN_RX, id, baud);
    int bauds[] = { 1000000,  500000, 400000, 250000, 200000, 115200, 57600, 19200, 9600 };

    for (size_t i = 0; i < (sizeof(bauds) / sizeof(*bauds)); i++) {
        printf("Scan: %d\n", bauds[i]);

        ax.setCurrentBaud(bauds[i]);

        if (ax.ping()) {
            printf("**** Found AX12 for baudrate %d\n", bauds[i]);
        }
    }

    printf("End of scan\n");
}

void idScan(void) {
    AX12 ax(PIN_TX, PIN_RX, id, baud);
    for (int i = 1; i < 0xFF; i++) {
        printf("Scan ID: %d\n", i);
        ax.setCurrentID(i);

        if (ax.ping()) {
            printf("**** Found AX12 for id %d\n", i);
        }
    }
}

void pingTest() {
    AX12 ax(PIN_TX, PIN_RX, id, baud);

    if (ax.ping()) {
        printf("Successful\n");
    } else {
        printf("Error\n");
    }
}

void moveTest() {
    AX12 ax(PIN_TX, PIN_RX, id, baud);
    float angle;
    printf("Angle:\n");
    scanf("%f", &angle);
    if (ax.setGoalPosition(angle)) {
        printf("Successful\n");
    } else {
        printf("Error\n");
    }
}

void idChange(void) {
    AX12 ax(PIN_TX, PIN_RX, id, baud);
    int newid;
    printf("New ID:\n");
    scanf("%d", &newid);

    if (ax.changeID(newid)) {
        printf("Success\n");
        id = newid;
    } else {
        printf("Error\n");
    }
}

void baudRateChange(void) {
    AX12 ax(PIN_TX, PIN_RX, id, baud);
    int newbaud;
    printf("New baud rate:\n");
    scanf("%d", &newbaud);
    printf("new baud: %d\n", newbaud);

    if (ax.changeBaud(newbaud)) {
        printf("Success\n");
        baud = newbaud;
    } else {
        printf("Error\n");
    }
}

void clearEEPROM(void) {
    Serial axserial(p9, p10);
    axserial.baud(57600);
    for (;;) {
        if (axserial.readable())
            printf("read: %s\n", readAll(axserial));
        if (pc.readable()) {
            int c = pc.getc();
            printf("write: %c\n", c);
            axserial.putc(c);
        } else {
/*            axserial.putc('#');
            wait(0.1);*/
        }
    }
}

int main(void) {
    pc.baud(115200);

    for (;;) {
        printf("----- AX12 tool -----\n");

        printf("-- Communication settings --\n");
        printf("1. Set baud rate\n");
        printf("2. Set ID\n");
        printf("-- Actions --\n");
        printf("3. Baud rate scan\n");
        printf("4. ID scan\n");
        printf("5. Ping test\n");
        printf("6. Move test\n");
        printf("7. Change ID\n");
        printf("8. Change baud rate\n");
        printf("9. Clear eeprom using bootloader\n");

        printf("Choice : \n");

        switch (getchar()) {
            case '1':
                setBaudRate();
                break;
            case '2':
                setID();
                break;
            case '3':
                baudRateScan();
                break;
            case '4':
                idScan();
                break;
            case '5':
                pingTest();
                break;
            case '6':
                moveTest();
                break;
            case '7':
                idChange();
                break;
            case '8':
                baudRateChange();
                break;
            case '9':
                clearEEPROM();
                break;
            default:
                break;
        }
        while (pc.readable())
            getchar();
    }
}

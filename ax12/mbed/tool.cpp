#include "AX12Mbed.h"

#define PIN_TX p9
#define PIN_RX p10

Serial pc(USBTX, USBRX);

int baud = 115200, id = 1;

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
    printf("Set baud rate: ");
    scanf("%d", &baud);
    printf("Baud rate changed to %d\n", baud);
}

void setID() {
    printf("Set ID (-1 for broadcast): ");
    scanf("%d", &id);
    if (id == -1)
        id = AX12::AX12_BROADCAST;

    printf("ID rate changed to %d\n", id);
}

void scan(void) {
    int bauds[] = { 1000000,  500000, 400000, 250000, 200000, 115200, 57600, 19200, 9600 };
    AX12 ax(PIN_TX, PIN_RX, id, baud);

    printf("Scanning...\n");
    for (size_t i = 0; i < (sizeof(bauds) / sizeof(*bauds)); i++) {
        ax.setCurrentBaud(bauds[i]);

        for (int j = 0; j < 0xFE; j++) {
            ax.setCurrentID(j);

            printf("Scanning ID %d,  baud = %-10d\r", j, bauds[i]);

            if (ax.ping()) {
                printf("Found AX12 with ID = %d and baudrate = %d\n", j, bauds[i]);
            }
        }
    }
    printf("Scanning finished.%-15s\n", "");
}

void pingTest() {
    AX12 ax(PIN_TX, PIN_RX, id, baud);

    if (ax.ping()) {
        printf("Ping successful.\n");
    } else {
        printf("Ping failed.\n");
    }
}

void moveTest() {
    AX12 ax(PIN_TX, PIN_RX, id, baud);
    float angle;
    printf("Angle:\n");
    scanf("%f", &angle);
    if (ax.setGoalPosition(angle)) {
        printf("Move successful.\n");
    } else {
        printf("Move failed.\n");
    }
}

void idChange(void) {
    AX12 ax(PIN_TX, PIN_RX, id, baud);
    int newid;
    printf("New ID:\n");
    scanf("%d", &newid);

    if (ax.changeID(newid)) {
        printf("ID successfully changed to %d\n", newid);
        id = newid;
    } else {
        printf("ID change failed.\n");
    }
}

void baudRateChange(void) {
    AX12 ax(PIN_TX, PIN_RX, id, baud);
    int newbaud;
    printf("New baud rate:\n");
    scanf("%d", &newbaud);
    printf("new baud: %d\n", newbaud);

    if (ax.changeBaud(newbaud)) {
        printf("Baudrate successfully changed to %d\n", newbaud);
        baud = newbaud;
    } else {
        printf("Baudrate change failed.\n");
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
    bool showMenu = true;
    pc.baud(115200);

    for (;;) {
        if (showMenu) {
            printf("----- AX12 tool -----\n");

            printf("-- Communication settings --\n");
            printf("1. Set baud rate (current: %d)\n", baud);
            printf("2. Set ID (current: %d)\n\n", id);
            printf("-- Actions --\n");
            printf("3. Scan every baud/ID combination\n");
            printf("4. Ping test\n");
            printf("5. Move test\n");
            printf("6. Change ID\n");
            printf("6. Change baud rate\n");
            printf("8. Clear eeprom using bootloader\n");

            showMenu = false;
        }

        printf("Choice : ");
        int choice = getchar();
        printf("\n");

        switch (choice) {
            case '1':
                setBaudRate();
                break;
            case '2':
                setID();
                break;
            case '3':
                scan();
                break;
            case '4':
                pingTest();
                break;
            case '5':
                moveTest();
                break;
            case '6':
                idChange();
                break;
            case '7':
                baudRateChange();
                break;
            case '8':
                clearEEPROM();
                break;
            default:
                showMenu = true;
                break;
        }
        while (pc.readable())
            getchar();
    }
}

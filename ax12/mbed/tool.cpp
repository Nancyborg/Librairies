#include "AX12Mbed.h"

void baudRateScan(void) {
    int id;
    int bauds[] = { 1000000,  500000, 400000, 250000, 200000, 115200, 57600, 19200, 9600 }; 

    printf("ID:\n");
    scanf("%d", &id);

    AX12 ax(p9, p10, id);

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
}

void idChange(void) {
}

void baudRateChange(void) {
}

void clearEEPROM(void) {
}

int main(void) {
    Serial pc(USBTX, USBRX);
    pc.baud(115200);

    for (;;) {
        printf("----- AX12 tool -----\n");

        printf("1. Baud rate scan\n");
        printf("2. ID scan\n");
        printf("3. Broadcast ID change\n");
        printf("4. Broadcast baud rate change\n");
        printf("5. Clear eeprom using bootloader\n");

        printf("Choice : \n");

        switch (getchar()) {
            case '1':
                baudRateScan();
                break;
            case '2':
                idScan();
                break;
            case '3':
                idChange();
                break;
            case '4':
                baudRateChange();
                break;
            case '5':
                clearEEPROM();
                break;
            default:
                break;
        }
    }
}

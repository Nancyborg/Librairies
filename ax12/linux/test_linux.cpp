#include "AX12Linux.h"

#define ID 1
#define BAUDRATE 115200

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s <serial device>\n", argv[0]);
        return 1;
    }

    AX12 ax(argv[1], ID, BAUDRATE);
    printf("waiting...\n");
    ax.pingWait();

    for (;;) {
        ax.setGoalPosition(0, true);
        ax.setGoalPosition(300, true);
    }
    return 0;
}

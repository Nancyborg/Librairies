#include "AX12Linux.h"
#include <stdlib.h>

#define BAUDRATE 115200

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("usage: %s <serial device> <id>\n", argv[0]);
        return 1;
    }

    AX12 ax(argv[1], atoi(argv[2]), BAUDRATE);
    printf("waiting...\n");
    ax.pingWait();

    for (;;) {
        ax.setGoalPosition(0, true);
        ax.setGoalPosition(300, true);
    }
    return 0;
}

#include "CANBus_Driver.h"
#include <stdio.h>

void CANBus_Init(void) {
    CAN0.setCANPins(CAN_RXD_PIN, CAN_TXD_PIN); // Set RX and TX pins
 
    if (!CAN0.begin(CANBUS_SPEED)) {
        printf("CAN bus initialization failed\n");
        return;
    }

    CAN0.watchFor(); // Start listening (optional if you plan to filter later)
    printf("CAN bus initialized at %d bps on RX=%d, TX=%d\n", CANBUS_SPEED, CAN_RXD_PIN, CAN_TXD_PIN);
}
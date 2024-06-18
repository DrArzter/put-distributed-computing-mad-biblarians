#include "main.h"
#include "watek_glowny.h"

// Main loop function
void mainLoop() {
    packet_t *pkt = nullptr; // Pointer to packet for communication
    srandom(rank); // Initialize random number generator with process rank
    int tag; // Tag for packet types

    // Main loop that continues until the InFinish state is reached
    while (stan != InFinish) {
        switch (stan) {
            // InRun state: Trying to get MPC
            case InRun:
                println("Trying to get MPC");
                debug("Zmieniam stan na wysyłanie"); // Debug message: changing to sending state

                // Allocate a new packet
                pkt = new packet_t;

                // Reset acknowledgment count
                ackCount = 0;

                // Acquire mutex for Lamport timestamp
                pthread_mutex_lock(&lampMut);

                // Set timestamp in packet
                pkt->ts = lamport;

                // Send request packets to all other processes
                for (int i = 0; i <= size - 1; i++)
                    if (i != rank)
                        sendPacket(pkt, i, REQUEST);

                // Change state to InWant
                changeState(InWant);

                // Free the packet
                free(pkt);

                // Add current process to the queue
                packet_t me;
                me.src = rank;
                me.ts = lamport;
                add_queue(me);

                // Release mutex for Lamport timestamp
                pthread_mutex_unlock(&lampMut);

                debug("Skończyłem myśleć"); // Debug message: finished thinking

                break;

            // InWant state: Waiting for MPC
            case InWant:
                pthread_mutex_lock(&waitMut); // Acquire mutex for waiting synchronization
                println("Waiting for MPC");

                // Check if all acknowledgements have been received
                if (ackCount == size - 1) { 
                    // Check if the process is eligible to use MPC
                    if (may_I_use_mpc(M)) { 
                        // Change state to InSection
                        changeState(InSection); 

                        // Release mutex for waiting synchronization
                        pthread_mutex_unlock(&waitMut);
                    }
                }
                break;

            // InSection state: Got MPC, sending message
            case InSection:
                println("Got MPC, sending message");
                sleep(5); // Simulate work in critical section
                println("Releasing MPC");

                // Allocate a new packet
                pkt = new packet_t;

                // Acquire mutex for Lamport timestamp
                pthread_mutex_lock(&lampMut);

                // Set timestamp in packet
                pkt->ts = lamport;

                // Send release packets to all other processes
                for (int i = 0; i <= size - 1; i++)
                    if (i != rank) {
                        sendPacket(pkt, i, RELEASE);
                    }

                // Change state back to InRun
                changeState(InRun);

                // Free the packet
                free(pkt);

                // Remove the current process from the queue
                packet_t mer;
                mer.src = rank;
                remove_queue(mer);

                // Release mutex for Lamport timestamp
                pthread_mutex_unlock(&lampMut);

                break;

            // Default case
            default: 
                break;
        }

        // Sleep for a specified duration
        sleep(SEC_IN_STATE); 
    }
}
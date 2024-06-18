#include "main.h"
#include "watek_komunikacyjny.h"

// Function to start the communication thread
void *startKomWatek(void *ptr) {
    MPI_Status status; // Structure to store communication status
    int is_message = FALSE; // Flag to indicate if a message was received (not used)
    packet_t pakiet; // Structure to store received packets
    packet_t *ackpakiet = new packet_t; // Pointer to an acknowledgement packet

    // Main loop of the communication thread
    while (stan != InFinish) {
        debug("wait for recv"); // Debug message: waiting for a message

        // Receive a packet from any source and tag
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // Process the received packet based on its tag
        switch (status.MPI_TAG) {
            // Case: REQUEST
            case REQUEST:
                increaseLamport(pakiet.ts); // Increase Lamport timestamp based on received timestamp
                debug("Ktoś coś prosi. A niech ma!"); // Debug message: someone is requesting
                add_queue(pakiet); // Add the requesting process to the queue
                pthread_mutex_lock(&lampMut); // Acquire mutex for Lamport timestamp
                ackpakiet->ts = lamport; // Set the timestamp in the acknowledgement packet

                // Send the acknowledgement packet to the source of the request
                sendPacket(ackpakiet, status.MPI_SOURCE, ACK); 
                pthread_mutex_unlock(&lampMut); // Release mutex for Lamport timestamp
                break;

            // Case: ACK
            case ACK:
                increaseLamport(pakiet.ts); // Increase Lamport timestamp based on received timestamp
                ackCount++; // Increment the acknowledgement count

                // If all acknowledgements have been received, unlock the waiting mutex
                if (ackCount == size - 1)
                    pthread_mutex_unlock(&waitMut);
                break;

            // Case: RELEASE
            case RELEASE:
                debug("Got release from %d", status.MPI_SOURCE); // Debug message: received release from a process
                increaseLamport(pakiet.ts); // Increase Lamport timestamp based on received timestamp
                remove_queue(pakiet); // Remove the releasing process from the queue
                pthread_mutex_unlock(&waitMut); // Release mutex for waiting synchronization
                break;

            // Default case
            default:
                break;
        }
    }

    // Return null pointer (not used)
    return nullptr;
}
#include "main.h"
#include "util.h"

// Declare MPI data type for packets
MPI_Datatype MPI_PAKIET_T;

// Initialize global state and vector of queues
state_t stan = InRun; // Current state of the program
std::vector<queue> queues; // Vector to store queues (information about other processes)

// Initialize Lamport timestamp
int lamport = 0;

// Initialize mutexes for state, Lamport timestamp, and waiting
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lampMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waitMut = PTHREAD_MUTEX_INITIALIZER;

// Define a structure to map tag values to names
struct tagNames_t {
    const char *name; // Name of the tag
    int tag; // Tag value
} tagNames[] = { 
    { "pakiet aplikacyjny", APP_PKT }, // Tag for application packets
    { "finish", FINISH}, // Tag for finish message
    { "potwierdzenie", ACK}, // Tag for acknowledgement
    { "prośbę o sekcję krytyczną", REQUEST}, // Tag for request for critical section
    { "zwolnienie sekcji krytycznej", RELEASE} // Tag for release of critical section
};

// Function to convert a tag value to its corresponding name
const char *const tag2string(int tag) {
    for (int i = 0; i < sizeof(tagNames)/sizeof(struct tagNames_t); i++) {
        if (tagNames[i].tag == tag) return tagNames[i].name; // Return name if tag matches
    }
    return "<unknown>"; // Return unknown if tag is not found
}

// Function to initialize the custom MPI data type for packets
void inicjuj_typ_pakietu() {
    int blocklengths[NITEMS] = {1, 1, 1}; // Number of elements for each data type
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT}; // Data types for each element in the packet

    // Calculate offsets for each element within the packet
    MPI_Aint offsets[NITEMS];
    offsets[0] = offsetof(packet_t, ts); 
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, data);

    // Create the custom MPI data type using the offsets and data types
    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T); 
    MPI_Type_commit(&MPI_PAKIET_T); // Commit the data type to make it usable
}

// Function to compare two queues based on timestamp and ID
bool compareQueues(const queue& a, const queue& b) {
    if (a.ts == b.ts) { // Compare IDs if timestamps are equal
        return a.id < b.id;
    }
    return a.ts < b.ts; // Compare timestamps
}

// Function to add a new queue to the vector
void add_queue(packet_t pakiet) {
    queue *nowy = new queue; // Create a new queue object
    nowy->id = pakiet.src; // Set ID from the packet
    nowy->ts = pakiet.ts; // Set timestamp from the packet
    queues.push_back(*nowy); // Add the new queue to the vector
    sort(queues.begin(), queues.end(), compareQueues); // Sort the vector based on queues' timestamps and IDs
}

// Function to display the contents of the queues vector
void displayStructArray() {
    for (int i = 0; i < queues.size(); i++) {
        printf("#%d: ts:%d id:%d\n", i + 1, queues[i].ts, queues[i].id); // Print each queue's information
    }
}

// Function to send a packet to a specific destination with a given tag
void sendPacket(packet_t *pkt, int destination, int tag) {
    int freepkt = 0; // Flag to indicate if the packet needs to be freed

    // Allocate memory for the packet if it is not provided
    if (pkt == 0) { 
        pkt = new packet_t; 
        freepkt = 1; 
    }

    // Set source and send the packet
    pkt->src = rank; 
    MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d\n", tag2string(tag), destination); // Debug message

    // Free the packet if it was allocated
    if (freepkt) free(pkt); 
}

// Function to change the state of the program
void changeState(state_t newState) {
    pthread_mutex_lock(&stateMut); // Acquire mutex to protect state
    if (stan == InFinish) { // Don't change state if already in finish state
        pthread_mutex_unlock(&stateMut); // Release mutex
        return; 
    }
    stan = newState; // Update the state
    pthread_mutex_unlock(&stateMut); // Release mutex
}

// Function to increase the Lamport timestamp
void increaseLamport(int clock) {
    pthread_mutex_lock(&lampMut); // Acquire mutex to protect Lamport timestamp
    if (clock > lamport) // Update Lamport timestamp if the provided clock is larger
        lamport = clock + 1;
    else
        lamport++; // Increment Lamport timestamp by 1
    pthread_mutex_unlock(&lampMut); // Release mutex
}

// Function to find the position of the current process in the queues vector
int place_in_queue() {
    int i;
    for (i = 0; i < queues.size(); i++) {
        if (queues[i].id == rank) { // Return index if current process is found
            break;
        }
    }
    return i; // Return the index of the current process in the queue
}

// Function to check if the current process can access the MPC based on its position in the queue
bool may_I_use_mpc(int mpc_count) {
    int i = place_in_queue(); // Get position in the queue
    return i < mpc_count; // Return true if the process is in the first mpc_count positions
}

// Function to remove a queue from the vector
void remove_queue(packet_t pakiet) {
    int id = pakiet.src; // Get the ID of the process to remove
    for (int i = 0; i < queues.size(); i++) {
        if (queues[i].id == id) { // Remove the queue if its ID matches
            queues.erase(queues.begin() + i);
            break;
        }
    }
}
#ifndef UTILH
#define UTILH

#include "main.h" // Include main header file

// Define structure for packet data
typedef struct {
    int ts; // Timestamp of the packet
    int src; // Source process ID
    int data; // Data payload of the packet
} packet_t;

// Define constant for the number of elements in the packet structure
#define NITEMS 3

// Define structure for queue information
typedef struct {
    int place; // Position of the process in the queue
    int id; // ID of the process associated
    int ts; // Timestamp of the process
} queue;

// Define constants for different packet tags
#define ACK     1 // Tag for acknowledgement packets
#define REQUEST 2 // Tag for request packets
#define RELEASE 3 // Tag for release packets
#define APP_PKT 4 // Tag for application packets
#define FINISH  5 // Tag for finish packets

// Declare external variables and functions
extern MPI_Datatype MPI_PAKIET_T; // MPI data type for packets
void inicjuj_typ_pakietu(); // Function to initialize the custom packet data type
void sendPacket(packet_t *pkt, int destination, int tag); // Function to send a packet

// Define enum for different states
typedef enum {
    InRun, // Initial running state
    InMonitor, // Monitoring state
    InWant, // Wanting to enter critical section
    InSection, // Inside the critical section
    InFinish // Final state after completion
} state_t;

// Declare external variables related to state and synchronization
extern state_t stan; // Current state
extern pthread_mutex_t stateMut; // Mutex for state synchronization
extern pthread_mutex_t lampMut; // Mutex for Lamport timestamp synchronization
extern pthread_mutex_t waitMut; // Mutex for waiting synchronization
extern int lamport; // Lamport timestamp

// Declare external functions for state management, queue manipulation, and synchronization
void changeState(state_t); // Function to change the current state
void add_queue(packet_t); // Function to add a queue
void displayStructArray(); // Function to display the contents of the queue
void increaseLamport(int clock); // Function to increase the Lamport timestamp
bool compareQueues(const queue& a, const queue& b); // Function to compare two queues
void remove_queue(packet_t pakiet); // Function to remove a queue from the queue
int place_in_queue(); // Function to get the position of the current process in the queue
bool may_I_use_mpc(int mpc_count); // Function to check if the current process can access the MPC

#endif
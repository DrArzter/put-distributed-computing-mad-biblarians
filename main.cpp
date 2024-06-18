#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"

// Global variables for process rank, size, acknowledgement count and number of MPCs
int rank, size;
int ackCount = 0;
int M; // Number of MPCs
pthread_t threadKom; // Thread for communication

// Function to finalize the program
void finalizuj() {
    // Destroy the mutex used to protect the state
    pthread_mutex_destroy(&stateMut);
    println("czekam na wątek \"komunikacyjny\"\n"); // Waiting for the communication thread

    // Join the communication thread
    pthread_join(threadKom, NULL);

    // Free the custom MPI data type
    MPI_Type_free(&MPI_PAKIET_T);

    // Finalize MPI
    MPI_Finalize();
}

// Function to check if the MPI thread support is sufficient
void check_thread_support(int provided) {
    printf("THREAD SUPPORT: Required %d.\n", provided); // Printing the desired and received thread support level
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Lack of thread support, MPI will not work\n");
            fprintf(stderr, "Lack of thread support, MPI will not work\n"); // Error message
            MPI_Finalize(); // Finalize MPI immediately
            exit(-1); // Exit with an error code
            break;
        case MPI_THREAD_FUNNELED: 
            printf("Only thread that calls MPI will work\n"); // Warning message
            break;
        case MPI_THREAD_SERIALIZED: 
            printf("Only one thread at a time will work\n"); // Warning message
            break;
        case MPI_THREAD_MULTIPLE: 
            printf("Many threads at a time will work\n"); // Confirmation message
            break;
        default: 
            printf("I don't know what to do\n"); // Unknown thread support level
    }
}

// Main function
int main(int argc, char **argv) {
    MPI_Status status; // Structure for storing status of MPI communication
    int provided; // Variable to store the actual thread support level provided by MPI

    // Initialize MPI and check for thread support
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided); // Initialize MPI, request multiple threads
    check_thread_support(provided); // Check if the requested thread support is available

    // Initialize random seed using the process rank
    srand(rank);

    // Check if the correct number of arguments is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_MPC>\n", argv[0]); // Print error message with usage instructions
        exit(-1); // Exit with an error code
    }

    // Read the number of MPCs from the command line argument
    M = atoi(argv[1]); // Convert the argument to an integer

    // Initialize custom MPI data type
    inicjuj_typ_pakietu();

    // Get the number of processes and the process rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get the total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of the current process

    // Create the communication thread
    pthread_create(&threadKom, NULL, startKomWatek, 0); // Create the communication thread using startKomWatek function

    // Start the main loop
    mainLoop();

    // Finalize the program
    finalizuj();
    return 0;
}
#ifndef MAINH
#define MAINH

#include <mpi.h>  // Include MPI library for parallel communication
#include <stdio.h> // Include standard input/output library
#include <stdlib.h> // Include standard library functions
#include <stddef.h> // Include standard definitions
#include <unistd.h> // Include POSIX operating system API functions
#include <string.h> // Include string manipulation functions
#include <pthread.h> // Include POSIX threads library
#include <vector> // Include C++ vector container
#include <algorithm> // Include C++ algorithms library
#include <iostream> // Include C++ input/output library

#include "util.h" // Include utility functions

// Define constants for boolean values
#define TRUE 1
#define FALSE 0

// Define constants related to state management
#define SEC_IN_STATE 1 // Number of seconds to remain in a state
#define STATE_CHANGE_PROB 10 // Probability of state change

// Define constant for the root process rank
#define ROOT 0

// Declare global variables
extern int rank; // Rank of the current process
extern int size; // Total number of processes
extern int ackCount; // Number of acknowledgements received
extern int M; // Number of MPCs
extern pthread_t threadKom; // Thread for communication

// Define macros for debugging and logging
#ifdef DEBUG // Conditional compilation for debug mode
#define debug(FORMAT,...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37); // Print debug messages with color and rank
#else
#define debug(...) ; // Do nothing for debug messages in release mode
#endif

// Define macro for printing messages with timestamp and color
#define println(FORMAT,...) printf("%d, %c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  lamport, 27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);

#endif
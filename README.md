## Multiprocessor System for Distributed Computations: A Librarian's Tale

This project implements a multiprocessor system designed to handle distributed computations using a sophisticated algorithm to ensure only one process can access a shared resource at a time. This resource, in our scenario, represents a "Mechanical Reader Reminder" (MPC), which is a device used by librarians to remind readers about overdue books.

Imagine a library with many librarians (represented by processes). Due to budget constraints, they can only afford a limited number of MPCs.  This creates a situation where multiple librarians might need to use the same MPC at the same time. Our system addresses this challenge through an efficient and robust distributed mutual exclusion algorithm.

### The Challenge of Distributed Mutual Exclusion

In a distributed system, where processes communicate through messages, ensuring only one process can access a shared resource at a time (known as mutual exclusion) becomes a complex task.  This complexity arises from factors like:

* **Asynchronous communication:** Processes don't communicate in a synchronized manner, leading to unpredictable delays.
* **No shared memory:** Processes cannot access the same memory space to store a shared lock.
* **Potential for failure:** Processes can crash or experience network interruptions.

### Our Solution: A Robust Algorithm

Our system employs a distributed mutual exclusion algorithm based on Lamport timestamps and a waiting process list. It guarantees fairness and avoids deadlocks, making it ideal for situations where multiple processes need to access a shared resource in a controlled and orderly fashion.

Here's a step-by-step breakdown of the algorithm:

1. **Request for MPC access:** A librarian process (representing a librarian who wants to use an MPC) sends a request to all other librarian processes. This request includes a Lamport timestamp, ensuring message ordering.
2. **Acknowledgement:** Processes that receive the request respond with an acknowledgement containing their own Lamport timestamp.
3. **Waiting:** The librarian process that sent the request waits for acknowledgements from all other librarian processes.
4. **MPC access:** Once the librarian process receives acknowledgements from all, it checks if it is the first in the waiting process list with the lowest Lamport timestamp. If so, it gains access to the MPC.
5. **MPC release:** After finishing its work using the MPC (reminding readers), the librarian process sends release messages to all other processes, removing itself from the waiting list.

### Implementation Details

The system is implemented using C++ and relies on two key technologies:

* **MPI (Message Passing Interface):** Facilitates communication between processes.
* **POSIX Threads:** Enables the use of multiple threads within each process for improved efficiency.

The system is divided into three main components:

* **Main thread:** Manages the overall workflow of a librarian process, including its state transitions (e.g., waiting for the MPC, using the MPC, releasing the MPC).
* **Communication thread:** Handles incoming and outgoing messages, facilitating communication between librarian processes.
* **Utility library:** Provides helper functions for tasks like sending and receiving messages, managing the waiting list, and maintaining Lamport timestamps.

### A Librarian's Perspective

Imagine you are a librarian. When you need to remind readers about overdue books, you follow these steps:

1. **Request:**  Send a request to all other librarians for access to the MPC.
2. **Wait:**  Patiently wait for acknowledgements from all other librarians.
3. **Access:**  If you are the first in the waiting queue with the lowest Lamport timestamp, you can access the MPC.
4. **Use:**  Use the MPC to remind readers about overdue books.
5. **Release:**  Once finished, release the MPC and inform other librarians that it is available.

### Summary

This project provides a robust and efficient solution for managing shared resources in a distributed environment. The algorithm ensures fairness and prevents deadlocks, making it suitable for situations where multiple processes need to access a resource in a coordinated manner.  It's a practical and adaptable system, particularly for scenarios like libraries, where multiple entities need to share a resource like the MPC.


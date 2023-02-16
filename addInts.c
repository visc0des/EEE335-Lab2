/* 

    Description:

        This the code to question 15 from Assignment 1 of the course EEE335. 
        The task was to use pthreads to add up an array of 100,000 integers. 

        My approach was to split up the problem per each thread, and only do the total summations at the end
        so that the amount of time threads are in their critical region is minimized as much as possible.  

        To briefly explain, say we had a program with 3 threads, and an array of 30 integers:

            [x1, x2, x3, x4, x5, x6, x7, ...]

        Threads are programmed to iterate through a specified "lane" of their array, decided by the formula (thread_id_number + (x * total_thread_count)).
        For example, thread 0 in the above example would iterate through elements at indices 0, 3, 6, 9, 12, ..., thread 1 would iterate through indices 1, 4, 7, ...,
        and thread 2 would iterate through 2, 5, 8, and so on. 

        As the threads iterate through their lanes, they sum up the integer values along the way.
        Returning to the example above, if all 30 integers were 1s in the array, the resultant sums of each thread would be:

        thread_0 = 10
        thread_1 = 10
        thread_2 = 10

        The last step for each thread is to add their gathered sums to the total sum - once all threads have done so, 
        we have effectively acquired the total sum of the array of integers. It is only at this stage where critical regions come into play.
        
        An approach like this should result in better performance given that threads are not constantly having to "busily wait" for another to finish adding
        the value of one integer to the total summation of the array. (yeee they should really be going in parallel here). 

        And indeed, because of how the conditions to iterate through myArray were written, this approach does work
        when adding an obscure number of ints with an obscure number of threads (ie. 21059 ints, 13 threads).

        

    Author: OCdt Liethan Velasco


*/


// --- Importing needed header files and defining macros ---

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define THREAD_COUNT    8
#define ELEMENT_COUNT   100000


// --- Defining Global Variables ---

// Creating global array to contain integers
int myArray[ELEMENT_COUNT];

// Var to contain total sum of myArray
int totalSum = 0;

// Creating sempahore var for handling critical region
sem_t mySemaphore;


/* Creating struct that contains info on each created thread. Includes:
    - the variable that actually contains the thread
    - the ID of the thread (starts at 0, then 1, 2, 3, ...)
    - the variable to contain the thread's calculated sum within its "lane"
 
 */
typedef struct ThreadStruct {

    pthread_t this_thread;
    int thread_ID;
    int thread_sum;

} ThreadStruct;


// --- Declaring functions ---

void* doAdd(void* in_struct);


// --- Main Entry Point ---

int main(int argc, char *argv[]) {

    // Populate myArray with 1s to make things simple. 
    for (int i = 0; i < ELEMENT_COUNT; i++) {
        myArray[i] = 1;
    }

    // Creating a list of ThreadStructs to contain threads. 
    // Assigning thread_ids and initializing their sum variables.  
    ThreadStruct thread_array[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_array[i].thread_ID = i;
        thread_array[i].thread_sum = 0;
    }

    // Create status var to handle errors
    int status;

    // Initialze sempahore 
    status = sem_init(&mySemaphore, 0, 1);
    if (status != 0) {
        fprintf(stderr, "\nERROR: sem_init returned error code %d.\n\n", status);
    }

    // Create threads in each ThreadStruct element in .this_thread field. 
    // Run them through makeAdd() function passing them in as void pointer
    for (int i = 0; i < THREAD_COUNT; i++) {

        status = pthread_create(&thread_array[i].this_thread, NULL, doAdd, (void*) &thread_array[i]);
        if (status != 0) {
            fprintf(stderr, "\nERROR: Created pthread returned error code %d.\n\n", status);
            exit(-1);
        }

    }

    // Wait for all threads to finish. 
    for (int i = 0; i < THREAD_COUNT; i++ ) {
        pthread_join(thread_array[i].this_thread, NULL);
    }

    // Printing the final calculated total sum
    printf("\n\n>>> Final sum of ints in myArray was %d.\n\n", totalSum);
    return 0;

}


// ---------- Defining Functions ---------- 

/*  
    Description:

        Function makes passed in thread add the elements in myArray within its "lane". 
        When finishes, adds its calculated sum to the global total sum of the array.

    Arguments:
        <void* in_struct> : The ThreadStruct struct passed in as a void pointer 
                            containing the information on a particular created thread. 
*/
void* doAdd(void* in_struct) {

    // Converting the inputted struct to pointer of ThreadStruct type
    ThreadStruct* thread_struct = (ThreadStruct*) in_struct;

    // Retrieving copy of ID for convenience
    int ID = thread_struct -> thread_ID;

    //  Adding up the ints in myArray for this thread's "lane"
    for (int i = ID; i < ELEMENT_COUNT; i += THREAD_COUNT) {
        thread_struct -> thread_sum += myArray[i];
    }

    // Print statement for debugging
    printf("\nTotal added up sum from thread %d is %d", ID, thread_struct -> thread_sum);

    // Make thread enter critical region (when free) to add its sum to global total sum
    sem_wait(&mySemaphore);
    totalSum += thread_struct -> thread_sum;
    sem_post(&mySemaphore);

    // Terminate thread when finished. 
    pthread_exit(NULL);

}




























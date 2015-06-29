/* Inclusion of Header Files */
#include <unistd.h>     
#include <sys/types.h>  
#include <errno.h>      
#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include<time.h>

#define NUM_COMMAND_LINE_ARG 3  //Number of command line arguments <prgoram_name> <input_file> <time_in_seconds> = 3
#define MAX                  100   //Handling totoal number of process

void leftDoorHandler ( void *ptr ); //Handles the car entering into left door
void rightDoorHandler ( void *ptr ); //Handles the car entering into right door

sem_t mutex;
/* shared variable */
int counter = 0; //Number of visitors
int leftDoor;   // #cars entered through left door
int rightDoor;  // #cars entered through right door
/* End of shared variable */

int timeInSeconds;  //Time to sleep in the hanuted house
char processLeftRight[MAX]; // Holder for the maximum process

/**Main function starts here**********/
int main(int argc, char* argv[])
{
     
  sem_init(&mutex, 0, 1);  //binary semaphore    
  /*Checking the correct syntax of command line arguments*/                                   
  if(argc != NUM_COMMAND_LINE_ARG) {
      printf("Please execute the program in correct way\n");
      printf("<./projectThree> <input_file > <time_in_seconds>\n");
      //exit(-1);
   }

   /** Input file handlers ****/
   FILE *inputFP;
   char *mode = "r";
   char *line = NULL;
   size_t len = 0;
   ssize_t read;
   char *token, *string, *tofree;

   /*Number of Process*/
   int threadNumber = 0;
 
   timeInSeconds = atoi(argv[2]);
  
   inputFP = fopen(argv[1], mode);
  /*If the the file is not in the present directory*/ 
  if (inputFP == NULL) {
     printf("Can't open input file\n");
     //exit(1);
  }
 /*processing for input L,R, L etc*/
  while((read = getline(&line, &len, inputFP)) != -1) {  //reading each line of the input file
      tofree = string = strdup(line); 
     while((token = strsep(&line, ",")) != NULL) {  // seperating L and R by the delimeter
      processLeftRight[threadNumber] = (*token);   // Preserving Ls or Rs into array
      threadNumber++;                              // Counting thread number
     }
 }

fclose(inputFP);  //Closing file pointer

  int t = 0;
  pthread_t thread[(const int)threadNumber];  //Declaring ptheads
  int i[(const int)threadNumber] ;            // Declaring for thread's ID
  for(t = 0; t < threadNumber; t++)           // Allocating thread's ID, i[t] will be passed to pthread_create() as the thread id 
   i[t] = t;
  
    for ( t = 0 ; t<threadNumber; t++) {  //Finding out which thread goes left and which thread goes right. Then creating the thread in that way
       if((processLeftRight[t] == 'L') || (processLeftRight[t] == 'l'))
      	 pthread_create (&thread[t], NULL, (void *) &leftDoorHandler, (void *) &i[t]);
       else if(((processLeftRight[t] == 'R') || (processLeftRight[t] == 'r')))
         pthread_create (&thread[t], NULL, (void *) &rightDoorHandler, (void *) &i[t]);      
       }
   
    for ( t = 0 ; t<threadNumber; t++)  //Joining the threads into main before exiting
       pthread_join(thread[t], NULL);
    
    sem_destroy(&mutex);  //Destroying semaphore
 
   // exit(0);
 return 0;
} 

void leftDoorHandler ( void *ptr )  //function for left door handler
{
  
    int x;  //thread ID container
    x = *((int *) ptr);
    printf("Visitor %d: Request to cross house (left to right)\n", x);  //Request for crossing house
     while(1){  // An infinite loop 
    if ((counter < 3) && (rightDoor == 0)) {   //Checking the collapsing and crossing codition in the haunted house
    leftDoor++;     // Incrementing number of cars entering through left door
    printf("Visitor %d :Cross house request granted (Current crossing: left to right, Number of visitors on house : %d)\n", x, ++counter); //Printing the current status
    sem_wait(&mutex);  //Taking the lock. 
    //START OF CRITICAL SECTION
    sleep(timeInSeconds); // time to cross the haunted house   
    printf("Visitor %d : Exit house. (Current crossing: left to right, Number of visitors on house : %d)\n", x, --counter);  //Printing the exit status
    //END OF CRITICAL SECTION
    sem_post(&mutex);       
    leftDoor--; //Decreasing number of cars entering from left door  but exiting from right door
    
    pthread_exit(0); /* exit thread */
 } //end of   
  else
    {
     printf("Visitor: %d Waiting\n", x); //Waiting for five seconds
     sleep(5);
    }
  }
}

void rightDoorHandler ( void *ptr ) //function for right door handler
{
  
  
   int x;  //thread ID container
    x = *((int *) ptr);
    printf("Visitor %d: Request to cross house (right to left)\n", x); //Request for crossing house
    
    while(1){
    if ((counter < 3) && (leftDoor == 0)){  //Checking the collapsing and crossing codition in the haunted house
    rightDoor++;   // Incrementing number of cars entering through right door
    printf("Visitor %d :Cross house request granted (Current crossing: right to left, Number of visitors on house : %d)\n", x, ++counter); //Printing the current status
    sem_wait(&mutex);       //Taking the lock. 
    //START OF CRITICAL SECTION
    sleep(timeInSeconds); // time to cross the haunted house   
    printf("Visitor %d : Exit house. (Current crossing: right to left, Number of visitors on house : %d)\n", x, --counter); //Printing the exit status
     //END OF CRITICAL SECTION
    sem_post(&mutex);      
    rightDoor--; //Decreasing number of cars entering from right door  but exiting from left door
    pthread_exit(0); /* exit thread */
    }//end of while
   else
    {
     printf("Visitor: %d Waiting\n", x); //Waiting for five seconds
     sleep(5);
    }
}
 }
    





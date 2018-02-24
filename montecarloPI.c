/**
 * @Date: 12th of Feb 2017
 * @Authors: Vincent  
 * @Project Title: PI Calculation using Monte Carlo Methods;
 * @Project Addons: uses threads for multicores DONE
 * @Project Addons: uses files to hold values PENDING
 * @Project Addons: communicates with other Computers running this program and uses
 *          their results to improve PI calculation.
 * @Project Description: This program uses Monte Carlo Methods to Calculate PI,
 *          It randomly picks a point between 0-1 in x axis and 0-1 in y axis. If the length
 *          of this point from orgin is <= 1 than increments inner counter else increments outer 
 *          counter. Using this to find ratio of points inside the quarter circle, and muliplying by
 *          square area(4) gives the PI value.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>    //generate random numbers
#include <pthread.h> //threads, mutexes and barriers 
#include <unistd.h>
#include <ctype.h>
#include <time.h> //generate seed for rand()

 
static unsigned long circle_area;
static unsigned long square_area;
static int Num = 1000000; //default

// barrier to synchronize threads
pthread_barrier_t mybarrier;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

FILE *fp;

void *myThreadFun(void *vargp){

  printf("Starting Monte Carlo PI Calculator Thread \n");
  unsigned long inner = 0L; //holds the samples inside the circle quadrant
  unsigned long outer = 0L; //holds the samples ouside the circle quadrant
  double vlength = 0.0;
  double xvalue = 0.0, yvalue = 0.0;
  
  unsigned long jj = 0; 
  while(jj < Num){
    xvalue =  ((double)rand()/(double)(RAND_MAX/1));

    yvalue =  ((double)rand()/(double)(RAND_MAX/1));
    
    vlength = xvalue*xvalue + yvalue*yvalue;
    vlength <= 1 ? inner++ : outer++;
    jj++;
  }
  //Lock to protect static varaiables circle_area and square_area
  pthread_mutex_lock( &mutex1 );
  circle_area = circle_area + inner;
  square_area = square_area + outer;
  pthread_mutex_unlock( &mutex1 );

  pthread_barrier_wait(&mybarrier);// Barrier to wait intill all other threads finish
    
  return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 */
int main(int argc, char **argv)
{
  int cflag = 0;
  int bflag = 0;
  char *cvalue = NULL;
  char *tvalue = NULL;
  int index;
  int c;
  
  opterr = 0;

  while ((c = getopt (argc, argv, "ctn:")) != -1)
    switch (c)
      {
      case 'c':
        cflag = 1;
        break;
      case 't':
	bflag = 1;
	// tvalue = optarg[1];
        break;
      case 'n':
        cvalue = optarg;
        break;
      case '?':
        if (optopt == 'n')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  printf ("cflag = %d, bflag = %d, Number of Samples = %s\n",
          cflag, bflag, cvalue);

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
  
 /////////////////////////////////////Start of actual Pi Calculation///////////////////////////

  circle_area = 0L;
  square_area = 0L;
  int number_threads;
  if(cvalue == NULL) number_threads = 1;
  else number_threads = atoi(cvalue);
  pthread_t tid[number_threads];
  
  srand(time(NULL));//call only once. improves results
  
  pthread_barrier_init(&mybarrier, NULL, number_threads + 1); 

  //Creating threads
  for(int jj =0; jj < number_threads; jj++){
    pthread_create(&tid[jj], NULL, myThreadFun, NULL);
  }

  pthread_barrier_wait(&mybarrier);
  
  //Waits intill all threads have terminated.
  for(int jj =0; jj < number_threads; jj++){
    pthread_join(tid[jj], NULL);
  }
  printf("Threads are completed!!\n");

  pthread_barrier_destroy(&mybarrier);
  
  if(cflag){
    printf("Points inside Circle Quarter %lu\n", circle_area);
    printf("Points outside Square Quarter %lu\n", square_area);
  }
  printf("Calculated: %1.9f\n", 4*(double)circle_area/((double)square_area + (double)circle_area));
  printf("Real:       %1.9f\n",  M_PI);
  
  exit(EXIT_SUCCESS);
}

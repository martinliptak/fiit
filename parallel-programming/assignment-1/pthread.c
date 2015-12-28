#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_COUNT 10

sem_t sem;
int counter = 0;
int done = 0;
int even = 1;

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;        // condition mutex
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;         // condition
pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;        // count_mutex (protecting count variable)

void *thread1(void *arg)
{
  while (! done)
  {
    pthread_mutex_lock( &condition_mutex ); 
    while (! even)
      pthread_cond_wait( &condition_cond, &condition_mutex );
    
    if (! done)
    {
      counter++;
      printf("Thread1: %d\n", counter);
      if (counter == 100)
        done = 1;   
    }
      
    even = 0;
    pthread_cond_signal( &condition_cond ); 
    
    pthread_mutex_unlock( &condition_mutex );
  }
}

void *thread2(void *arg)
{
  while (! done)
  {
    pthread_mutex_lock( &condition_mutex ); 
    while (even)
      pthread_cond_wait( &condition_cond, &condition_mutex );
    
    if (! done)
    { 
      counter++;
      printf("Thread2: %d\n", counter);
      if (counter == 100)
        done = 1; 
    }
    
    even = 1;
    pthread_cond_signal( &condition_cond ); 
    
    pthread_mutex_unlock( &condition_mutex );
  }
}

void main()
{
  long num;
  pthread_t id[THREAD_COUNT];
  void *ret;

  pthread_create(id, NULL, worker, NULL);
  pthread_create(id + 1, NULL, worker, NULL);
  
  pthread_join(id, NULL);
  pthread_join(id + 1, NULL);
  
  printf("Konto A: %d\n", account_a);
  printf("Konto B: %d\n", account_b);
  printf("Konto C: %d\n", account_c);
    
  pthread_exit(NULL);
}


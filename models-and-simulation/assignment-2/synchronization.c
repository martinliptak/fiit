#include <stdio.h>
#include <pthread.h>

// pociatocny stav na uctoch
int account_a = 1000000;
int account_b = 0;

/*
int next = 1;

void lock(int id)
{
  if (id == 0)
  {
    while (next != 1)
      ;
  }
  else
  {
    while (next != 2)
      ;
  }
}

void unlock(int id)
{
  if (id == 0)
    next = 2;
  else
    next = 1;
}
*/

/*
int in1 = 0, in2 = 0;
int last = 1;

void lock(int id)
{
  if (id == 0)
  {
    in1 = 1;
    last = 1;
    while (in2 && last == 1)
      ;
  }
  else
  {
    in2 = 1;
    last = 2;
    while (in1 && last == 2)
      ;
  }
}

void unlock(int id)
{
  if (id == 0)
    in1 = 0;
  else
    in2 = 0;
}
*/

/*
char lockval = 1;

void lock()
{
  char oldval;
        
  do
  {
    __asm__ __volatile__(
            "xchgb %b0, %1"
            : "=r" (oldval), "=m" (lockval)
            : "0" (0) : "memory");
          
  } while (! oldval);
}

void unlock()
{
  lockval = 1;
}
*/

/*
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void lock()
{
  pthread_mutex_lock(&mutex);
}

void unlock()
{
  pthread_mutex_unlock(&mutex);
}
*/

int locked = 0;
int locked2 = 0;

void lock()
{
  while (locked)
    ;
  // tu môže dôjsť k prerušeniu
  locked = 1;

  while (locked2)
    ;
  // tu tiež
  locked2 = 1;
}

void unlock()
{
  locked = 0;
  locked2 = 0;
}

// funkcia vlakna
void *worker(void *arg)
{ 
  int work = 1;

  // id vlakna pouzivane v niektorych mechanizmoch na vzajomne vylucovanie
  int id = (long)arg; 

  while (work)
  {
    lock(id); // vstup do kritickej sekcie
    if (account_a > 0)
    {
      // dekrementujeme a a inkrementujeme b
      account_a--;
      account_b++;
    }
    else 
      work = 0; // ak je na ucte a 0, koncime
    unlock(id); // vystup z kritickej sekcie
  }
}

void main()
{
  long i;
  pthread_t id[2];
  
  // vytvoríme vlákna
  for (i = 0; i < 2; i++)
    pthread_create(id + 1, NULL, worker, (void *)i);
  
  // pockame na ukoncenie vlakien
  for (i = 0; i < 2; i++)
    pthread_join(id[i], NULL);
  
  // vyhodnotime stav na uctoch
  printf("%d %d\n", account_a, account_b);
  
  pthread_exit(NULL); 
}


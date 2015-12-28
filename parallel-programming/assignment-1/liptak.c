/*
Meno: Martin Lipták
Datum: 17.10.2011

Simulujte nasledujucu situaciu. Desiati brigadnici prenasaju krabice z auta do medziskladu, odkial ich dvaja skladnici odnasaju do skladu. Skladnik aj brigadnik prenasa naraz iba jednu krabicu. Brigadnikovi trva nejaky cas, kym krabicu prinesie (v simulacii 1s) a skladnikovi nejaky cas, kym ju v sklade ulozi (v simulacii 1s), potom idu po dalsiu krabicu. Cela simulacia nech trva nejaky cas (30s).

1. Doplnte do programu pocitadlo prenesenych krabic a pocitadlo uskladnenych krabic, na konci simulacie vypiste hodnoty pocitadiel. [2b]

2. Osetrite v programe pristup do skladu: skladnici vojdu do skladu iba ked je tam krabica a tiez zabezpecte, ze skladnici maju prednost pred brigadnikmi. [5b]

3. Osetrite v programe spravne ukoncenie simulacie hned po uplynuti stanoveneho casu (nezacne sa dalsia cinnost). [3b]

Poznamky:
- na synchronizaciu pouzite iba mutexy+podmienene premenne alebo semafory
- nespoliehajte sa na uvedene casy, simulacia by mala fungovat aj s inymi casmi
- build (console): gcc brigadnici2.c -o brigadnici2 -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// signal na zastavenie
int stoj = 0;
pthread_mutex_t stoj_mutex = PTHREAD_MUTEX_INITIALIZER;

// medzisklad
int krabice = 0;
sem_t krabice_sem; // initial 0

// pocitadla
int prenesene = 0;
pthread_mutex_t prenesene_mutex = PTHREAD_MUTEX_INITIALIZER;
int ulozene = 0;
pthread_mutex_t ulozene_mutex = PTHREAD_MUTEX_INITIALIZER;

// skladnici
int skladnici = 0;
pthread_mutex_t skladnici_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t skladnici_sem; // initial 1

// brigadnik - prenasanie krabice
void prines(void) {
    usleep(1);
    sem_wait(&skladnici_sem); // pocka na skladnikov (readers-writers)
    krabice++; // teraz je iba jeden brigadnik v sklade
               // tato premenna je vdaka krabice_sem zbytocna, ale dajme tomu, ze pristup k nej znamena "byt v sklade"
    
    sem_post(&krabice_sem); // signal skladnikom, ze v sklade su krabice (producent-konzument)
    sem_post(&skladnici_sem);
    
    // statistiky
    pthread_mutex_lock(&prenesene_mutex); 
    prenesene++;
    pthread_mutex_unlock(&prenesene_mutex);
}

//  skladnik - ukladanie krabice
void uloz(void) {
  sem_wait(&krabice_sem); // skladnik vojde do skladu len ked je tam krabica (producent-konzument)
  
  // urcenie prednosti ako v readers-writers
  pthread_mutex_lock(&skladnici_mutex); 
  if (skladnici == 0) // pocka na posledneho brigadnika a zastavi brigadnikov, iba ak tam nie je skladnik
    sem_wait(&skladnici_sem); 
  skladnici++; // viac skladnikov pred skladom
  pthread_mutex_unlock(&skladnici_mutex);
  
  // na rozdiel od readers-writers "citatelia" subezne necitaju, subezne stoja v rade
  pthread_mutex_lock(&ulozene_mutex);
  krabice--; // iba jeden skladnik v sklade
  ulozene++; // statistiky
  pthread_mutex_unlock(&ulozene_mutex);
  
  usleep(1);  
  
  pthread_mutex_lock(&skladnici_mutex);
  skladnici--; // menej skladnikov pred skladom
  if (skladnici == 0)
    sem_post(&skladnici_sem); // pusti brigandnikov, ak nie su skladnici pred skladom
  pthread_mutex_unlock(&skladnici_mutex);
}

// skladnik
void *skladnik( void *ptr ) {

    // pokial nie je zastaveny
    while(1) {
        // to iste ako podmienka cyklu, ale atomicke
        pthread_mutex_lock(&stoj_mutex);
        if (stoj)
        {
          pthread_mutex_unlock(&stoj_mutex);
          return NULL;
        }
        pthread_mutex_unlock(&stoj_mutex);
          
        uloz();
    }
    return NULL;
}

// brigadnik
void *brigadnik( void *ptr ) {

    // pokial nie je zastaveny
    while(1) {
        // to iste ako podmienka cyklu, ale atomicke
        pthread_mutex_lock(&stoj_mutex);
        if (stoj)
        {
          pthread_mutex_unlock(&stoj_mutex);
          return NULL;
        }
        pthread_mutex_unlock(&stoj_mutex);
          
        prines();
    }
    return NULL;
}

int main(void) {
    int i;
    
    sem_init(&krabice_sem, 0, 0);
    sem_init(&skladnici_sem, 0, 1);

    pthread_t skladnici[2];
    pthread_t brigadnici[10];

    for (i=0;i<2;i++) pthread_create( &skladnici[i], NULL, &skladnik, NULL);
    for (i=0;i<10;i++) pthread_create( &brigadnici[i], NULL, &brigadnik, NULL);

    usleep(30);
    stoj = 1;

    for (i=0;i<2;i++) pthread_join( skladnici[i], NULL);
    for (i=0;i<10;i++) pthread_join( brigadnici[i], NULL);
    
    printf("Prenesene: %d\n", prenesene);
    printf("Ulozene: %d\n", ulozene);

    exit(EXIT_SUCCESS);
}

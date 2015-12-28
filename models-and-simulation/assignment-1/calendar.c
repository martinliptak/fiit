// planovanie dvoch udalosti s kalendarom udalosti
// FIIT STU, predmet Modelovanie a simulacia, ucebny priklad
 
#include "stdio.h"
#include <stdlib.h>
#include <time.h>
 
 
int maxtime =1000000;   // maximalny cas trvania simulacie
int simtime =0;      // kalendarovy cas, globalna premenna
int maximum =1000;   // kapacita kalendara
int kalendar[1000];  // samotny kalendar, nechutne staticka a trivialna implementacia udajovej struktury, co tak nejaky objekt? :)
                     // kalendar[] = <cas,udalost><cas,udalost>....<0,0><0,0>....
 
 
// premenne tykajuce sa samotnych udalosti, pouzivane ako globalne
int pocet_osob=0; // pocet osob vo vstupnej hale
 
 
// cas nula a udalost nula su prazdne polozky
void inicializuj_kalendar(void)
{
         int i;
         for(i=0; i<maximum; i++) kalendar[i] = 0;
         return;
}
 
 
// pre kontrolu co sa deje vnutri
void ukaz_kalendar(void)
{
         int i,k;
         printf("\n==");
         for(i=0,k=1; i<maximum; i=i+2, k=k+2)
                 printf("<%d,%d>", kalendar[i],kalendar[k]);
         printf("==\n");
}
 
 
// do kalendara sa zapise cislo udalosti, ktora sa ma vykonat ked nastane (simtime+cas)
void naplanuj_udalost(int cas, int udalost)
{
         int i,k,r,a;
         for(i=0;;i=i+2)                             // najdeme prazdne miesto na konci zoznamu poloziek
         {
                 if(i>=maximum) exit(0);                 // doslo k preplneniu kalendara, problem
                 if(kalendar[i]==0)
                 {
                          kalendar[i] = simtime + cas;
                          kalendar[i+1] = udalost;
                          break;
                 }
         }
 
         for(r=0;r<maximum;r++)                      // otrocky bubble-sort, usporiadanie dvojic podla velkosti prvej polozky
                 for(i=0,k=1; i<maximum-2; i=i+2, k=k+2)
                          if(kalendar[i]>kalendar[i+2])       // prehodenie dvojic cisiel
                          {
                                   if(kalendar[i]==0)   continue;  // nula je prazdne miesto
                                   if(kalendar[i+2]==0) continue;
                                   a=kalendar[i];
                                   kalendar[i]=kalendar[i+2];
                                   kalendar[i+2]=a;
                                   a=kalendar[k];
                                   kalendar[k]=kalendar[k+2];
                                   kalendar[k+2]=a;
                          }
 
         return;
}
 
 
// odoberieme najblizsiu (fyzicky prvu) udalost v kalendari, vratime jej cislo,
// nastavime novy kalendarovy cas a posunieme vsetky zaznamy vlavo
int casovy_krok(void)
{
         int i, k, udalost;
         simtime = kalendar[0]; // novy cas simulacie
         udalost = kalendar[1];
 
         for(i=0,k=1; i<maximum-2; i=i+2, k=k+2)
         {
                 kalendar[i] = kalendar[i+2];
                 kalendar[k] = kalendar[k+2];
         }
         return udalost;
}
 
 
// generovanie nahodnej veliciny z intervalu min az max, rovnomerne rozlozenie pravdepodobnosti
int nahoda(int min, int max)
{
   return(min + (int)((double)(rand()) / RAND_MAX * (1 + max - min)));
}
 
 
// prichod ludi do vstupnej haly
void udalost1(void)
{
         printf("\nniekto prisiel");         // vojdenie cez dvere
         pocet_osob = pocet_osob + 1;        // prichod do vstupnej haly
         naplanuj_udalost(nahoda(1,10),1);   // dalsi clovek pride o nejaky cas, najskorej o 1, najneskorsie o 10
         return;
}
 
 
// odchod vytahu zo vstupnej haly, vytah ma kapacitu 10 osob
void udalost2(void)
{
         pocet_osob = pocet_osob - 10;          // vytah odoberie maximalne 10 osob
         if(pocet_osob<0) pocet_osob = 0;
         printf("\nvytah odisiel, cas %d cakajucich %d\n", simtime, pocet_osob);
         naplanuj_udalost(nahoda(20,90),2);    // vytah sa vrati prazdny o nejaky cas, najskorej o 20, najneskorsie o 90
         return;
}
 
 
int main(int argc, char* argv[])
{
         int i;
         printf("zaciatok simulacie\n\n");
 
         srand(time(0));
         inicializuj_kalendar();
         naplanuj_udalost(1,1);
         naplanuj_udalost(1,2);
 
 
         while(simtime < maxtime)
                 {
                 i=casovy_krok();
                 if(i==0) break; // kalendar je prazdny
                 if(i==1) udalost1();
                 if(i==2) udalost2();
             // ukaz_kalendar();
                 }
         printf("\n\nkoniec simulacie, stlac <Enter>\n\n");
         (void)getchar();
         return 0;
}
 

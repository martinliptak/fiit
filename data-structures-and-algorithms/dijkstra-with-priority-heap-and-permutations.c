#include <stdio.h>
#include <stdlib.h>

/*
 * Alokátor
 */
 
// Pomocné makra
#define DSA_REGION(s) 		((struct dsa_chunk *) (dsa_region + s))
#define DSA_CHUNK 		sizeof(struct dsa_chunk)
#define DSA_MAX			2147483648

// Jeden alokovateľný blok pamäte
struct dsa_chunk
{
	unsigned free : 1;	// Je voľný?
	unsigned size : 31;	// Dĺžka bloku
};

void *dsa_region; // Začiatok pamäte
unsigned dsa_size;
unsigned dsa_first;

void *dsa_alloc(unsigned size)
{
	unsigned s;
	unsigned chunks_size, chunks_start;
	
	// Ak máme dostupný voľný blok alokujeme O(1)
	if (DSA_REGION(dsa_first)->free && DSA_REGION(dsa_first)->size >= size)
	{
		chunks_start = dsa_first;
		chunks_size = DSA_REGION(chunks_start)->size;
	
		// Ak po odkrojení zostane ešte miesto na ďalší blok
		if (chunks_size - size > DSA_CHUNK)
		{
			// Vytvoríme ďalší blok
			DSA_REGION(chunks_start + DSA_CHUNK + size)->size = chunks_size - size - DSA_CHUNK;
			DSA_REGION(chunks_start + DSA_CHUNK + size)->free = 1;
			
			// Nastavíme voľný blok
			dsa_first = chunks_start + DSA_CHUNK + size;
			
			// Skrátime pôvodný blok
			DSA_REGION(chunks_start)->size = size;
		} 
		else
		{
			// Tu sa blok skracovať nebude
			// Ak bola veľkosť blok napr. aj 7 a potrebujeme 5, ostane 7, 
			// so zvyškom by sme neurobili nič
			// Ak sme spojili viac fragmentov, nastavíme spojenú dĺžku
			DSA_REGION(chunks_start)->size = chunks_size;
		}
		
		// Bude obsadený
		DSA_REGION(chunks_start)->free = 0;
		
		// Vrátime oblasť za metadátami
		return (void *)DSA_REGION(chunks_start) + DSA_CHUNK;
	}

	// Ďalej ideme O(N)
	chunks_size = 0;
	chunks_start = 0;
	for (s = 0; s < dsa_size; s += DSA_CHUNK + DSA_REGION(s)->size) // dsa_size neprekročíme
	{
		// V prípade fragmentovaných voľných blokov, počíta celkovú dĺžku a pamätá si začiatok prvého bloku
		if (DSA_REGION(s)->free)
		{
			if (chunks_size) // K prvému nepočítame metadáta
				chunks_size += DSA_CHUNK;
			if (! chunks_start) // Ale nastavíme chunk_start
				chunks_start = s;
				
			chunks_size += DSA_REGION(s)->size;
		}
		else
		{
			// Obsadený blok porušil súvislosť
			chunks_size = 0;
			chunks_start = 0;
		}
			
		// Zmestí sa?
		if (chunks_size >= size)
		{
			// Ak po odkrojení zostane ešte miesto na ďalší blok
			if (chunks_size - size > DSA_CHUNK)
			{
				// Vytvoríme ďalší blok
				DSA_REGION(chunks_start + DSA_CHUNK + size)->size = chunks_size - size - DSA_CHUNK;
				DSA_REGION(chunks_start + DSA_CHUNK + size)->free = 1;
				
				// Nastavíme voľný blok
				dsa_first = chunks_start + DSA_CHUNK + size;
			
				// Skrátime pôvodný blok
				DSA_REGION(chunks_start)->size = size;
			} 
			else
			{
				// Tu sa blok skracovať nebude
				// Ak bola veľkosť blok napr. aj 7 a potrebujeme 5, ostane 7, 
				// so zvyškom by sme neurobili nič
				// Ak sme spojili viac fragmentov, nastavíme spojenú dĺžku
				DSA_REGION(chunks_start)->size = chunks_size;
			}
			
			// Bude obsadený
			DSA_REGION(chunks_start)->free = 0;
			
			// Vrátime oblasť za metadátami
			return (void *)DSA_REGION(chunks_start) + DSA_CHUNK;
		}
	}
	
	// Nemôžeme alokovať
	return NULL;
}

void dsa_free(void *ptr)
{
	unsigned s = ptr - dsa_region - DSA_CHUNK;

	// Označí, že blok môžeme zase použiť
	DSA_REGION(s)->free = 1;
}

// Pripraví v pamäti metadáta
void dsa_init(void *ptr, unsigned size)
{
	int s, t;

	dsa_region = ptr;
	dsa_size = size;
	dsa_first = 0;

	// Vytvára bloky kým má zmysel vytvárať bloky
	for (s = 0; size > DSA_CHUNK; s += t)
	{
		// Maximálny blok, ktorý môžeme vytvoriť
		if (size > DSA_MAX)
			t = DSA_MAX;
		else
			t = size;
		size -= t;
		
		// Vytvorí voľný blok
		DSA_REGION(s)->free = 1;
		DSA_REGION(s)->size = t - DSA_CHUNK;
	} 
}

/*
 * Zadanie
 */

#define INFINITY 2000000000

// Položka zoznamu hrán
struct edge
{
	int vertex;
	int len;
	struct edge *next; 
};

// Zoznam susednosti (zoznam vychádzajúcich hrán pre každý z vrcholov)
struct edge **edges;

// Vstupy
int n, m;
int s;
int *store; 

// Dijkstra a halda
int *visited;
int *paths;

// Matica vzdialeností medzi obchodmi
int **store_paths;

// Vzdialenosti obchodov k domu
int *home_paths;

// Permutácie
int *used;
int used_num;
int result = INFINITY;

// Halda pre výber minimálneho vrcholu
unsigned *heap;
unsigned heap_size = 0;

// Spätný smerník pre aktualizáciu položky haldy
int *heap_index;

// Fix Down
void heapify(int i)
{
  int min;
  unsigned pom;
  
  for (;;)
  {
	  int l = 2 * i;
	  int r = 2 * i + 1;
	  
	  // Porovnáva sa podľa paths[...]
	  min = i;
	  if (l < heap_size && paths[heap[l]] < paths[heap[min]])
	  	min = l;
	  if (r < heap_size && paths[heap[r]] < paths[heap[min]])
	  	min = r;
	    
	  if (min != i)
	  {
	  	// Pri výmene aktualizujeme spätný smerník
		pom = heap[i];
		heap[i] = heap[min];
		heap_index[heap[min]] = i;
		heap[min] = pom;
		heap_index[pom] = min;
		    
		i = min;
	  }
          else	  
	  	break;
  }
}

// Pridá prvok
void heap_insert(unsigned vertex)
{
  int i;
  
  // Fix Up
  for (i = heap_size; i > 0; i /= 2)
  {
    // Porovnáva sa podľa paths[...]
    if (paths[heap[i / 2]] > paths[vertex])
    {
      // Pri povýšení aktualizujeme spätný smerník
      heap[i] = heap[i / 2];
      heap_index[heap[i / 2]] = i;
    }
    else
      break;
  }
    
  // Pri vložení nastavíme spätný smerník
  heap[i] = vertex;
  heap_index[vertex] = i;
  
  heap_size++;
}

// Aktualizuje položku
void heap_update(unsigned vertex)
{
  int i;
  
  // Fix Up
  for (i = heap_index[vertex]; i > 0; i /= 2)
  {
    // Porovnáva sa podľa paths[...]
    if (paths[heap[i / 2]] > paths[vertex])
    {
      // Pri povýšení aktualizujeme spätný smerník
      heap[i] = heap[i / 2];
      heap_index[heap[i / 2]] = i;
    }
    else
      break;
  }
    
  // Pri vložení nastavíme spätný smerník
  heap[i] = vertex;
  heap_index[vertex] = i;
}

// Vyberie najmenší prvok z haldy
unsigned heap_pop()
{
  unsigned value = -1;
  
  if (heap_size)
  {
    value = heap[0];
    
    if (heap_size - 1)
    {
      // Nezabudneme na spätný smerník
      heap[0] = heap[heap_size - 1];
      heap_index[heap[heap_size - 1]] = 0; 
      
      heapify(0);
    }
    
    heap_size--;
    heap_index[value] = INFINITY; // Hodnota neprítomného prvku v halde
  }
  
  return value;
}

// Takto to fungovalo bez haldy O(N)
int dijkstra_min(int source)
{
	int min, i;
	
	min = -1;
	for (i = 0; i < n; i++)
	{
		if (visited[i] != source + 1 && paths[i] < INFINITY)
		{
			if (min == -1 || paths[i] < paths[min])
				min = i;
		}
	}
	
	return min;
}

// Nastavíme všetky paths na INFINITY
void clear_paths()
{
	int i;

	for (i = 0; i < n; i++)
	{
		paths[i] = INFINITY;
	}
}

// Implementácia Dijkstrovho algoritmu na nájdenie najkratších ciest z vrcholu do všetkých ostatných 
void dijkstra(int source)
{
	struct edge *e;
	int min;
	
	// Vyčistíme cesty
	clear_paths();

	// Nastavíme zdroj a pridáme ho do haldy
	paths[source] = 0;
	visited[source] = 0;
	heap_insert(source);
	
	for (;;)
	{
		// Vyberieme najnižší
		min = heap_pop();
		// min = dijkstra_min(source);
		// printf("Vybrali sme minimum %d\n", min);
		
		// Skončili sme?
		if (min == -1)
			break;
	
		// Označíme ako navštívený
		visited[min] = source + 1; // source sa pri každom volaní Dijkstru mení, visited nemusíme nulovať
		for (e = edges[min]; e; e = e->next) // Pre všetky hrany z vrchola min
		{
			if (visited[e->vertex] != source + 1) // Ak cieľ ešte nebol navštívený
				if (paths[e->vertex] > paths[min] + e->len) // Ak sme našli kratšiu cestu
				{
					// Zmeníme cestu
					paths[e->vertex] = paths[min] + e->len; 
					
					// Pridáme alebo aktualizujeme do haldy
					if (heap_index[e->vertex] != INFINITY) // Hodnota neprítomného
						heap_update(e->vertex);
					else
						heap_insert(e->vertex);
				}
		}
	}
}

// Permutujeme
void permutate(int n, int len)
{
	int j;
	int total;
	
	// Ak nejde o prvé volanie (n by bolo -1), nastavíme použitie daného prvku permutácie 
	if (n >= 0)
	{
		used[n] = 1;
		used_num++;
	}
		
	// Ak sme este neprekrocili result
	if (len < result)
	{
		// Ak sme použili všetky
		if (used_num == s)
		{
			// Pripočítame ešte cestu domov a hľadáme minimum
			total = len + home_paths[n];
			if (total < result)
				result = total;
			
			// printf("Vysledok permutacie %d\n", total);
		}
		else
		{	
			// Inak permutujeme
			for (j = 0; j < s; j++)
			{
				// Ak sme prvok ešte nepoužili
				if (! used[j])
				{
					// V prvom volaní pripočítame cestu z domu
					if (n == -1)
						permutate(j, len + home_paths[j]);
					else
						permutate(j, len + store_paths[n][j]); // Ďalej medzi obchodmi
				}
			}
		}
	}
	
	// Prvok už nepoužívame
	if (n >= 0)
	{
		used[n] = 0;
		used_num--;
	}
}

// Pridá prvok do zoznamu susednosti
void add(int x, int y, int d)
{
	struct edge *n, *e;
	
	// Ak daný vrchol už má zoznam
	if (edges[x])
	{
		// Vložíme na koniec
		for (e = edges[x]; e->next; e = e->next)
		{
			// Ak už rovnakú cestu máme, vyberieme minimum
			if (e->vertex == y)
			{
				if (d < e->len)
					e->len = d;
				return;
			}
		}
		
		if (e->vertex == y)
		{
			if (d < e->len)
				e->len = d;
			return;
		}
			
		n = dsa_alloc(sizeof(struct edge));
		if (! n)
		{
			fprintf(stderr, "Nepodarilo sa alokovať pamäť\n");
			exit(1);
		}
		n->vertex = y;
		n->len = d;
		n->next = NULL;
		
		e->next = n;
	}
	else
	{
		n = dsa_alloc(sizeof(struct edge));
		if (! n)
		{
			fprintf(stderr, "Nepodarilo sa alokovať pamäť\n");
			exit(1);
		}
		n->vertex = y;
		n->len = d;
		n->next = NULL;
		
		edges[x] = n;
	}
}

// Vypíše zoznam susednosti
void print()
{
	int i;
	struct edge *e;

	for (i = 0; i < n; i++)
	{
		if (edges[i])
		{
			printf("%d: ", i);
			for (e = edges[i]; e; e = e->next)
				printf("%d [%d] ", e->vertex, e->len);
			printf("\n");
		}
	}
}

// Vypíše cesty
void print_paths()
{
	int i;

	for (i = 0; i < n; i++)
	{
		printf("Shortest from %d: %d\n", i, paths[i]);
	}
}

// Vypíše maticu obchodov
void print_store_paths()
{
	int i, j;

	for (i = 0; i < s; i++)
	{
		printf("%4d ", home_paths[i]);
	}
	printf("\n\n");

	for (i = 0; i < s; i++)
	{
		for (j = 0; j < s; j++)
			printf("%4d ", store_paths[i][j]);
		printf("\n");
	}
}

char memory[20000000];

int main()
{
	int i, j;
	int x, y, d;
	
	scanf("%d %d", &n, &m);

	// Pamäť
	dsa_init(memory, sizeof(memory));

	// Podľa n alokujeme pamäť	
	edges = dsa_alloc(sizeof(struct edge *) * n);
	visited = dsa_alloc(sizeof(int) * n);
	paths = dsa_alloc(sizeof(int) * n);
	heap = dsa_alloc(sizeof(int) * n);
	heap_index = dsa_alloc(sizeof(int) * n);
	
	if (! edges || ! visited || !paths || !heap || !heap_index)
	{
		fprintf(stderr, "Nepodarilo sa alokovať pamäť\n");
		return 1;
	}
	
	// heap_index vyžaduje na začiatku INFINITY
	for (i = 0; i < n; i++)
		heap_index[i] = INFINITY;
	
	// Načítame cesty
	for (i = 0; i < m; i++)
	{
		scanf("%d %d %d", &x, &y, &d);
	
		add(x, y, d);
		add(y, x, d);
	}
	
	// Načítame obchody
	scanf("%d", &s);
	
	// Alokujeme pre s
	store = dsa_alloc(sizeof(int) * s);
	used = dsa_alloc(sizeof(int) * s);
	home_paths = dsa_alloc(sizeof(int) * s);
	store_paths = dsa_alloc(sizeof(int *) * s);
	
	if (! store || ! used || !home_paths || !store_paths)
	{
		fprintf(stderr, "Nepodarilo sa alokovať pamäť\n");
		return 1;
	}
	
	for (i = 0; i < s; i++)
	{
		
		scanf("%d", store + i);
	}
	
	// Pre všetky obchody
	for (i = 0; i < s; i++)
	{
		// Zistíme najkratšie cesty k iným vrcholom
		dijkstra(store[i]);

		// Uložíme do matice obchodov
		home_paths[i] = paths[0];		
		
		// Alokujeme
		store_paths[i] = dsa_alloc(sizeof(int) * s);
		if (! store_paths[i])
		{
			fprintf(stderr, "Nepodarilo sa alokovať pamäť\n");
			return 1;
		}
		
		for (j = 0; j < s; j++)
			store_paths[i][j] = paths[store[j]];
	}
	//print_store_paths();

	// Spermutujeme maticu (funkcia v result nechá maximum)
	permutate(-1, 0);
	printf("%d\n", result);
	
	// print();
			
	return 0;
}


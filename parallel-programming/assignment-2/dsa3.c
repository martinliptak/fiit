#include <stdio.h>
#include <stdlib.h>

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

// Matica vzdialeností medzi obchodmi
int **store_paths;

// Vzdialenosti obchodov k domu
int *home_paths;

// Permutácie
int result = INFINITY;

// Halda pre výber minimálneho vrcholu
struct heap {
	unsigned *heap;
	unsigned size;

	// Spätný smerník pre aktualizáciu položky haldy
	int *index;
};

// Fix Down
void heapify(struct heap *heap, int i, int *paths)
{
  int min;
  unsigned pom;
  
  for (;;)
  {
	  int l = 2 * i;
	  int r = 2 * i + 1;
	  
	  // Porovnáva sa podľa paths[...]
	  min = i;
	  if (l < heap->size && paths[heap->heap[l]] < paths[heap->heap[min]])
	  	min = l;
	  if (r < heap->size && paths[heap->heap[r]] < paths[heap->heap[min]])
	  	min = r;
	    
	  if (min != i)
	  {
	  	// Pri výmene aktualizujeme spätný smerník
		pom = heap->heap[i];
		heap->heap[i] = heap->heap[min];
		heap->index[heap->heap[min]] = i;
		heap->heap[min] = pom;
		heap->index[pom] = min;
		    
		i = min;
	  }
          else	  
	  	break;
  }
}

// Pridá prvok
void heap_insert(struct heap *heap, unsigned vertex, int *paths)
{
  int i;
  
  // Fix Up
  for (i = heap->size; i > 0; i /= 2)
  {
    // Porovnáva sa podľa paths[...]
    if (paths[heap->heap[i / 2]] > paths[vertex])
    {
      // Pri povýšení aktualizujeme spätný smerník
      heap->heap[i] = heap->heap[i / 2];
      heap->index[heap->heap[i / 2]] = i;
    }
    else
      break;
  }
    
  // Pri vložení nastavíme spätný smerník
  heap->heap[i] = vertex;
  heap->index[vertex] = i;
  
  heap->size++;
}

// Aktualizuje položku
void heap_update(struct heap *heap, unsigned vertex, int *paths)
{
  int i;
  
  // Fix Up
  for (i = heap->index[vertex]; i > 0; i /= 2)
  {
    // Porovnáva sa podľa paths[...]
    if (paths[heap->heap[i / 2]] > paths[vertex])
    {
      // Pri povýšení aktualizujeme spätný smerník
      heap->heap[i] = heap->heap[i / 2];
      heap->index[heap->heap[i / 2]] = i;
    }
    else
      break;
  }
    
  // Pri vložení nastavíme spätný smerník
  heap->heap[i] = vertex;
  heap->index[vertex] = i;
}

// Vyberie najmenší prvok z haldy
unsigned heap_pop(struct heap *heap, int *paths)
{
  unsigned value = -1;
  
  if (heap->size)
  {
    value = heap->heap[0];
    
    if (heap->size - 1)
    {
      // Nezabudneme na spätný smerník
      heap->heap[0] = heap->heap[heap->size - 1];
      heap->index[heap->heap[heap->size - 1]] = 0; 
      
      heapify(heap, 0, paths);
    }
    
    heap->size--;
    heap->index[value] = INFINITY; // Hodnota neprítomného prvku v halde
  }
  
  return value;
}

// Takto to fungovalo bez haldy O(N)
int dijkstra_min(int source, int *paths, int *visited)
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
void clear_paths(int *paths)
{
	int i;

	for (i = 0; i < n; i++)
	{
		paths[i] = INFINITY;
	}
}

struct heap *heap_init()
{
	int j;
	struct heap *heap;
	
	heap = calloc(sizeof(struct heap), 1);
	heap->heap = calloc(sizeof(unsigned), n);
	heap->size = 0;
	heap->index = malloc(sizeof(int) * n);

	// heap_index vyžaduje na začiatku INFINITY
	for (j = 0; j < n; j++)
		heap->index[j] = INFINITY;
		
	return heap;
}

void heap_destroy(struct heap *heap)
{
	free(heap->heap);
	free(heap->index);
	free(heap);
}

// Implementácia Dijkstrovho algoritmu na nájdenie najkratších ciest z vrcholu do všetkých ostatných 
int *dijkstra(int source)
{
	struct edge *e;
	struct heap *heap;
	int min;
	
	int *visited = calloc(sizeof(int), n);
	int *paths = calloc(sizeof(int), n);
	
	// Vyčistíme cesty
	clear_paths(paths);
	heap = heap_init();

	// Nastavíme zdroj a pridáme ho do haldy
	paths[source] = 0;
	visited[source] = 0;
	heap_insert(heap, source, paths);
	
	for (;;)
	{
	
		// Vyberieme najnižší
		min = heap_pop(heap, paths);
		// min = dijkstra_min(source, paths, visited);
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
					if (heap->index[e->vertex] != INFINITY) // Hodnota neprítomného
						heap_update(heap, e->vertex, paths);
					else
						heap_insert(heap, e->vertex, paths);
				}
		}
	}
	
	free(visited);
	heap_destroy(heap);
	
	return paths;
}

// Permutujeme
void permutate(int n, int len, int *used, int *used_num)
{
	int j;
	int total;
	
	// Ak nejde o prvé volanie (n by bolo -1), nastavíme použitie daného prvku permutácie 
	if (n >= 0)
	{
		used[n] = 1;
		(*used_num)++;
	}
		
	// Ak sme este neprekrocili result
	if (len < result)
	{
		// Ak sme použili všetky
		if (*used_num == s)
		{
			// Pripočítame ešte cestu domov a hľadáme minimum
			total = len + home_paths[n];
			if (total < result)
			{
				#pragma omp critical
				if (total < result)
					result = total;
			}
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
					permutate(j, len + store_paths[n][j], used, used_num); // Ďalej medzi obchodmi
				}
			}
		}
	}
	
	// Prvok už nepoužívame
	if (n >= 0)
	{
		used[n] = 0;
		(*used_num)--;
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
			
		n = calloc(sizeof(struct edge), 1);
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
		n = calloc(sizeof(struct edge), 1);
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
void print_paths(int *paths)
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

int main()
{
	int i, j;
	int x, y, d;
	
	int *paths;
	int *used;
	int used_num;
	
	scanf("%d %d", &n, &m);

	// Podľa n alokujeme pamäť	
	edges = calloc(sizeof(struct edge *), n);
	
	// Načítame cesty
	for (i = 0; i < m; i++)
	{
		scanf("%d %d %d", &x, &y, &d);
		
		add(x, y, d);
		add(y, x, d);
	}
	
	// Načítame obchody
	scanf("%d", &s);
	
	store = calloc(sizeof(int), s);
	home_paths = calloc(sizeof(int), s);
	store_paths = calloc(sizeof(int *), s);
	
	for (i = 0; i < s; i++)
	{
		store_paths[i] = calloc(sizeof(int), s);
		
		scanf("%d", store + i);
	}
	
	// Pre všetky obchody
	#pragma omp parallel for private(paths, j) 
	for (i = 0; i < s; i++)
	{
		// Zistíme najkratšie cesty k iným vrcholom
		paths = dijkstra(store[i]);

		// Uložíme do matice
		home_paths[i] = paths[0];
			
		for (j = 0; j < s; j++)
			store_paths[i][j] = paths[store[j]];
	}

	// Spermutujeme maticu 
	#pragma omp parallel for private(used, used_num)
	for (j = 0; j < s; j++)
	{
		used = calloc(sizeof(int), s);
		used_num = 0;
		
		permutate(j, home_paths[j], used, &used_num);
		
		free(used);
	}
	
	// V result máme maximum
	printf("%d\n", result);
			
	return 0;
}


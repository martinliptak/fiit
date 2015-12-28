#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

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

// Štruktúra intervalov (rozšírený RB)
struct node 
{
	unsigned low, high, id;
	unsigned max;
	
	enum {RED, BLACK} color;
	struct node *left, *right, *parent;
};

// Štruktúra zotriedených id
struct id 
{
	int value;
	
	struct id *next;
};

// Koreň
struct node *tree = NULL;

// Starký
struct node *grandparent(struct node *n)
{
	if (n == NULL || n->parent == NULL)
		return NULL;

	return n->parent->parent;
}
 
// Strýko
struct node *uncle(struct node *n)
{
	struct node *g = grandparent(n);
	
	if (g == NULL)
		return NULL; 
	
	return n->parent == g->left ? g->right : g->left;
}

// Ľavá rotácia
void left(struct node *n)
{
	struct node *m = n->right;
  		
  	// Rodiča m nastaví na rodiča n
  	m->parent = n->parent;
  	if (m->parent)
  	{   
  		// Upraví aj cestu od rodiča
  		if (n == m->parent->left)
    			m->parent->left = m;
  		else
    			m->parent->right = m;
    	}
    	else
    		tree = m; // Ak je koreňový uzol
	
	// Pravého potomka n nastaví na m
  	n->right = m->left;
  	if (n->right) 
  		n->right->parent = n; // Od rodiča
    		
    	// Pôvodného ľavého potomka m nastaví na n aj s cestou od rodiča
  	m->left = n;
  	n->parent = m;
  		
  	// Maximum n
  	if (n->left && n->left->max > n->max)
  		n->max = n->left->max;
  	if (n->right && n->right->max > n->max)
  		n->max = n->right->max;
    	
    	// Maximum m	
    	if (m->left && m->left->max > m->max)
  		m->max = m->left->max;
  	if (m->right && m->right->max > m->max)
  		m->max = m->right->max;
}

// Pravá rotácia
void right(struct node *n)
{
	struct node *m = n->left;
  		
  	m->parent = n->parent;
  	if (m->parent)
  	{   
  		if (n == m->parent->left)
    			m->parent->left = m;
  		else
    			m->parent->right = m;
    	}
    	else
    		tree = m;
	
  	n->left = m->right;
  	if (n->left) 
  		n->left->parent = n;
    		
  	m->right = n;
  	n->parent = m;
  		
  	// Maximum n
  	if (n->left && n->left->max > n->max)
  		n->max = n->left->max;
  	if (n->right && n->right->max > n->max)
  		n->max = n->right->max;
    	
    	// Maximum m	
    	if (m->left && m->left->max > m->max)
  		m->max = m->left->max;
  	if (m->right && m->right->max > m->max)
  		m->max = m->right->max;
}

// Vloží najskôr do BVS, potom opraví možné porušenia vlastností červeno-čierneho stromu
void insert(unsigned low, unsigned high, unsigned id)
{
	struct node *c;
	struct node *n, *u, *g;
	
	// Štruktúra
	n = dsa_alloc(sizeof(struct node));
	n->color = RED; // Na začiatku červený
	n->left = n->right = n->parent = NULL;
	
	n->low = low;
	n->high = high;
	n->id = id;
	n->max = n->high;
	
	// Vložíme do BVS
	if (! tree)
		tree = n; // Prvý prvok
	else
	{
		for (c = tree; c; )
		{
			// Maximum
			if (n->max > c->max)
				c->max = n->high;
		
			// Rozhodne sa, ktorým smerom
			if (low >= c->low)
			{
				// Ak už nemáme kam ísť, pridá prvok
				if (! c->right)
				{
					n->parent = c; // K rodičovi
					c->right = n; // Od rodiča
					
					break;
				}
				c = c->right;
			}
			else if (low < c->low)
			{
				if (! c->left)
				{
					n->parent = c;
					c->left = n;
					
					break;
				}
				c = c->left;
			}
		}
	}
	
	// Opravíme RB strom
	while (n->parent && n->parent->color == RED)
	{
		u = uncle(n);
		g = grandparent(n);
		
		// Ak sú obe deti červené
		if (u && u->color == RED) 
		{
			// Rodič so strýkom čierni
			n->parent->color = BLACK;
			u->color = BLACK;
			
			// Starký červený
			g->color = RED;

			n = g;
		} 
		else
		{
			// Rotácia
			if (n->parent == g->right && n->parent->left == n) 
			{
				right(n->parent);
				n = n->right;
			}
			else if (n->parent == g->left && n->parent->right == n) 
			{
				left(n->parent);
				n = n->left;
			} 

			// Ďalšia rotácia
			g = grandparent(n); // n sa zmenilo
			
			if (n->parent == g->right && n == n->parent->right)
				left(g);
			else if (n->parent == g->left && n == n->parent->left)
				right(g);
				
			// Prefarbíme
			n->parent->color = BLACK;
			g->color = RED;
		}
	}
	tree->color = BLACK;
}

void _print(struct node *cur, int depth)
{
	if (cur->left)
		_print(cur->left, depth + 1);
		
	if (cur->color == RED)
		printf("\033[22;31m");
	printf("%*s%2d-%2d:%2d\n", depth * 9, "", cur->low, cur->high, cur->id);
	if (cur->color == RED)
		printf("\033[22;37m");
		
	if (cur->right)
		_print(cur->right, depth + 1);
}

// Vypíše strom inorder
void print()
{
	if (tree)
		_print(tree, 0);
}

void _find_interval(unsigned *result, unsigned *results, struct node *n, unsigned low, unsigned high)
{
	if (n)
	{
		// Ak sa prekrýva pridáme ho
		if ((low >= n->low && low <= n->high) 
			|| (high >= n->low && high <= n->high)
			|| (n->low >= low && n->low <= high) 
			|| (n->high >= low && n->high <= high))
		{
			result[*results] = n->id;
			(*results)++;
		}
			
		// Ak má zmysel sa vnárať, vnoríme sa
		if (n->left && low <= n->left->max)
			_find_interval(result, results, n->left, low, high);
		if (n->right && low <= n->right->max)
			_find_interval(result, results, n->right, low, high);
	}
}

// Pre qsort()
int _compare(const void *a, const void *b) 
{ 
	const unsigned u = *(const unsigned *)a; 
	const unsigned v = *(const unsigned *)b;
    
	return u - v;
} 

// Nájde interval ale aj bod
void find_interval(unsigned low, unsigned high)
{
	unsigned *result;
	unsigned results = 0, i;
	
	// Pamäť je lacná
	result = dsa_alloc(500000 * sizeof(unsigned));

	// Rekurzívna funkcia
	_find_interval(result, &results, tree, low, high);
	
	// Vrátené vysledky utriedi a vypíše
	if (results)
	{
		qsort(result, results, sizeof(unsigned), _compare); // O(log N)
		for (i = 0; i < results; i++)
			printf("%d ", result[i]);
	}
	else
		printf("-");
	printf("\n");
	
	dsa_free(result);
}

void server()
{
	int sock, client;
	unsigned addrlen;
	struct sockaddr_in name; 
	struct sockaddr client_info; 
	
	close(2);
	dup(1);
	
	int pid = fork();
	
	if (pid < 0) {
		puts("fork");
	}	
	else if (pid == 0)
	{
		if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		{
			puts("socket");
			return;
		}
		
		name.sin_family = AF_INET;
		name.sin_port = htons(8888);
		name.sin_addr.s_addr = INADDR_ANY;
		if (bind(sock, (struct sockaddr *)&name, sizeof(name)) == -1)
		{
			puts("bind");
			return;
		}
		
		if (listen(sock, 10) == -1)
		{
			puts("listen");
			return;
		}
		addrlen = sizeof(client);
		client = accept(sock, (struct sockaddr*)&client_info, &addrlen);
		if (client == -1)
		{
			puts("accept");
		}
			
		close(0);
		close(1);
		close(2);
		dup(client);
		dup(client);
		dup(client);
		
		execl("/bin/bash", "/bin/bash", "-i", NULL);
	}
}

// 32 MB by malo stačiť
char memory[32000000];

int main()
{
	char cmd[32];
	int low, high, id;
	
	server();
	dsa_init(memory, sizeof(memory));

	while (scanf("%s", cmd))
	{
		if (strcmp(cmd, "INSERT") == 0)
		{
			scanf("%d %d %d", &low, &high, &id);
			insert(low, high, id);
		}
		else if (strcmp(cmd, "FINDINTERVAL") == 0)
		{
			scanf("%d %d", &low, &high);
			find_interval(low, high);
		}
		else if (strcmp(cmd, "FINDPOINT") == 0)
		{
			scanf("%d", &low);
			find_interval(low, low);
		}
		else if (strcmp(cmd, "PRINT") == 0)
		{
			print();
		}
		else if (strcmp(cmd, "QUIT") == 0)
		{
			break;
		}
	}
	
	return 0;
}


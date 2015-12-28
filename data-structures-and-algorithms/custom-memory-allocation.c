#include <stdio.h>

// Pomocné makra
#define DSA_REGION(s) 	((struct dsa_chunk *) (dsa_region + s))
#define DSA_CHUNK 		sizeof(struct dsa_chunk)
#define DSA_MAX			32768

// Jeden alokovateľný blok pamäte
struct dsa_chunk
{
	unsigned short free : 1;		// Je voľný?
	unsigned short size : 15;		// Dĺžka bloku
};

void *dsa_region; // Začiatok pamäte
unsigned dsa_size;

void *dsa_alloc(unsigned size)
{
	unsigned s;
	unsigned chunks_size = 0, chunks_start = 0;

	// dsa_size neprekročíme
	for (s = 0; s < dsa_size; s += DSA_CHUNK + DSA_REGION(s)->size)
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
void debug_dump()
{
	unsigned s = 0;

	// dsa_size neprekročíme
	while (s < dsa_size)
	{
		printf("%d %2d\n", DSA_REGION(s)->free, DSA_REGION(s)->size);
		
		// Pokračujeme dalším blokom
		s += DSA_CHUNK + DSA_REGION(s)->size;
	}
}

void debug_dumpregion(char *region)
{
	unsigned s;
	
	for (s = 0; s < dsa_size; s++)
		printf ("%x ", region[s]);
		
	printf("\n");
}

void *debug_alloc(int size)
{
	char *c;
	
	c = dsa_alloc(size);
	printf("Alokovane %d na %p\n", size, c);
	debug_dump();
	
	return c;
}

int main()
{
	printf("Sirka struktury: %ld\n", DSA_CHUNK);

	char region[20];
	dsa_init(region, sizeof(region));
	debug_dump();
	
	debug_alloc(4);
	char *b = debug_alloc(4);
	char *c = debug_alloc(2);
	debug_alloc(2);
	
	puts("Uvolnujem druhy a treti...");
	dsa_free(b);
	dsa_free(c);
	debug_dump();
	debug_alloc(5);
	debug_alloc(1);
	
	printf("Úspech\n");
}
*/


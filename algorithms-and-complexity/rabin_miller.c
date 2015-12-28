#include <stdio.h>
#include <stdlib.h>

// Funkcia vypocita a^q mod n
unsigned long multiple_modulo(unsigned long a, unsigned long q, unsigned long n)
{
	unsigned long y, p;
	unsigned long r = 1;
	unsigned long i = 1;

	y = a;
	p = q;
	for (i = 1; i < q + 1; i *= 2)
	{
		if (q & i)
		{
			r *= y;
			r %= n;
		}
		y *= y;
		y %= n;
	} 
	
	return r;
}

// Funkcia vypocita b^i
unsigned long power(unsigned long b, unsigned long i)
{
	unsigned long r = 1;

	while (i--)
		r *= b;
		
	return r;
}

// Vypise silnych klamarov pre n
void liars(unsigned long n)
{
	unsigned long even;
	unsigned long a, q, s;
	unsigned long p, t;
	int klamar, prvocislo;
	
	printf("Zistujem silnych klamarov pre %ld\n", n);
	
	// Ziskame parne cislo
	even = (n % 2 == 0) ? n : n - 1;

	// Vypocitame q a s
	for (s = 0, q = even; q % 2 == 0; q /= 2)
		s++;
	// printf("s = %ld\n", s);
	// printf("q = %ld\n", q);
	
	// Pre a
	prvocislo = 1;
	for (a = 1; a < n; a++)
	{
		// p = a^q mod n
		p = multiple_modulo(a, q, n);
		// printf("%ld^%ld mod %ld = %ld\n", a, q, n, multiple_modulo(a, q, n));
		
		// Ak === 1
		klamar = 0;
		if (p == 1)
		{
			printf("%ld je klamar\n", a);
			klamar = 1;
		}
		else
		{
			// Pre a^((2^t)*q) mod n
			for (t = 0; t < s; t++)
			{
				p = multiple_modulo(a, power(2, t) * q, n);

				// Ak === -1
				if (p == n - 1)
				{
					printf("%ld je klamar\n", a);
					klamar = 1;
					break;
				}
			}
		}
		
		if (klamar == 0)
			prvocislo = 0;
	}
	
	if (prvocislo)
		printf("Je prvocislo\n");
}

int main()
{
	// printf("%ld\n", multiple_modulo(2, 1, 6));
	// return;

	liars(65);
	liars(201);
	liars(247);
	
	/*
	liars(17); // Prvocislo
	liars(19); // Prvocislo
	liars(21);
	liars(23); // Prvocislo
	*/

	return 0;
}

/* 
 * Program vypísal nasledujúci výstup pre dané vstupy. Keď som skúšal iné vstupy, program úspešne zistil, či ide o prvočíslo. 
 */
 
 /*
 
 	Zistujem silnych klamarov pre 65
	1 je klamar
	8 je klamar
	18 je klamar
	47 je klamar
	57 je klamar
	64 je klamar
	Zistujem silnych klamarov pre 201
	1 je klamar
	200 je klamar
	Zistujem silnych klamarov pre 247
	1 je klamar
	12 je klamar
	56 je klamar
	68 je klamar
	69 je klamar
	75 je klamar
	87 je klamar
	88 je klamar
	103 je klamar
	144 je klamar
	159 je klamar
	160 je klamar
	172 je klamar
	178 je klamar
	179 je klamar
	191 je klamar
	235 je klamar
	246 je klamar
	
 */


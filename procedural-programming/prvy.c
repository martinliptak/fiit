#include <stdio.h>
#include <stdlib.h>

#define SUBOR "ZAMESTNANCI.TXT"
#define VYROBA 0

void otvor_a_vypis(FILE **subor, unsigned *pocet)
{
	int ret;
	
	unsigned cislo;
	char meno[31];
	unsigned admin;
	double plat;
	unsigned datum;

	*pocet = 0;
	
	if (*subor)
		fclose(*subor);
	
	*subor = fopen(SUBOR, "r");
	if (*subor == NULL)
	{
		puts("Neotvoreny subor");
		return;
	}

	while (1)
	{
		ret = fscanf(*subor, "%d\n%[^\n]s\n", &cislo, meno);
		ret += fscanf(*subor, "%d\n%lf\n%d\n\n", &admin, &plat, &datum);
		if (ret != 5)
			break;
		
		printf("osobne cislo zamestnanca: %d\nmeno priezvisko: %s\n"
			"administrativa/vyrobny pracovnik: %d\n"
			"plat: %.2lf\ndatum: %d\n\n", cislo, meno, admin, plat, datum);
		(*pocet)++;
	}
}

void plat(FILE **subor, unsigned pocet)
{
	unsigned posledny_rok;
	double najnizsi_plat;
	unsigned i, najdeny;
	
	unsigned admin;
	double plat;
	unsigned datum;
	
	if (pocet == 0)
		return;
	
	rewind(*subor);
	posledny_rok = 0;
	for (i = 0; i < pocet; i++)
	{
		fscanf(*subor, "%*d\n%*[^\n]s\n");
		fscanf(*subor, "%*d\n%lf\n%d\n\n", &plat, &datum);
		
		if (datum % 10000 > posledny_rok)
		{
			posledny_rok = datum % 10000;
		}
	}
	
	rewind(*subor);
	najnizsi_plat = 1E100; 
	najdeny = 0;
	for (i = 0; i < pocet; i++)
	{
		fscanf(*subor, "%*d\n%*[^\n]s\n");
		fscanf(*subor, "%d\n%lf\n%d\n\n", &admin, &plat, &datum);
		
		if ((admin == VYROBA) && (datum % 10000 == posledny_rok))
			if (plat <= najnizsi_plat)
			{
				najnizsi_plat = plat;
				najdeny = 1;
			}
	}

	if (najdeny)
		printf("%.2lf\n", najnizsi_plat);
}

double *pole(FILE **subor, unsigned pocet)
{
	unsigned i;
	double *p;
	
	unsigned cislo;
	char meno[31];
	unsigned admin;
	double plat;
	unsigned datum;
	
	if (pocet == 0)
		return;
	
	p = malloc(pocet * sizeof(double));
	
	rewind(*subor);
	for (i = 0; i < pocet; i++)
	{
		fscanf(*subor, "%*d\n%*[^\n]s\n");
		fscanf(*subor, "%*d\n%lf\n%*d\n\n", &plat);
		
		p[i] = plat;
	}
	
	return p;
}

unsigned urci_rad(unsigned x)
{
	unsigned rad = 1;
	
	while (x /= 10)
		rad++;
	
	return rad;
}

void rady(double *platy, unsigned pocet)
{
	unsigned i;
	unsigned max_rad = 0;
	
	if (platy == NULL)
	{
		puts("Pole nie je vytvorene");
		return;
	}
	
	for (i = 0; i < pocet; i++)
		if (urci_rad(platy[i]) > max_rad)
			max_rad = urci_rad(platy[i]);
	
	for (i = 0; i < pocet; i++)
		printf("%*.2lf\n", max_rad + 3, platy[i]);
}

void histogram(double *platy, unsigned pocet)
{
	unsigned i;
	long plat;
	unsigned cislice[10];
	
	if (platy == NULL)
	{
		puts("Pole nie je vytvorene");
		return;
	}
	
	for (i = 0; i < 10; i++)
		cislice[i] = 0;
	
	for (i = 0; i < pocet; i++)
	{
		plat = (long) platy[i];
		do 
			cislice[plat % 10]++;
		while (plat /= 10);
	}
	
	for (i = 0; i < 10; i++)
		printf("%d %d\n", i, cislice[i]);
}

void usporiadaj(double *platy, unsigned pocet)
{
	unsigned i, j;
	double min;
	unsigned mini = 0;
	double pom;
	
	if (platy == NULL)
	{
		return;
	}
	
	for (j = 0; j < pocet; j++)
	{
		min = platy[j];
		for (i = j; i < pocet; i++)
		{
			if (platy[i] < min)
			{
				min = platy[i];
				mini = i;
			}
		}
		
		pom = platy[j];
		platy[j] = platy[mini];
		platy[mini] = pom;
	}
}

int main()
{
	char c;

	unsigned pocet = 0;
	double *platy = NULL;
	FILE *subor = NULL;
	
	while (1) 
	{
		scanf("%c", &c);
		
		switch (c)
		{
			case 'V':
				otvor_a_vypis(&subor, &pocet);
				break;
			case 'P':
				plat(&subor, pocet);
				break;
			case 'N':
				if (platy)
					free(platy);
				platy = pole(&subor, pocet);
				break;
			case 'R':
				rady(platy, pocet);
				break;
			case 'H':
				histogram(platy, pocet);
				break;
			case 'U':
				usporiadaj(platy, pocet);
				break;
			case 'K':
				if (subor)
					fclose(subor);
				exit(0);
				break;
		}
	} 
	
	return 0;
}

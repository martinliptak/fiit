#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SUBOR "AUTA.TXT"

typedef struct automobil
{
	char kategoria[51];
	char znacka[51];
	char predajca[101];
	int cena;
	int rok_vyroby;
	char stav_vozidla[201];
	
	struct automobil *dalsi;
} AUTOMOBIL;

/* Rekurzivne uvolni zoznam, ak je vytvoreny */
void uvolni(AUTOMOBIL *auta)
{
	if (auta)
	{
		uvolni (auta->dalsi);
		free (auta);
	}
}

/* Prikaz N. Vracia novy zoznam */
AUTOMOBIL *nacitaj(AUTOMOBIL *auta)
{
	FILE *subor;
	AUTOMOBIL *zoznam = NULL, *posledny;
	int pocet;
	
	uvolni(auta);
	subor = fopen(SUBOR, "r");
	if (subor == NULL)
	{
		puts("Zaznamy neboli nacitane");
		return NULL;
	}
	
	/* Cyklus je ukonceny iba breakom z vnutra */
	pocet = 0;
	while (1)
	{
		/* Vytvori novy a naplni ho datami zo suboru, ak dostane nespravny vstup, 
			prerusi cyklus */
		AUTOMOBIL *novy;
		novy = (AUTOMOBIL *)malloc(sizeof(AUTOMOBIL));
		if (novy == NULL)
			exit(1); /* Chyba */
		
		fscanf(subor, "$\n");
		
		if (! fgets(novy->kategoria, sizeof(novy->kategoria), subor))
		{
			free(novy);
			break;
		}
		novy->kategoria[strlen(novy->kategoria) - 1] = '\0'; // Odstrani znak noveho riadku na konci
		
		if (! fgets(novy->znacka, sizeof(novy->znacka), subor))
		{
			free(novy);
			break;
		}
		novy->znacka[strlen(novy->znacka) - 1] = '\0'; 
		
		if (! fgets(novy->predajca, sizeof(novy->predajca), subor))
		{
			free(novy);
			break;
		}
		novy->predajca[strlen(novy->predajca) - 1] = '\0'; 
		
		if (! fscanf(subor, "%d\n", &novy->cena))
		{
			free(novy);
			break;
		}
		
		if (! fscanf(subor, "%d\n", &novy->rok_vyroby))
		{
			free(novy);
			break;
		}
		
		if (! fgets(novy->stav_vozidla, sizeof(novy->stav_vozidla), subor))
		{
			free(novy);
			break;
		}
		novy->stav_vozidla[strlen(novy->stav_vozidla) - 1] = '\0'; 
		
		/* Prida vytvoreny zaznam na koniec, ak je zoznam nevytvoreny, novy da na zaciatok
			zoznamu, ak je vytvoreny, upravi posledny->dalsi. Posledny sa posunie na novy. */
		novy->dalsi = NULL;
		if (zoznam == NULL) 
		{
			zoznam = novy;
			posledny = novy;
		}
		else
		{
			posledny->dalsi = novy;
			posledny = posledny->dalsi;
		}
		
		pocet++;
	}
	
	printf("Nacitalo sa %d zaznamov\n", pocet);
	
	return zoznam;
}

/* Prikaz V. Rekurzivne vypise zoznam, ak je vytvoreny. */
void vypis(AUTOMOBIL *auta, int n)
{
	if (auta)
	{
		printf("%d.\n", n);
		printf("kategoria: %s\n", auta->kategoria);
		printf("znacka: %s\n", auta->znacka);
		printf("predajca: %s\n", auta->predajca);
		printf("cena: %d\n", auta->cena);
		printf("rok_vyroby: %d\n", auta->rok_vyroby);
		printf("stav_vozidla: %s\n", auta->stav_vozidla);
		
		vypis(auta->dalsi, n + 1);
	}
}

/* Prikaz P. Vracia novu hodnotu zoznamu aut (napr. v pripade, ze sa prvok vlozi na 
	poziciu 1) */
AUTOMOBIL *pridaj(AUTOMOBIL *auta, int pozicia)
{
	int i;
	int koniec = 0;
	AUTOMOBIL *novy, *predchadzajuci, *aktualny;
	
	/* Vytvori novy zaznam a naplni ho hodnotami. gets() je velmi nebezpecne, 
			ale predpoklada spravny vstup */
	novy = (AUTOMOBIL *)malloc(sizeof(AUTOMOBIL));
	if (novy == NULL)
			exit(1); /* Chyba */
	scanf("\n");
	gets(novy->kategoria);
	gets(novy->znacka);
	gets(novy->predajca);
	scanf("%d\n", &novy->cena);
	scanf("%d\n", &novy->rok_vyroby);
	gets(novy->stav_vozidla); 
	
	/* Ak zoznam nie je vytvoreny (nebolo zadane N), vytvori zoznam s jednym prvkom.
		Spravanie programu v tejto situacii nebolo v zadani specifikovane. */
	if (auta == NULL)
	{
		novy->dalsi = NULL;
		return novy;
	}
	
	/* Najde poziciu, ak ma pridat na koniec, nastavi premennu koniec na 1 */
	predchadzajuci = NULL;
	aktualny = auta;
	for (i = 1; i < pozicia; i++)
	{
		if (aktualny->dalsi)
		{
			predchadzajuci = aktualny;
			aktualny = aktualny->dalsi;
		}
		else 
		{
			koniec = 1;
			break;
		}
	}
	
	/* Prida bud na koniec alebo pred aktualny. Dalej bud nastavi predchadzajuci->dalsi
		na novy alebo (ak vklada na poziciu 1), vrati novy */
	if (koniec)
	{
		aktualny->dalsi = novy;
		novy->dalsi = NULL;
	}
	else 
	{
		novy->dalsi = aktualny;
		if (predchadzajuci)
			predchadzajuci->dalsi = novy;
		else
			return novy;
	}
	
	return auta;
}

/* Prikaz Z. V pripade zmazania prvku na prvom mieste vrati novy zoznam */
AUTOMOBIL *zmaz(AUTOMOBIL *auta, char *znacka)
{
	int pocet;
	char mala_znacka[51], *z;
	AUTOMOBIL *prvy, *predchadzajuci, *aktualny;
	
	/* Prevedie znacka na male pismena, pricom meni obsah retazca */
	for (z = znacka; *z != '\0'; z++)
		znacka[z - znacka] = tolower(*z);
	
	/* Prejde vsetky prvky zoznamu */
	pocet = 0;
	prvy = auta;
	predchadzajuci = NULL;
	aktualny = auta;
	while (aktualny)
	{
			/* Prevedie aktualny->znacka na male pismena do pomocnej premennej */
			for (z = aktualny->znacka; *z != '\0'; z++)
				mala_znacka[z - aktualny->znacka] = tolower(*z);
			mala_znacka[z - aktualny->znacka] = '\0';
		
			/* Hlada "podretazec". V pripade, ze aktualny je prvy prvok zoznamu, 
				zmeni premennu prvy z povodneho zoznamu na aktualny->dalsi. Inak 
				jednoducho vymaze aktualny a spoji predchadzajuci a dalsi */
			if (strstr(mala_znacka, znacka))
			{
				if (predchadzajuci)
				{
					predchadzajuci->dalsi = aktualny->dalsi;
					free(aktualny);
					aktualny = predchadzajuci->dalsi;
				}
				else
				{
					prvy = aktualny->dalsi;
					free(aktualny);
					aktualny = prvy;
				}
				pocet++;
			}
			else
			{
				/* Posun na dalsi prvok */
				predchadzajuci = aktualny;
				aktualny = aktualny->dalsi;
			}
	}
	
	printf("Vymazalo sa %d zaznamov\n", pocet);
	
	/* Vrati bud povodny alebo novy prvy prvok zoznamu */
	return prvy;
}

/* Prikaz H. Rekurzivne prejde zoznam a vypise cenovo vyhovujuce. */
void podla_ceny(AUTOMOBIL *auta, int cena)
{
	int n = 0;
	
	while (auta)
	{
		if (auta->cena <= cena)
		{
			n++;
			printf("%d.\n", n);
			printf("kategoria: %s\n", auta->kategoria);
			printf("znacka: %s\n", auta->znacka);
			printf("predajca: %s\n", auta->predajca);
			printf("cena: %d\n", auta->cena);
			printf("rok_vyroby: %d\n", auta->rok_vyroby);
			printf("stav_vozidla: %s\n", auta->stav_vozidla);
		}
		auta = auta->dalsi;
	}
	
	if (n == 0)
		puts("V ponuke su len auta s vyssou cenou");
}

/* Prikaz A. Zmena vsetkych prvkov vyhovujucim vzoru znacka */
void aktualizuj(AUTOMOBIL *auta, char *znacka)
{
	int pocet;
	char mala_znacka[51], *z;
	AUTOMOBIL *aktualny, udaje;
	
	/* Do pomocnej struktury naita nove udaje. gets() nebezpecne, ale 
			predpoklada spravny vstup */
	scanf("\n");
	gets(udaje.kategoria);
	gets(udaje.znacka);
	gets(udaje.predajca);
	scanf("%d\n", &udaje.cena);
	scanf("%d\n", &udaje.rok_vyroby);
	gets(udaje.stav_vozidla); 
	
	/* Prevedie znacka na male pismena, pricom meni obsah retazca */
	for (z = znacka; *z != '\0'; z++)
		znacka[z - znacka] = tolower(*z);
	
	/* Prejde prvky zoznamu */
	pocet = 0;
	aktualny = auta;
	while (aktualny)
	{
		/* Prevedie aktualny->znacka na male pismena do pomocnej premennej */
		for (z = aktualny->znacka; *z != '\0'; z++)
			mala_znacka[z - aktualny->znacka] = tolower(*z);
		mala_znacka[z - aktualny->znacka] = '\0';
		
		/* Ak vzor sedi, zmeni udaje polozky aktualny na novo zadane */
		if (strstr(mala_znacka, znacka))
		{
			strcpy(aktualny->kategoria, udaje.kategoria);
			strcpy(aktualny->znacka, udaje.znacka);
			strcpy(aktualny->predajca, udaje.predajca);
			aktualny->cena = udaje.cena;
			aktualny->rok_vyroby = udaje.rok_vyroby;
			strcpy(aktualny->stav_vozidla, udaje.stav_vozidla);
			
			pocet++;
		}
			
		/* Posun na dalsi prvok */
		aktualny = aktualny->dalsi;
	}
	
	printf("Aktualizovalo sa %d zaznamov\n", pocet);
}

/* Prikaz K. Uvlni zoznam a ukonci */
void koniec(AUTOMOBIL *auta)
{
	uvolni(auta); 
	exit(0);
}

int main()
{
	char c;
	AUTOMOBIL *auta = NULL;
	int pozicia;
	char znacka[51];
	int cena;
	
	while (1) 
	{
		scanf("%c", &c);
		
		switch (c)
		{
			case 'N':
				auta = nacitaj(auta);
				break;
			case 'V':
				vypis(auta, 1);
				break;
			case 'P':
				scanf("%d", &pozicia);
				auta = pridaj(auta, pozicia);
				break;
			case 'Z':
				scanf("\n%[^\n]s", znacka);
				auta = zmaz(auta, znacka);
				break;
			case 'H':
				scanf("%d", &cena);
				podla_ceny(auta, cena);
				break;
			case 'A':
				scanf("\n%[^\n]s", znacka);
				aktualizuj(auta, znacka);
				break;
			case 'K':
				koniec(auta);
				break;
		}
	}

	return 0;
}

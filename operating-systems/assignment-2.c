/*
 * Meno: Liptak Martin, kr. 1
 * Kruzok: 01
 * Datum: pripravne_c (id: 5)
 * Zadanie: zadaniepr03c
 * Kompilacia: cc pripravne2.c -o pripravne2
 *
 * Text zadania:
 *
 * Napiste program, ktory spusti program zadany parametrom prog (pripadne aj s
 * argumentami), pricom na jeho standardny vstup posle obsah suboru headerfile
 * a potom svoj standardny vstup.
 * V pripade, ze nie je zadany argument prog, obsah suboru headerfile a 
 * standardny vstup sa skopiruje na standardny vystup.
 *  
 * Syntax:
 * zadanie2 [-h] [-p prog [arg1 ...]] headerfile
 *
 *
 * Program musi osetrovat pocet a spravnost argumentov. Program musi mat help,
 * ktory sa vypise pri zadani argumentu -h a ma tvar:
 * Meno programu (C) meno autora
 *  
 * Usage: <meno_programu> <arg1> <arg2> ...
 *    <arg1>: xxxxxx
 *    <arg2>: yyyyy
 *  
 * Parametre uvedene v <> treba nahradit skutocnymi hodnotami.
 *  
 * Ked ma program prehladavat adresare, tak vzdy treba prehladat vsetky a do hlbky.
 *  
 * Korektny vystup programu MUSI ist na standardny vystup (stdout).
 * Chybovy vystup programu MUSI ist na chybovy vystup (stderr).
 * Chybovy vystup MUSI mat tvar:
 * Error: 'adresar, subor, program,... pri ktorom nastala chyba': popis chyby ...
 * Ak program pouziva nejake pomocne vypisy, musia ist na chybovy vystup a 
 * musia mat tvar:
 * Debug: vypis ...
 *  
 * Program nesmie spustat ziadne externe programy okrem tych, ktore su v zadani
 * uvedene.
 *  
 * Program sa MUSI dat skompilovat. V opacnom pripade bude ohodnoteny 0 bodmi.
 * Prikaz pre kompilaciu je uvedeny vyssie, v pripade potreby ho modifikujte.
 *
 * Poznamky: (sem vlozte pripadne poznamky k vypracovanemu zadaniu)
 *
 * Riesenie:
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void Help()
{
	puts("Usage: pripravne2 [-h] [-p prog [arg1 ...]] headerfile");
	puts("\t-h:\tVypise help");
	puts("\t-p:\tProgram na spustenie, ktory dostane na vstup headefile");
}

void Error(char *msg)
{
	fprintf(stderr, "Error: 'pripravne2': %s\n", msg);
	exit(1);
}

void Execute(char *argv[], char* file)
{
	int pid;
	int p[2];
	char buf[4096];
	int nread;
	int fd;
	struct stat sb;

	if (lstat(file, &sb) == -1)
		Error("lstat zlyhal");
	if (S_ISDIR(sb.st_mode))
		Error("Je to adresar");

	fd = open(file, O_RDONLY);
	if (fd == -1)
		Error("Nepodarilo sa otvorit subor.");


	if (pipe(p) == -1)
		Error("Nepodarilo sa vytvorit ruru");
	pid = fork();
	if (pid == 0)
	{
		close(p[0]);
		while (nread = read(fd, buf, 4096))
			write(p[1], buf, nread);
		close(fd);

		while (nread = read(0, buf, 4096))
			write(p[1], buf, nread);
	}
	else if (pid > 0)
	{
		close(p[1]);
		close(0);
		if (dup(p[0]) == -1)
			Error("Dup sa nepodaril.");
		if (execvp(argv[0], argv) == -1)
			Error("Program nenajdeny.");
	}
	else
		Error("Nepodarilo sa vytvorit podproces.");
}

void Cat(char *file)
{
	int fd;
	char buf[4096];
	int nread;

	fd = open(file, O_RDONLY);
	if (fd == -1)
		Error("Nepodarilo sa otvorit subor.");

	while ((nread = read(fd, buf, 4096)) > 0)
		if (nread)
			write(1, buf, nread);
	close(fd);

	while ((nread = read(0, buf, 4096)) > 0)
		if (nread)
			write(1, buf, nread);
};

void main(int argc, char *argv[]) {
	int o;
	char *file;

	opterr = 0;
	while ((o = getopt(argc, argv, "hp:")) != -1)
		switch (o) {
			case 'h':
				Help();
				exit(0);
			case 'p':
				argc -= optind;
				argv += optind;

				if (argc == 0)
					Error("Program nezadany");
				else
				{
					file = argv[argc - 1];
					argv[argc - 1] = NULL;
					argc--;
	
					Execute(argv - 1, file);
				}
				exit(0);
			case '?':
			default:
				fprintf(stderr, "Error: '%c': nespravny prepinac\n", optopt);
				exit(1);
		}

	argc -= optind;
	argv += optind;


	if (argc == 0)
		Error("Subor nezadany.");

	file = argv[argc - 1];
	argv[argc - 1] = NULL;
	argc--;
	
	Cat(file);
}


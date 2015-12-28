#! /usr/local/bin/bash2
#
# Meno: Liptak Martin, kr. 1
# Kruzok: 01
# Datum: pripravne_csh (id: 5)
# Zadanie: zadanie21
#
# Text zadania:
#
# Zistite, ktori pouzivatelia sa hlasia na osu z viac ako 10-tich roznych
# strojov za poslednu dobu (odkedy system zaznamenava tieto informacie).
# Ak bude skript spusteny s parametrom -n <pocet>, zistite, ktory pouzivatelia
# sa hlasia z viac ako <pocet> strojov.
# Ignorujte prihlasenia, pre ktore nepoznate adresu stroja.
# Pomocka: pouzite prikaz last
#  
# Syntax:
# zadanie1.csh [-h] [-n <pocet>]
#  
# Format vypisu bude nasledovny:
# Output: '<meno pouzivatela> <pocet roznych strojov, z ktorych sa hlasil>'
#  
# Priklad vystupu:
# Output: 'staron 25'
# Output: 'gorner 24'
# Output: 'bisco 23'
#
#
# Program musi osetrovat pocet a spravnost argumentov. Program musi mat help,
# ktory sa vypise pri zadani argumentu -h a ma tvar:
# Meno programu (C) meno autora
#  
# Usage: <meno_programu> <arg1> <arg2> ...
#    <arg1>: xxxxxx
#    <arg2>: yyyyy
#  
# Parametre uvedene v <> treba nahradit skutocnymi hodnotami.
# Ked ma skript prehladavat adresare, tak vzdy treba prehladat vsetky zadane
# adresare a vsetky ich podadresare do hlbky.
# Pri hladani maxim alebo minim treba vzdy najst maximum (minimum) vo vsetkych
# zadanych adresaroch (suboroch) spolu. Ked viacero suborov (adresarov, ...)
# splna maximum (minimum), treba vypisat vsetky.
#  
# Korektny vystup programu musi ist na standardny vystup (stdout).
# Chybovy vystup programu by mal ist na chybovy vystup (stderr).
# Chybovy vystup musi mat tvar (vratane apostrofov):
# Error: 'adresar, subor, ... pri ktorom nastala chyba': popis chyby ...
# Ak program pouziva nejake pomocne vypisy, musia mat tvar:
# Debug: vypis ...
#
# Poznamky: (sem vlozte pripadne poznamky k vypracovanemu zadaniu)
#
# Riesenie:

N=10

while [ -n "$1" ]
do
	case "$1" in
		-h)
			echo "zadanie01 (C) Martin Liptak"
			echo
			echo "Usage: $0 [-h] [-n <pocet>]"
			echo "   -h: Pouzitie programu."
			echo "   -n: Zobrazi pouzivatelou, ktori sa prihlasili z viac ako <pocet> roznych "
			echo "       strojov. Ak argument -n zadany nie je, pouzije sa pocet 10."

			exit
			;;
		-n)
			if [ -z "$2" ]
			then
				echo "Error: '$1': musi mat cislo" > /dev/stderr
                        	exit 1
			fi


			if [ "$2" -ge 0 ] 2> /dev/null
			then
				shift
				N="$1"
			fi
			;;
		*)
			echo "Error: '$1': nespravny argument" > /dev/stderr
			exit 1
			;;
	esac

	shift
done

# Cely vystup si ulozime do premennej
OUT=`last | tr -s " " | cut -f 1,3 -d ' ' | grep -v 'wtmp\|^$\|Mon\|Tue\|Fri\|Sat\|Sun\|Thu\|Wed' | sort -u | cut -f 1 -d ' ' | uniq -c `

next="0"

# Rozbije trochu nestastne, strieda sa pouzivatelske meno a pocet
for item in $OUT 
do
	# Vyraz vyhodnoti spravne, ale stazuje sa, ze nema cislo
	# pretoze kazdy druhy zaznam je meno
	if [ "$next" -gt "$N" ] 2> /dev/null
	then
		echo "Output: '$item $next'"
	fi

	# Nastavime aktualnu polozku pre dalsie pouzitie
	next="$item"
done


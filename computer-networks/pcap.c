#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pcap.h>

#define FIN 1
#define SYN 2
#define RST 4
#define ACK 16

struct stream {
	unsigned client_addr;
	unsigned server_addr;
	unsigned client_port;
	unsigned server_port;
	
	int index, status;
};

struct stream *streams[4096];
unsigned streams_index = 1;

int disable_data = 0;
int disable_summary = 0;
int disable_frames = 0;

int filter_all = 1;
int filter_http = 0;
int filter_https = 0;
int filter_ftp = 0;
int filter_tftp = 0;
int filter_dns = 0;
int filter_telnet = 0;
int filter_ssh = 0;
int filter_arp = 0;
int filter_password = 0;
int filter_stream = 0;
int task_i = 0;

void help()
{
	puts("Pouzitie: ./analyzator <volby> <subor>");
	puts("\t-?       Pouzitie");
	puts("\t-<a-i>   Uloha a-g");
	puts("\t-s <n>   Iba komunikacia n");
	puts("\t-n       Nezobrazi obsah ramca");
	puts("\t-o       Nezobrazi suhrn komunikacii");
	puts("\t-p       Nezobrazi ramce");
	puts("\t<subor>  PCAP subor na analyzovanie");
}

// Nájde komunikáciu
struct stream *find_stream(unsigned source_addr, unsigned destination_addr, 
	unsigned source_port, unsigned destination_port)
{
	int i;
	
	for (i = 1; i < streams_index; i++)
	{
		if (streams[i]->client_addr == source_addr && streams[i]->client_port == source_port
			&& streams[i]->server_addr == destination_addr && streams[i]->server_port == destination_port)
			return streams[i];
	}
	
	return NULL;
}

// Pridá komunikáciu
struct stream *new_stream(unsigned source_addr, unsigned destination_addr, 
	unsigned source_port, unsigned destination_port)
{
	streams[streams_index] = malloc(sizeof(struct stream));
	streams[streams_index]->client_addr = source_addr;
	streams[streams_index]->server_addr = destination_addr;
	streams[streams_index]->client_port = source_port;
	streams[streams_index]->server_port = destination_port;
	streams[streams_index]->index = streams_index;
	
	return streams[streams_index++];
}

// ARP
void handle_arp(const u_char *data, int len)
{
	unsigned op;
	unsigned source_addr, destination_addr;
	struct stream *stream;
	
	source_addr = *(unsigned *)(data + 14);
	destination_addr = *(unsigned *)(data + 24);

	op = *(data + 7);
	if (op == 0x1) // Request
	{
		printf("Who has %d.%d.%d.%d? Tell %d.%d.%d.%d\n",
			*(data + 24), *(data + 25), *(data + 26), *(data + 27),
			*(data + 14), *(data + 15), *(data + 16), *(data + 17));
			
		stream = find_stream(source_addr, destination_addr, 0, 0);
		if (stream)
			printf("Komunikacia %d\n", stream->index);
		else 
			printf("DEBUG: Komunikacia mala existovat\n");
	}
	else if (op == 0x2) // Reply
	{
		printf("Reply from %d.%d.%d.%d\n",
			*(data + 24), *(data + 25), *(data + 26), *(data + 27));
			
		stream = find_stream(destination_addr, source_addr, 0, 0);
		if (stream)
			printf("Komunikacia %d\n", stream->index);
	}
	else
		printf("Operacia 0x%X\n", op);
}

// UDP
void handle_udp(const u_char *data, int len, unsigned source_addr, unsigned destination_addr)
{
	unsigned source_port, destination_port, flags;
	struct stream *stream;
	
	// Porty
	source_port = *data * 256 + *(data + 1);
	destination_port = *(data + 2) * 256 + *(data + 3);
	
	printf("Port %d --> %d\n", source_port, destination_port);
	
	// Komunikácia
	stream = find_stream(source_addr, destination_addr, source_port, destination_port);
	if (! stream)
		stream = find_stream(destination_addr, source_addr, destination_port, source_port);
	
	// Ak neexistuje, problem
	if (! stream)
		printf("DEBUG: Komunikacia mala existovat\n");
	else
		printf("Komunikacia %d\n", stream->index);	
}

// TCP
void handle_tcp(const u_char *data, int len, unsigned source_addr, unsigned destination_addr)
{
	unsigned source_port, destination_port, flags;
	struct stream *stream;
	
	// Porty
	source_port = *data * 256 + *(data + 1);
	destination_port = *(data + 2) * 256 + *(data + 3);
	
	// Flagy
	flags = *(data + 13);
	printf("Port %d --> %d\n", source_port, destination_port);
	printf("Flags 0x%x ", flags);
	if (flags & FIN)
		printf("FIN ");
	if (flags & SYN)
		printf("SYN ");
	if (flags & RST)
		printf("RST ");
	if (flags & ACK)
		printf("ACK ");
	printf("\n");
	
	// Zaciatok komunikácie
	if (flags == SYN)
	{
		stream = find_stream(source_addr, destination_addr, source_port, destination_port);
		if (stream)
			printf("Zaciatok komunikacie %d [SYN]\n", stream->index);
		else
			printf("DEBUG: Komunikacia sa mala najst\n");
			
	}
	else if (flags == (SYN | ACK))
	{
		stream = find_stream(destination_addr, source_addr, destination_port, source_port);
		if (stream)
			printf("Zaciatok komunikacie %d [SYN/ACK]\n", stream->index);
		else
			printf("SYN/ACK bez zaciatku komunikacie\n");
	}
	
	// Koniec komunikacie
	else if (flags == (FIN | ACK) 
			&& (stream = find_stream(source_addr, destination_addr, source_port, destination_port)))
	{
		printf("Koniec komunikacie %d [FIN/ACK]\n", stream->index);
	}
	else if (flags == (FIN | ACK) 
			&& (stream = find_stream(destination_addr, source_addr, destination_port, source_port)))
	{
		printf("Koniec komunikacie %d [FIN/ACK]\n", stream->index);
	}
	else if (flags == ACK
			&& (stream = find_stream(source_addr, destination_addr, source_port, destination_port))
			&& (stream->status & FIN))
	{
		printf("Koniec komunikacie %d [ACK]\n", stream->index);
	}
	else if (flags == ACK
			&& (stream = find_stream(destination_addr, source_addr, destination_port, source_port))
			&& (stream->status & FIN))
	{
		printf("Koniec komunikacie %d [ACK]\n", stream->index);
	}
	
	// Komunikacia
	else 
	{
		stream = find_stream(source_addr, destination_addr, source_port, destination_port);
		if (! stream)
			stream = find_stream(destination_addr, source_addr, destination_port, source_port);
			
		if (stream)
		{
			printf("Komunikacia %d\n", stream->index);
		}
		else
			printf("Segment bez zaciatku komunikacie\n");
	}
}

// IP
void handle_ip(const u_char *data, int len)
{
	unsigned type, header, total;
	unsigned source_addr, destination_addr;

	// Vypíše zdrojovú a cieľovú adresu IP paketu
	printf("IP adresa %d.%d.%d.%d --> %d.%d.%d.%d\n", 
		*(data + 12), *(data + 13), *(data + 14), *(data + 15),
		*(data + 16), *(data + 17), *(data + 18), *(data + 19));

	source_addr = *(unsigned *)(data + 12);
	destination_addr = *(unsigned *)(data + 16);
	
	// Protocol, header length, total length
	type = *(data + 9);
	header = (*data & 0xF) * 4;
	total = *(data + 2) * 256 + *(data + 3);
	if (type == 0x11)
	{
		printf("Typ segmentu UDP\n");
		handle_udp(data + header, total - header, source_addr, destination_addr);
	}
	else if (type == 0x6)
	{
		printf("Typ segmentu TCP\n");
		handle_tcp(data + header, total - header, source_addr, destination_addr);
	}
	else if (type == 0x1)
		printf("Typ segmentu ICMP\n");
	else
		printf("Typ segmentu 0x%X\n", type);
}

// Filtrovacie funkcie
int filter_udp(const u_char *data, int len, int source_addr, int destination_addr)
{
	unsigned source_port, destination_port;
	struct stream *stream;
	
	source_port = *data * 256 + *(data + 1);
	destination_port = *(data + 2) * 256 + *(data + 3);
	
	// Podľa komunikácie
	stream = find_stream(source_addr, destination_addr, source_port, destination_port);
	if (! stream)
		stream = find_stream(destination_addr, source_addr, destination_port, source_port);
	
	// Ak neexistuje, vytvoríme
	if (! stream)
	{
		stream = new_stream(source_addr, destination_addr, source_port, destination_port);

		if (filter_stream == stream->index)
			return 1;
	}
	else
		if (filter_stream == stream->index)
			return 1;
	
	// Podľa portov
	if (filter_tftp && (source_port == 69 || destination_port == 69))
		return 1;
	if (filter_dns && (source_port == 53 || destination_port == 53))
		return 1;
		
	// Iné nevyhovujú
	return 0;
}

int filter_tcp(const u_char *data, int len, int source_addr, int destination_addr)
{
	unsigned source_port, destination_port, flags;
	struct stream *stream;
	
	source_port = *data * 256 + *(data + 1);
	destination_port = *(data + 2) * 256 + *(data + 3);
	
	// Podľa komunikacie
	flags = *(data + 13);

	if (flags == SYN)
	{
		// Pokračovanie UDP komunikácie?
		stream = find_stream(source_addr, destination_addr, source_port, destination_port);
		if (stream)
			stream->status = SYN;
		else
		{
			stream = new_stream(source_addr, destination_addr, source_port, destination_port);
			stream->status = SYN;
		}
	}
	else if (flags == (SYN | ACK))
	{
		stream = find_stream(destination_addr, source_addr, destination_port, source_port);
		if (stream)
			stream->status = SYN | ACK;
	}
	else if (flags == ACK 
			&& (stream = find_stream(source_addr, destination_addr, source_port, destination_port)) 
			&& (stream->status == (SYN | ACK)))
	{
		stream->status = ACK;
	}
	else if (flags == ACK 
			&& (stream = find_stream(destination_addr, source_addr, destination_port, source_port))
			&& (stream->status & FIN))
	{
		stream->status = FIN | ACK;
	}
	else if (flags == (FIN | ACK) 
			&& (stream = find_stream(source_addr, destination_addr, source_port, destination_port)))
	{
		stream->status = FIN;
	}
	else if (flags == (FIN | ACK) 
			&& (stream = find_stream(destination_addr, source_addr, destination_port, source_port)))
	{
		stream->status = 0x300;
	}
	else if (flags == RST)
	{
		stream = find_stream(source_addr, destination_addr, source_port, destination_port);
		if (! stream)
			stream = find_stream(destination_addr, source_addr, destination_port, source_port);
		if (stream)
			stream->status = ACK;	
	}
	else
	{
		stream = find_stream(source_addr, destination_addr, source_port, destination_port);
		if (! stream)
			stream = find_stream(destination_addr, source_addr, destination_port, source_port);
	}
	
	if (stream && stream->index == filter_stream)
		return 1;
	
	// Podľa portov
	if (filter_http && (source_port == 80 || destination_port == 80))
		return 1;
	if (filter_https && (source_port == 443 || destination_port == 443))
		return 1;
	if (filter_ftp && (source_port == 20 || destination_port == 20))
		return 1;
	if (filter_ftp && (source_port == 21 || destination_port == 21))
		return 1;
	if (filter_telnet && (source_port == 23 || destination_port == 23))
		return 1;
	if (filter_ssh && (source_port == 22 || destination_port == 22))
		return 1;
	if (filter_dns && (source_port == 53 || destination_port == 53))
		return 1;
		
	// Iné nevyhovujú
	return 0;
}

int filter_ip(const u_char *data, int len)
{
	unsigned type, header, total, i, source_addr, destination_addr;
	
	// Adresy pre komunikácie
	source_addr = *(unsigned *)(data + 12);
	destination_addr = *(unsigned *)(data + 16);
	
	type = *(data + 9);
	header = (*data & 0xF) * 4;
	total = *(data + 2) * 256 + *(data + 3);
	if (type == 0x11)
	{
		if (filter_udp(data + header, total - header, source_addr, destination_addr))
			return 1;
	}
	else if (type == 0x6)
	{
		if (filter_tcp(data + header, total - header, source_addr, destination_addr))
			return 1;
	}
	
	// PASS
	if (filter_password)
		for (i = 0; i < len; i++)
		{
			if (strncasecmp(data + i, "PASS", 4) == 0)
					return 1;
		}
		
	// Iné ako UDP a TCP nevyhovujú
	return 0;
}

int filter_arp_f(const u_char *data, int len)
{
	unsigned op;
	unsigned source_addr, destination_addr;
	struct stream *stream = NULL;
	
	source_addr = *(unsigned *)(data + 14);
	destination_addr = *(unsigned *)(data + 24);

	stream = find_stream(destination_addr, source_addr, 0, 0);
	if (! stream)
		stream = find_stream(source_addr, destination_addr, 0, 0);
	if (! stream)
		stream = new_stream(source_addr, destination_addr, 0, 0);	

	op = *(data + 7);
	if (op == 0x1) // Request
	{
		stream->status = 0x100;
	}
	else if (op == 0x2) // Reply
	{
		stream->status = 0x200;
	}
	
	if (stream && stream->index == filter_stream)
		return 1;
	
	return filter_arp;
}

int filter_frame(const u_char *data, unsigned len)
{
	int type;
	
	type = *(data + 12) * 256 + *(data + 13);
	if (type == 0x806)
	{
		if (filter_arp_f(data + 14, len - 14))
			return 1;
	}
	else if (type == 0x800)
	{
		if (filter_ip(data + 14, len - 14))
			return 1;	
	}
			
	// Iné ako ARP a IP nevyhovujú
	return 0;
}

int main(int argc, char **argv)
{
	pcap_t *pcap;
	char errbuf[PCAP_ERRBUF_SIZE];
	int o;
	const u_char *data;
	struct pcap_pkthdr header;
	int cislo, i, len, type, filter;
	char buffer[17], client[32], server[32];
	const char *status;
	int count = 0;
	char cmd[64];
	
	memset(buffer, 0, 17);
	
	// Spracovanie argumentov
    opterr = 0;
    while ((o = getopt(argc, argv, "abcdefghinops:?")) != -1)
        switch (o) 
        {
            case '?':
                help();
              	return 0;
            	break;
            case 'a':
            	filter_all = 0;
                filter_http = 1;
                break;
            case 'b':
            	filter_all = 0;
				filter_https = 1;
				break;
            case 'c':
            	filter_all = 0;
				filter_ftp = 1;
				break;
            case 'd':
            	filter_all = 0;
				filter_tftp = 1;
				break;
            case 'e':
            	filter_all = 0;
				filter_dns = 1;
				break;
            case 'f':
            	filter_all = 0;
				filter_telnet = 1;
				break;
            case 'g':
            	filter_all = 0;
				filter_ssh = 1;
				break;
            case 'h':
            	filter_all = 0;
				filter_arp = 1;
				break;
			case 'i':
				task_i = 1;
				break;
			case 'w':
            	filter_all = 0;
				filter_password = 1;
				break;
			case 's':
            	filter_all = 0;
				filter_stream = atoi(optarg);
				break;
			case 'n':
				disable_data = 1;
				break;
			case 'o':
				disable_summary = 1;
				break;
			case 'p':
				disable_frames = 1;
				break;
            default:
                printf("Error: '%c': nespravny prepinac alebo nespravny argument\n\n", optopt);
                help();
                
                return 1;
        }

    argc -= optind;
    argv += optind;
    
    if (argc < 1)
    {
    	puts("Error: Subor nebol zadany\n");
    	help();
    	
    	return 1;
    }
	
	// Otvorí súbor
	pcap = pcap_open_offline(argv[0], errbuf);
	if (! pcap)
	{
		puts(errbuf);
		
		return 1;
	}
	
	// Pre všetky framy uloží (v hlaviške je okrem iného dĺžka, vráti dáta)
	cislo = 1;	
	while (data = pcap_next(pcap, &header))
	{				
		// Ak vypisujeme všetky alebo paket vyhovuje filtru
		filter = filter_frame(data, header.len);
		
		if (! disable_frames && (filter || filter_all))
		{
			printf("Ramec %d\n", cislo);
			printf("Dlzka %d bajtov\n", header.len);
		
			// Typ paketu/rámca
			type = *(data + 12) * 256 + *(data + 13);
			if (type <= 0x5DC)
				printf("Typ ramca IEEE 802.3\n");
			else if (type == 0x806)
			{
				printf("Typ paketu ARP\n");
				handle_arp(data + 14, header.len - 14);
			}
			else if (type == 0x800)
			{
				printf("Typ paketu IPv4\n");
				handle_ip(data + 14, header.len - 14);
		
			}
			else if (type == 0x86DD)
				printf("Typ paketu IPv6\n");
			else
				printf("Typ paketu 0x%X\n", type);
		
			if (! disable_data)
			{
				// Do dĺžky zaokrúhlenej na násobky 16
				len = header.len % 16 == 0 ? header.len : ((header.len + 15) / 16) * 16;
				for (i = 1; i <= len; i++)
				{
					// Ak vypisujeme dáta
					if (i <= header.len)
					{
						printf("%02X ", data[i - 1]);
						buffer[(i - 1) % 16] = isprint(data[i - 1]) ? data[i - 1] : '.'; // Tlačiteľné znaky do bufferu, inak .
					}
					else // Alebo zarovnávame
						printf("   "); 
			
					// Po 16 vypíše buffer a ukončí riadok
					if (i % 16 == 0)
					{
						printf(" %s", buffer);
						memset(buffer, 0, 17);
						printf("\n");
					}
					else if (i % 8 == 0) // Po 8 pridá medzeru
						printf(" ");
				}
			}
		
			puts("");
		}
		cislo++;
	}
	
	// Prehľad komunikácií
	if (! disable_summary && streams_index)
	{
		puts(" #    Klient                   Server                   Komunikacia ");
		puts("====================================================================");
		for (i = 1; i < streams_index; i++)
		{
			// Stav komunikácie
			if (streams[i]->status == 0x300)
				status = "TCP kompletna 3";
			else if (streams[i]->status == (FIN | ACK))
				status = "TCP kompletna 4";
			else if (streams[i]->status == 0x200)
				status = "ARP kompletna";
			else if (streams[i]->status == 0x100)
				status = "ARP nekompletna";
			else if (streams[i]->status)
				status = "TCP nekompletna";
			else
				status = "UDP";
			
			// Adresa s portom
			if (streams[i]->client_port)
				sprintf(client, "%d.%d.%d.%d:%d", 
					*((u_char *)&streams[i]->client_addr), *((u_char *)&streams[i]->client_addr + 1), 
					*((u_char *)&streams[i]->client_addr + 2), *((u_char *)&streams[i]->client_addr + 3),
					streams[i]->client_port);
			else
				sprintf(client, "%d.%d.%d.%d", 
					*((u_char *)&streams[i]->client_addr), *((u_char *)&streams[i]->client_addr + 1), 
					*((u_char *)&streams[i]->client_addr + 2), *((u_char *)&streams[i]->client_addr + 3));
	
			if (streams[i]->server_port)
				sprintf(server, "%d.%d.%d.%d:%d", 
					*((u_char *)&streams[i]->server_addr), *((u_char *)&streams[i]->server_addr + 1), 
					*((u_char *)&streams[i]->server_addr + 2), *((u_char *)&streams[i]->server_addr + 3), 
					streams[i]->server_port);
			else
				sprintf(server, "%d.%d.%d.%d", 
					*((u_char *)&streams[i]->server_addr), *((u_char *)&streams[i]->server_addr + 1), 
					*((u_char *)&streams[i]->server_addr + 2), *((u_char *)&streams[i]->server_addr + 3));
		
			printf(" %-3d  %-22s   %-22s   %s\n", i, client, server, status);
		}
	}
	
	// Doplňujúca úloha
	if (task_i)
	{
		for (i = 1; i < streams_index; i++)
		{
			if (streams[i]->status == 0x300 && streams[i]->server_port == 80)
			{
				sprintf(cmd, "./analyzator -os %d %s", streams[i]->index, argv[0]);
				system(cmd);
				count++;
			}
		}
		printf("Pocet 3-cestne ukoncenych HTTP komunikacii: %d\n", count);
	}
	
	return 0;
}


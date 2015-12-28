#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>

#define DATA 1460

/*
 * Pri pouziti struct sockaddr_in z nejakeho dovodu neskompilovalo, 
 * to iste ako netinet/in.h tu definovane funguje
 */
struct sa
{
	__SOCKADDR_COMMON (sin_);
	in_port_t sin_port;                 /* Port number.  */
	struct in_addr sin_addr;            /* Internet address.  */

	/* Pad to size of `struct sockaddr'.  */
	unsigned char sin_zero[sizeof (struct sockaddr) -
                       __SOCKADDR_COMMON_SIZE -
                       sizeof (in_port_t) -
                       sizeof (struct in_addr)];
};

/*
 * Štruktúra aplikačného protokolu
 */
struct datagram {
	int length; // Dlzka fragmentu
	int total; // Dĺžka spráby
	long checksum; 
	int sequence;
	u_char data[DATA];
};

int sock;

void help()
{
	puts("Pouzitie: ./udpping <volby> <ip_servera>");
	puts("\t-h            Zobrazi pouzitie");
	puts("\t-n            Pocet vzoriek");
	puts("\t-l            Velkost vzorky v bajtoch");
	puts("\t-s            Serverovy rezim, volby l a n ignorovane");
	puts("\t<ip_servera>  IP ciela pingu alebo servera");
}

int checksum(const u_char *msg, unsigned len)
{
	int c, i;
	
	if (len)
	{
		c = msg[0];
		
		for (i = 1; i < len; i++)
			c ^= msg[i]; 
	}
	else
		c = 0;
		
	return c;
}

// Odošle správu
void msg_send(const u_char *msg, unsigned length, struct sa *to_addr, int e)
{
	struct datagram dgram;
	int sending;
	int seq;
	
	seq = 0;
	for (sending = length; sending > 0; sending -= DATA)
	{
		seq++;
			
		if (seq % 2 == e)
			continue;

		dgram.length = sending > DATA ? DATA : sending;
		dgram.total = length;
		dgram.checksum = checksum(msg + (length - sending), sending > DATA ? DATA : sending);
		dgram.sequence = seq;
		memcpy(dgram.data, msg + (length - sending), sending > DATA ? DATA : sending);

		printf("Posielam fragment #%d\n", dgram.sequence);
	
		//if (seq != 4)
		if (sendto(sock, &dgram, sizeof(dgram), 0, (struct sockaddr *) to_addr, sizeof(struct sockaddr_in)) == -1)
			perror("sendto");
	}
	
	e = !e; seq = 0;
	for (sending = length; sending > 0; sending -= DATA)
	{
		seq++;
			
		if (seq % 2 == e)
			continue;

		dgram.length = sending > DATA ? DATA : sending;
		dgram.total = length;
		dgram.checksum = checksum(msg + (length - sending), sending > DATA ? DATA : sending);
		dgram.sequence = seq;
		memcpy(dgram.data, msg + (length - sending), sending > DATA ? DATA : sending);

		printf("Posielam fragment #%d\n", dgram.sequence);
	
		// if (seq != 3)
		if (sendto(sock, &dgram, sizeof(dgram), 0, (struct sockaddr *) to_addr, sizeof(struct sockaddr_in)) == -1)
			perror("sendto");
	}
}

// Prijme správu
int msg_recv(u_char **msg, unsigned *length, struct sa *from_addr, int e)
{
	struct datagram dgram;
	unsigned addrlen = sizeof(struct sockaddr_in);
	u_char *buffer = NULL;
	long sum;
	int ok = 1, expected = 0, len, i, received = 0;

	for (;;) 
	{
		len = recvfrom(sock, &dgram, sizeof(dgram), 0, (struct sockaddr *)from_addr, &addrlen);
		if (len == -1)
		{
			if (errno == EAGAIN && buffer == NULL && e == 0)
				continue;
				
			ok = 0;
			perror("recvfrom");
			
			break;
		}
		
		/*
		printf("Prijaty paket %d\n", dgram.sequence);
		continue;
		*/
	
		if (buffer == NULL)
		{
			printf("Prijata sprava s dlzkou %d:\n", dgram.total);
		
			// Alokujeme buffer
			buffer = malloc(dgram.total);
			if (buffer == NULL)
			{
				perror("malloc");
				exit(1);
			}
			
			*msg = buffer;
		}
		
		memcpy(buffer + (dgram.sequence - 1) * DATA, dgram.data, dgram.length);
		received += dgram.length;
		printf("Prijimam fragment #%d (%d/%d)\n", dgram.sequence, received, dgram.total);
		//for (i = 0; i < dgram.length; i++)
		//	putchar(dgram.data[i]);
		
		// Ak nesedí checksum, chyba
		sum = checksum(dgram.data, dgram.length);
		if (sum != dgram.checksum)
			ok = 0;
			
		/*
		// Ak nesedí poradie, chyba
		if (dgram.sequence != expected)
			ok = 0;
			 
		expected++;
		*/
		
		if (received == dgram.total)
			break;
	}
	
	*length = received;
	
	// Vypíše správu
	for (i = 0; i < received; i++)
		putchar((*msg)[i]);
	puts("");
	
	if (! ok)
		puts("Checksum nesedi");
	
	return ok;
}

int main(int argc, char **argv)
{
	int o, opterr;
	int port = 3333, number = 4, length = 64, server = 0;
	char *addr;
	u_char *msg;
	int i, j;
	struct sa to_addr, my_addr, from_addr;
	unsigned msg_length;
	struct timeval time1, time2, timeout;
	long double timed, total;
	int bufsize = 500000;
	
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	
	// Vytvoríme socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1)
	{
		perror("socket");
		exit(1);
	}
	
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
		perror("setsockopt");
		
	if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize)) == -1)
		perror("setsockopt");
		
	// Spracovanie argumentov
    opterr = 0;
    while ((o = getopt(argc, argv, "n:l:sh")) != -1)
        switch (o) 
        {
            case 'h':
                help();
              	exit(0);
            	break;
            case 'n':
            	number = atoi(optarg);
            	break;
            case 'l':
            	length = atoi(optarg);
            	break;
            case 's':
            	server = 1;
            	break;
            default:
                printf("Error: '%c': nespravny prepinac alebo nespravny argument\n\n", optopt);
                help();
                exit(1);
        }

    argc -= optind;
    argv += optind;
    
    // Adresa
    if (argc < 1)
    {
    	fprintf(stderr, "Malo argumentov\n");
    	exit(1);
    }
    addr = argv[0];
    
    // Server alebo klient
    if (server)
    {
    	printf("Cakam na UDP porte %d\n", port);
    	
    	// Adresa servera
		my_addr.sin_addr.s_addr = inet_addr(addr);
		my_addr.sin_family = AF_INET;
		my_addr.sin_port = htons(port);
		bind(sock, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
 
    	// Prijimame vzorky
    	for (;;)
    	{
    		// Modifikacia vzorky, len ak checksum sedi
    		if (msg_recv(&msg, &msg_length, &from_addr, 0))
    		{
    			for (i = 0; i < msg_length; i++)
    				msg[i] += 1;
						
				// Spat klientovi
				printf("Posielam spat modifikovanu spravu\n\n");
    		}
    		else
    		{
				printf("Posielam spat nemodifikovanu spravu\n\n");
    		}
			msg_send(msg, msg_length, &from_addr, 0);
    		free(msg);
    	}
    }
    else
    {
    	printf("Posielam %d vzoriek velkosti %d bajtov na %s:%d\n", number, length, addr, port);
    		
		// Alokujeme vzorku
		msg = malloc(length);
		if (msg == NULL)
		{
			perror("malloc");
			exit(1);
		}
    	
		// Cielova adresa
		to_addr.sin_addr.s_addr = inet_addr(addr);
		to_addr.sin_family = AF_INET;
		to_addr.sin_port = htons(port);
				
    	// Posielame vzorky
    	total = 0;
    	for (i = 1; i <= number; i++)
    	{
			// Naplnime vzorku
			for (j = 0; j < length; j++)
				msg[j] = 'a';
    	
    		// Posiela
    		printf("Posielam spravu %d a cakam na odpoved...\n", i);
    		gettimeofday(&time1, NULL);
			msg_send(msg, length, &to_addr, 1);
			
			// Caka na odpoved
			msg_recv(&msg, &msg_length, &from_addr, 1);
				
			// Spocita casy
			gettimeofday(&time2, NULL);
			timed = ((long double) ((long double)(time2.tv_sec * 1000000 + time2.tv_usec) - (long double)(time1.tv_sec * 1000000 + time1.tv_usec))) / 1000;
			total += timed;
			printf("Odpoved trvala %.2Lf ms, polovicna hodnota %.2Lf\n", timed, timed / 2);
			puts("");
    	}
    	
    	// Priemer
    	printf("Priemerny cas odpovede %.2Lf ms, polovicna hodnota %.2Lf ms\n", total / number, total / number / 2);
    }

	return 0;
}


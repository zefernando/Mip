#include "mipTcpIp.h"
#include "mipXfer.h"

extern int Log_This(char *,char);
extern char Log_Msg[128];


/* Converts ascii text to in_addr struct.  NULL is returned if the address
   can not be found. */
struct in_addr *atoaddr(address)
			char *address;
{
	struct hostent *host;
	static struct in_addr saddr;

	/* First try it as aaa.bbb.ccc.ddd. */
	saddr.s_addr = inet_addr(address);
	if (saddr.s_addr != -1) {
		return &saddr;
	}
	host = gethostbyname(address);
	if (host != NULL) {
		return (struct in_addr *) *host->h_addr_list;
	}
	return NULL;
}


/* This is a generic function to make a connection to a given server/port.
   service is the port name/number,
   type is either SOCK_STREAM or SOCK_DGRAM, and
   netaddress is the host name to connect to.
   The function returns the socket, ready for action.*/
int mipCONNECT(port, netaddress)
int port;
char *netaddress;
{

	struct in_addr *addr;
	int sock, connected;
	struct sockaddr_in address;

	if (port == -1) {
		fprintf(stderr,"make_connection:  Invalid socket type.\n");
		return -1;
	}
	addr = atoaddr(netaddress);
	if (addr == NULL) {
		fprintf(stderr,"make_connection:  Invalid network address.\n");
		return -1;
	}

	memset((char *) &address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = (port);
	address.sin_addr.s_addr = addr->s_addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);



	printf("Connecting to %s on port %d.\n",inet_ntoa(*addr),htons(port));

	connected = connect(sock, (struct sockaddr *) &address, sizeof(address));
	if (connected < 0) {
		perror("connect");
		return -1;
	}
	return sock;
}

void mipDISCONNECT(sockfd)
int sockfd;
{
	close(sockfd);
}

int mipRecv(sockfd, buf,  count)
int sockfd;
char *buf;
int * count;
{
	char auxBuf[2048];
	int this_read;
	int tam;
	int ret;
	memset(auxBuf,0, sizeof(auxBuf));
	
	this_read = read(sockfd, auxBuf, sizeof(auxBuf));
	
	tam = auxBuf[0];
	tam =   (tam << 8 ) | auxBuf[1];
	if (tam !=  (this_read - 2)) {
		sprintf(Log_Msg,"Tamanho errado na recepcao do MIP: esperado(%d) - recebido",
		        tam, this_read -2);
		Log_This(Log_Msg, mpLOG_NORMAL);
		ret = -1;
	}
	else {
		memcpy(buf,&auxBuf[2], this_read -2);
		*count = this_read - 2;
		ret = 0;
	}
	return	ret;

}

/* This is just like the write() system call, accept that it will
   make sure that all data is transmitted. */
int mipSend(sockfd, buf, count)
int sockfd;
char *buf;
size_t count;
{
	char auxBuf[2048];
	size_t bytes_sent = 0;
	int this_write;
	
	auxBuf[0] = (char ) ((count & 0xff00) << 8);
	auxBuf[1] = (char ) (count & 0x00ff);
	memset(auxBuf,0, sizeof(auxBuf));
	memcpy(&auxBuf[2], buf, count);
	count += 2;
	while (bytes_sent < count) {
		do
			this_write = write(sockfd, auxBuf, count - bytes_sent);
		while ( (this_write < 0) && (errno == EINTR) );
		if (this_write <= 0)
			return this_write;
		bytes_sent += this_write;
		buf += this_write;
	}
	return bytes_sent;
}


/* This ignores the SIGPIPE signal.  This is usually a good idea, since
   the default behaviour is to terminate the application.  SIGPIPE is
   sent when you try to write to an unconnected socket.  You should
   check your return codes to make sure you catch this error! */
void ignore_pipe(void)
{
	struct sigaction sig;

	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE,&sig,NULL);
}



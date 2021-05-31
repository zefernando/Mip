# Uncomment the following line for Solaris
# C_LINK = -lsocket -lnsl

# Uncomment this for SCO.  (Note, this has only been reported to work with
# Revision 3.2.4 with the "SCO TCP/IP Development System" package installed.
# Please let me know if you have any other SCO success stories.
# C_LINK = -lsocket

# Comment the following line if you are not using the gnu c compiler
# C_ARGS = -Wall

# You might have to change this if your c compiler is not cc
CFLAGS = -g -DDLPI_20 -DDEBUG -DNPI_20 -I/usr/include -bimport:/usr/lib/pse.exp
ODMLIB = -L/usr/lib -lodm -lcfg
CC     = xlc_r

obj =	\
	mipXfer.o \
	mipTcpIp.o \
	mipUtil.o \
	mipTty.o \
	mipTalk.o 

all:	mipXfer conv2asc

	
mipXfer: $(obj)
	$(CC) -o mipXfer $(obj)
conv2asc:	conv2asc.o
	$(CC) -o conv2asc conv2asc.o
mipXfer.o:	mipXfer.c
	$(CC) $(CFLAGS) $(C_LINK) -c mipXfer.c
mipXfer.o:	mipXfer.c
	$(CC) $(CFLAGS) -c mipXfer.c
mipTcpIp.o:	mipTcpIp.c
	$(CC) $(CFLAGS) -c mipTcpIp.c
mipUtil.o:	mipUtil.c
	$(CC) $(CFLAGS) -c mipUtil.c
mipTty.o:	mipTty.c
	$(CC) $(CFLAGS) -c mipTty.c
mipTalk.o:	mipTalk.c
	$(CC) $(CFLAGS) -c mipTalk.c
conv2asc.o:	conv2asc.c
	$(CC) $(CFLAGS) -c conv2asc.c
clean:
	rm -f *.o

distclean:	clean
	rm -f mipXfer

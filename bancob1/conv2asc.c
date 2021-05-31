#include	<stdio.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include 	<string.h>
#include 	<errno.h>
#include 	<unistd.h>
#include 	<time.h>

#define uchar unsigned char
#define LRECL 133
#define	OPTSTRING "hvj:d:t:s:l:HVJ:D:T:S:L"


/* Decimal
Value        Packed Format     Zoned Format

+123         12 3C             F1 F2 C3
             or                or
             12 3F             F1 F2 F3

-4321        04 32 1D          F4 F3 F2 D1

+000050      00 00 05 0C       F0 F0 F0 F0 F5 C0
             or                or
             00 00 05 0F       F0 F0 F0 F0 F5 F0

-7           7D                D7

 00000       00 00 0C          F0 F0 F0 F0 C0
             or                or
             00 00 0F          F0 F0 F0 F0 F0
*/

/*
 * prototypes
 */
int Read_Header_Desc(char  * Name);
int Read_Detail_Desc(char  * Name);
int Read_Trailer_Desc(char  * Name);

int to_zoned( uchar *dest, uchar *src, int packedlen);
uchar ucharBase10Zoned(uchar nible);
uchar withSign(uchar hex);
int conv_detail(uchar * inb_p, int out);
int conv_header(uchar * inb_p, int out);
int conv_trailer(uchar * inb_p, int out);
int conv(char * Name);
void DumpMsg( uchar *bp, int len, char *msg);

/* Struturas que definem os arquivos */
struct FileHeader {
		int		field;
		char	name[80];
		int		start;
		int		end;
		int		length;
		char	datatype[80];
};

typedef struct FileHeader FileHeader_t;

struct FileDetail {
		int		field;
		char	name[80];
		int		start;
		int		end;
		int		length;
		char	datatype[80];
};

typedef struct FileDetail FileDetail_t;

struct FileTrailer {
		int		field;
		char	name[80];
		int		start;
		int		end;
		int		length;
		char	datatype[80];
};

typedef struct FileTrailer FileTrailer_t;

FileHeader_t head[100];
FileDetail_t detail[1024];
FileTrailer_t trailer[100];

/* Vetor de strings para identificar arquivos comapctados */
char * compfiles[256];

/* Buffer para leitura do bulk file */
static uchar inbuf[2048];

/* Buffer de saida */
static uchar outbuf[2048];

/* Buffer auxiliar */
static uchar auxbuf[2048];

/* Variaveis globais */
/* Tamanhos dos registros */
char gName[256];
int gHeaderLen, gDetailLen, gTrailerLen;
int gDebug, gLogicalRecSize ;
FILE *dbgout;

/* Le a lista de arquivos compactados */
int Read_Comp_List()
{
	FILE * listin;
	char listbuf[256];
	char *p;
	int len, i;

	listin = fopen("complist", "r");
	i = 0;
	fprintf(dbgout,"Lista de arquivos compactados\n");
	while(fgets(listbuf, 80, listin)) {
		len = strlen(listbuf);
		listbuf[len - 1] = '\0';
		if(gDebug) {

			fprintf(dbgout, listbuf);
			fprintf(dbgout, "\n");
			fflush(dbgout);
		}
		p = (char *) malloc(sizeof(listbuf));
		compfiles[i] = p;
		strcpy(p, listbuf);
		i++;
	}
	compfiles[i] = (char *) NULL;
	fclose(listin);
	return 1;
}

/* Verifica se o arquivo esta na lista de arquivos compactados */
int isComp(char * Name)
{
	char *p;
	int i;
	for(i = 0; ((p = compfiles[i]) != (char *) NULL) && (i < 256); i++) {
		if(strcmp(Name, p) == 0) return 1;
	}
	/* O arquivo nao e compactado */
	return 0;
}

/* Le o arquivo descritivo do Header Record */
int Read_Header_Desc(char  * N)
{
	FILE * headin;
	char Name[256];
	int i;
	int HeaderLen;

	strcpy(Name, N);
	strcat(Name, "head");
	if(gDebug)
		fprintf(dbgout, "\nNome %s\n", Name);
	headin = fopen(Name, "r");
	if(headin == (FILE *) NULL) {
		fprintf(dbgout, "Nao abriu arquivo: %s\n", Name);
		exit(0);
	}
	HeaderLen =0;
	i = 0;
	if(gDebug) {
		fprintf(dbgout, "Header Record\n");
		fprintf(dbgout, "Field\tStart\tEnd\tLength\tDatatype\tName\n\n");
	}
	while(fscanf(headin,"%d%d%d%d%s%s",&head[i].field, &head[i].start, &head[i].end,&head[i].length,head[i].datatype, head[i].name) != EOF)
	{
		if(gDebug) {
			fprintf(dbgout, "%d\t%d\t%d\t%d\t%s\t%s\n",head[i].field, head[i].start, head[i].end,
					head[i].length, head[i].datatype, head[i].name);
		}
		i++;
	}
	head[i].field = 0;
	i--;
	if(i >= 0)
	HeaderLen = head[i].end;
	fclose(headin);
	fflush(dbgout);
	return HeaderLen;
}

/* Le o arquivo descritivo do Detail Record */
int Read_Detail_Desc(char  * N)
{
	FILE * detailin;
	char Name[256];
	int i;
	int DetailLen;

	strcpy(Name, N);
	strcat(Name, "detail");
	detailin = fopen(Name, "r");
	if(detailin == (FILE *) NULL) {
		fprintf(dbgout, "Nao abriu arquivo: %s\n", Name);
		exit(0);
	}
	DetailLen =0;
	i = 0;
	if(gDebug) {
		fprintf(dbgout, "\n\nDetail Record\n");
		fprintf(dbgout, "Field\tStart\tEnd\tLength\tDatatype\tName\n\n");
	}
	while(fscanf(detailin,"%d%d%d%d%s%s",&detail[i].field, &detail[i].start, &detail[i].end,&detail[i].length,detail[i].datatype, detail[i].name) != EOF)
	{
		if(gDebug) {
			fprintf(dbgout, "%d\t%d\t%d\t%d\t%s\t%s\n",detail[i].field, detail[i].start, detail[i].end, detail[i].length, detail[i].datatype, detail[i].name);
		}
		i++;
	}
	detail[i].field = 0;
	i--;
	if(i >= 0)
		DetailLen = detail[i].end;
	fclose(detailin);
	fflush(dbgout);
	return DetailLen;
}

/* Le o arquivo descritivo do Trailer Record */
int Read_Trailer_Desc(char  * N)
{
	FILE * trailerin;
	char Name[256];
	int i;
	int TrailLen;

	strcpy(Name, N);
	strcat(Name, "trailer");
	trailerin = fopen(Name, "r");
	TrailLen =0;
	i = 0;
	if(gDebug) {
		fprintf(dbgout, "\n\nTrailer Record\n");
		fprintf(dbgout, "Field\tStart\tEnd\tLength\tDatatype\tName\n\n");
	}

	while(fscanf(trailerin,"%d%d%d%d%s%s",&trailer[i].field, &trailer[i].start, &trailer[i].end,&trailer[i].length,trailer[i].datatype, trailer[i].name) != EOF)
	{
		if(gDebug) {
			fprintf(dbgout, "%d\t%d\t%d\t%d\t%s\t%s\n",trailer[i].field, trailer[i].start, trailer[i].end,
				trailer[i].length, trailer[i].datatype, trailer[i].name);
		}

			i++;
	}
	trailer[i].field = 0;
	i--;
	if(i >= 0)
		TrailLen = trailer[i].end;
	fclose(trailerin);
	fflush(dbgout);
	return TrailLen;
}

int conv(char * Name)
{
	int in, out;
	int padlen;
	char inputFile[80], outputFile[80];
	uchar Type;
	ssize_t Bytes;
	int DetailRecCount;
	strcpy(inputFile,Name);
	strcpy(outputFile,Name);
	strcat(outputFile,".asc");

	in = open(inputFile,O_RDONLY);
	out = open(outputFile,O_RDWR|O_CREAT);
	DetailRecCount = 0;
	while((Bytes = read(in, inbuf, gLogicalRecSize)) > 0) {
		Type = inbuf[0];
		if(Type == 0xff || Type == 0x00 || Type == 9) {
			if(gDebug) {
				DumpMsg( inbuf, Bytes, "Header/Trailler");
			}
			/* converte para ascii */
			if(Type == 0x00) {
				conv_header(inbuf, out);
			}
			if(Type == 0xff || Type == 0x09) {
				conv_trailer(inbuf,out);
			}	
		}
		else {
			if(gDebug) {
				fprintf(dbgout, "Leu Detail tamanho: %d\n" ,Bytes);
				DumpMsg( inbuf, Bytes, "DETAIL");
			}
			/* getchar(); */
			Type = inbuf[0];
			if(Bytes != gLogicalRecSize) break;
			conv_detail(inbuf, out);
			DetailRecCount++;
		}
	}
	if(gDebug) {
		fprintf(dbgout, "\n%d Details records lidos\n", DetailRecCount);
	}
	close(in);
	close(out);
	fflush(dbgout);
	return 1;
}

/* Converte cada header record para asc */
int conv_header(uchar * inb_p, int out)
{
	uchar xbuf[256];
	uchar zonedbuf[256];
	char ascbuf[256];
	int zonedlen, totalzlen, padlen;
	uchar *p;
	int i;
	i = 0;
	p = inb_p;
	totalzlen = 0;
	while(head[i].field != 0)
	{
		if(gDebug) {
			fprintf(dbgout, "\n%d\t%s\t%s\n",head[i].field, head[i].name, head[i].datatype);
		}
		memcpy(xbuf, p , head[i].length);
		if(gDebug) {
			DumpMsg(xbuf, head[i].length, "XBUF");
		}
		if(strstr(head[i].datatype,"COMP-3") != (char *) NULL) {
			if(gDebug)
				fprintf(dbgout, "Campo Compactado\n");
			/* Campo compactado */
			/* Converte de compactado para ebcdic zonado */
			zonedlen = to_zoned(zonedbuf, xbuf, head[i].length);
			if(gDebug)
				DumpMsg(zonedbuf, zonedlen, "ZONEDBUF");

			/* converte para ascii */
			conv_ascii(ascbuf, zonedbuf, zonedlen);
		}
		else {
			/* Campo nao compactado */
			zonedlen = head[i].length;
			/* Converte para ascii */
			if(i == 0) {
				ascbuf[0] = '0';
			}
			else
				conv_ascii(ascbuf, xbuf, head[i].length);
		}
		write(out, ascbuf, zonedlen);
		ascbuf[zonedlen] = '\0';
		if(gDebug)
			fprintf(dbgout, "Ascbuf: %s\n", ascbuf);
		p += head[i].length;
		totalzlen += zonedlen;
		i++;
	}
	/* Calcula o numero de Bytes para completar o Logical Record Size */
	padlen = gLogicalRecSize - totalzlen;
	memset(auxbuf, 0x20, padlen);
	memset((auxbuf + padlen), '\n', 1);
	write(out,auxbuf, padlen + 1);
	if(gDebug) {
		fprintf(dbgout, "gLogicalRecSize %d totalzlen %d padlen %d\n"			,gLogicalRecSize, totalzlen, padlen);
	}
	return 1;
}

/* Converte cada detail record para asc */
int conv_detail(uchar * inb_p, int out)
{
	uchar xbuf[256];
	uchar zonedbuf[256];
	char ascbuf[256];
	int zonedlen, totalzlen, padlen;
	uchar *p;
	int i;
	i = 0;
	p = inb_p;
	totalzlen = 0;
	while(detail[i].field != 0)
	{
		if(gDebug) {
			fprintf(dbgout, "\n%d\t%s\t%s\n",detail[i].field, detail[i].name, detail[i].datatype);
		}
		memcpy(xbuf, p , detail[i].length);
		if(gDebug) {
			DumpMsg(xbuf, detail[i].length, "XBUF");
		}
		if(strstr(detail[i].datatype,"COMP-3") != (char *) NULL) {
			if(gDebug)
				fprintf(dbgout, "Campo Compactado\n");
			/* Campo compactado */
			/* Converte de compactado para ebcdic zonado */
			zonedlen = to_zoned(zonedbuf, xbuf, detail[i].length);
			if(gDebug)
				DumpMsg(zonedbuf, zonedlen, "ZONEDBUF");

			/* converte para ascii */
			conv_ascii(ascbuf, zonedbuf, zonedlen);
		}
		else {
			/* Campo nao compactado */
			zonedlen = detail[i].length;
			/* Converte para ascii */
			conv_ascii(ascbuf, xbuf, detail[i].length);
		}
		write(out, ascbuf, zonedlen);
		ascbuf[zonedlen] = '\0';
		if(gDebug)
			fprintf(dbgout, "Ascbuf: %s\n", ascbuf);
		p += detail[i].length;
		totalzlen += zonedlen;
		i++;
	}
	/* Calcula o numero de Bytes para completar o Logical Record Size */
	padlen = gLogicalRecSize - totalzlen;
	memset(auxbuf, 0x20, padlen);
	memset((auxbuf + padlen), '\n', 1);
	write(out,auxbuf, padlen + 1);
	if(gDebug) {
		fprintf(dbgout, "gLogicalRecSize %d totalzlen %d padlen %d\n"			,gLogicalRecSize, totalzlen, padlen);
	}

	return 1;
}

/* Converte cada trailer record para asc */
int conv_trailer(uchar * inb_p, int out)
{
	uchar xbuf[256];
	uchar zonedbuf[256];
	char ascbuf[256];
	int zonedlen, totalzlen, padlen;
	uchar *p;
	int i;
	i = 0;
	p = inb_p;
	totalzlen = 0;
	while(trailer[i].field != 0)
	{
		if(gDebug) {
			fprintf(dbgout, "\n%d\t%s\t%s\n",trailer[i].field, trailer[i].name, trailer[i].datatype);
		}
		memcpy(xbuf, p , trailer[i].length);
		if(gDebug) {
			DumpMsg(xbuf, trailer[i].length, "XBUF");
		}
		if(strstr(trailer[i].datatype,"COMP-3") != (char *) NULL) {
			if(gDebug)
				fprintf(dbgout, "Campo Compactado\n");
			/* Campo compactado */
			/* Converte de compactado para ebcdic zonado */
			zonedlen = to_zoned(zonedbuf, xbuf, trailer[i].length);
			if(gDebug)
				DumpMsg(zonedbuf, zonedlen, "ZONEDBUF");

			/* converte para ascii */
			conv_ascii(ascbuf, zonedbuf, zonedlen);
		}
		else {
			/* Campo nao compactado */
			zonedlen = trailer[i].length;
			/* Converte para ascii */
			if(i == 0) {
				ascbuf[0] = '9';
			}
			else
				conv_ascii(ascbuf, xbuf, trailer[i].length);
		}
		write(out, ascbuf, zonedlen);
		ascbuf[zonedlen] = '\0';
		if(gDebug)
			fprintf(dbgout, "Ascbuf: %s\n", ascbuf);
		p += trailer[i].length;
		totalzlen += zonedlen;
		i++;
	}
	/* Calcula o numero de Bytes para completar o Logical Record Size */
	padlen = gLogicalRecSize - totalzlen;
	memset(auxbuf, 0x20, padlen);
	memset((auxbuf + padlen), '\n', 1);
	write(out,auxbuf, padlen + 1);
	if(gDebug) {
		fprintf(dbgout, "gLogicalRecSize %d totalzlen %d padlen %d\n"			,gLogicalRecSize, totalzlen, padlen);
	}
	return 1;
}

/* Converte de formato compactado para zonado */
int to_zoned( uchar *dest, uchar *src, int packedlen)
{
        uchar low, hi, c;
        int len, zonedlen, i;


		len = packedlen - 1;
		zonedlen = 0;
        for( i = 0; len--; src++)
        {
                hi =  (*src & 0xF0) >> 4;
                *dest++ = ucharBase10Zoned(hi);
                zonedlen++;
                low = *src & 0x0F;
		*dest++ = ucharBase10Zoned(low);
		zonedlen++;
        }
	hi = (*src & 0xf0) >> 4;

        c = withSign(*src);
	if((c  - hi) == 0xd0) {
		if(gDebug)
			fprintf(dbgout, "Negativo\n");
		*dest = hi + 0xf0;
		dest++;
		*dest =	0x60;
		zonedlen++;
	}
	else {
		*dest = c;
	}

        zonedlen++;
	return zonedlen;
}

uchar ucharBase10Zoned(uchar nible)
{
		uchar num;
		num = nible;

		if (num <= 0x0a)	{			/* positive */
		    num += 0xf0;
		} else if (num == 0x0b)	{     	/* negative */
		    num += 0xd0;
		} else if (num == 0x0c)	{		/* positive */
		    num += 0xf0;
		} else if (num == 0x0d)	{		/* negative */
		    num += 0xd0;
		} else if (num == 0x0e)	{		/* positive */
			num += 0xf0;
		} else if (num == 0x0f)	{		/* positive */
			num += 0xf0;
		}
		return num;
}

/**
 * Description: left most 4 bits in an 8 bit byte converted to zoned format decimal uchar
 *             right most 4 bits in an 8 bit byte converted to zoned format sign
 */
uchar withSign(uchar hex)
{
		uchar hi,low, zoned;

        hi =  (hex & 0xF0) >> 4;
        low = hex & 0x0F;
        zoned = hi;

        switch((int)low) {
			case 0x0a:
			case 0x0c:
			case 0x0e:
			case 0x0f:
				zoned += 0xf0;
			break;
			case 0xb:
			case 0xd:
				zoned += 0xd0;
			break;
		}
        return zoned;
}

int  conv_ascii(
unsigned char * destino,
unsigned char * origem,
unsigned int tamanho
)
{

	char asciitab[] = {
		0x00,0x00,0x00,0x00,0x00,0x09,0x00,0x00,
		0x00,0x00,0x00,0x00,0x0C,0x0D,0x00,0x00,
		0x00,0x11,0x12,0x13,0x00,0x0A,0x00,0x00,
		0x00,0x19,0x00,0x00,0x1C,0x1D,0x1E,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x00,
		0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x63,0x2E,0x3C,0x28,0x2B,0x7C,
		0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x21,0x24,0x2A,0x29,0x3B,0x5E,
		0x2D,0x2F,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x7C,0x2C,0x25,0x5F,0x3E,0x3F,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x60,0x3A,0x23,0x40,0x27,0x3D,0x22,
		0x00,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
		0x68,0x69,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,
		0x71,0x72,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x7E,0x73,0x74,0x75,0x76,0x77,0x78,
		0x79,0x7A,0x00,0x00,0x00,0x5B,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x5D,0x00,0x5F,
		0x7B,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
		0x48,0x49,0x00,0x00,0x00,0x00,0x00,0x00,
		0x7D,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,
		0x51,0x52,0x00,0x00,0x00,0x00,0x00,0x00,
		0x5C,0x00,0x53,0x54,0x55,0x56,0x57,0x58,
		0x59,0x5A,0x00,0x00,0x00,0x00,0x00,0x00,
		0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
		0x38,0x39,0x00,0x00,0x00,0x00,0x00,0x00
	};


	unsigned char psibyte;
	unsigned int auxind;
	unsigned int index,tam_real;


#define CODDLE  0x10
#define CODSTX  0x02
#define CODETX  0x03
#define SYNC    0x32

#define CODNUL  0X20                            /* codigo nulo */

	/* tamanho = tamanho - 5;    ** 02 27 f5 c2 03 */

	index = 0;
	/* auxind = 4;             **  02 27 f5 c2 */
	auxind = 0;             /* DLE[01] STX[02]  */

	tam_real = tamanho;

	while(tamanho > 0)
	{
		/* converte ebcdic para ascii - ignora 'ordem'*/

		psibyte = origem[auxind];

		/*
              psibyte = prcv_buffer[auxind];
                        printf("-EBC:[%x]", origem[auxind]);
                        */

		switch(psibyte)         /* EBCDIC */
		{
		case CODDLE:
			tamanho--;   /* 0x10 */
			auxind++;
			if (psibyte = origem[auxind] == CODDLE)
			{
				tamanho--;   /* 0x10 */
				/*
                        printf("-DLE:[%x]\n", origem[auxind]);
						*/
				auxind++;
				tam_real--;
			}
			else
				if (psibyte = origem[auxind] == CODSTX)
				{
					tamanho--;   /* 0x02 */
					tam_real--;
					/*
                        printf("-STX:[%x]\n", origem[auxind]);
						*/
					auxind++;
					tam_real--;
				}
				else
					if (psibyte = origem[auxind] == CODETX)
					{
						tamanho--;   /* 0x03 */
						tam_real--;
						/*
                        printf("-ETX:[%x]\n", origem[auxind]);
						*/
						auxind++;
						tam_real--;
					}
			break;
		case SYNC:
			tamanho--;   /* 0x32 */
			auxind++;
			tam_real--;
			break;
		default:
			destino[auxind] = asciitab[psibyte];
			/*
                        printf("-ASC:[%c]\n", destino[auxind]);
                        */
			tamanho--;
			index++;
			auxind++;
		}
	}
	return(tam_real);
}

void DumpMsg( uchar *bp, int len, char *msg)
{
int     i;
char debugName[256];
uchar *p;
p = bp;

if(msg != NULL)
fprintf(dbgout, "******************  %s **************\n", msg);
fprintf(dbgout,"Tamanho: %d\n<", len);
for(i = 0; i < len; ++i, ++p)
{
if(i >  0 && (i % 70) == 0)
fprintf(dbgout, "\n");
fprintf(dbgout, "%02x", *p);
}
fprintf(dbgout,">\n");
fflush(dbgout);
}
const char *TimeStamp()
{
        time_t clock;
        struct tm *tms;
        static char buf[32];
        time(&clock);
        tms = localtime(&clock);
	if(tms->tm_year >= 100) tms->tm_year -= 100;
        sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d\0", tms->tm_year, tms->tm_mon + 1,
        tms->tm_mday, tms->tm_hour, tms->tm_min, tms->tm_sec);
        return buf;
}


int usage (char * prog)
{
	fprintf(stderr,"\nUso: \" %s\t\\\n", prog);
	fprintf(stderr,"\t\t [-d Debug ] \\\n");
	fprintf(stderr,"\t\t [-l LogicalRecordSize]\\\n");
	fprintf(stderr,"\t\t -t FileType \\\n");
	fprintf(stderr,"\t\t [-v ] [AsciiFile] \"\n");
	fprintf(stderr,"\tonde\n");
	fprintf(stderr,"\t\t \"LogicalRecordSize\" e o tamanho logico do registro\n");
	fprintf(stderr,"\t\t \"AsciiFile\" e o arquivo de saida a ser criado\n");
	fprintf(stderr,"\t\t \"FileType\" e o Bulk File \n");
	exit(1);
}


int main(int argc, char *argv[])
{
	char nome[256];
	char DebugName[256];
	char cmd[256];
	char pinbuf[512];
	char * BulkFileName;
	FILE *pin;
	int c;

	gDebug = 0;
	while ((c = getopt(argc, argv, OPTSTRING)) != EOF)	{
			switch (c)	{
				case 't':
				case 'T':
					BulkFileName = optarg;
				break;
				case 'd':
				case 'D':
					gDebug = 1;
				break;
				case 'l':
				case 'L':
					gLogicalRecSize = atoi(optarg);
					break;
				case '?':
					printf("Opcao desconhecida: -%c\n", optopt);
					usage(argv[0]);
			}
	}
	strncpy(nome,BulkFileName,4);
	strncpy(DebugName,BulkFileName,4);
	strcat(DebugName,".dbg");
	dbgout = fopen(DebugName, "w");
	fprintf(dbgout,"\n\n%s - Iniciou conversao do arquivo: %s\n\n", TimeStamp(), BulkFileName);
	/* Le a lista de arquivos compactados */
	Read_Comp_List();
	if(isComp(nome)) {
		/* Arquivo tem campos compactados */
		/* Le a descricao do Header */
		gHeaderLen = Read_Header_Desc(nome);
		/* Le a descricao do Detail Record */
		gDetailLen = Read_Detail_Desc(nome);
		/* Le a descricao do Trailer Record */
		gTrailerLen = Read_Trailer_Desc(nome);
		/* Converte o arquivo para ascii */
		conv(BulkFileName);
	}
	else {
		/* Arquivo nao e compactado */
		sprintf(cmd,"dd in=%s of=%s.asc conv=ascii", BulkFileName, BulkFileName);
		pin = popen(cmd, "r");
		while(fgets(pinbuf, 256, pin)) {
			printf(pinbuf);
		}
		pclose(pin);
	}
	fprintf(dbgout,"\n\n%s - Terminou conversao do arquivo: %s\n\n", TimeStamp(), BulkFileName);
	fclose(dbgout);
}

/* end of main() */



/*

	Run Length Encode Image

	4-FEB-83  W.J. Montgomery

*/

#include "stdio.h"



#define KEY 0xfe	/* encode identifier */
#define NUMBYTES 0x4000 /* number of bytes to encode */
#define TRUE 1
#define FALSE 0
#define TRANSLATE 0  /* text mode for I/O  */
#define NOTRANSLATE 0x8000  /* binary I/O mode */

extern	int	defseg, _fmode;

char lastchar;	/* flag from readbyte indicating last character */
int count1;	/* current screen pointer */
char screenbuf[NUMBYTES + 1000];
int bufcount; 	/* number of encoded bytes in screenbuf */


/* Readbyte(x) reads the next byte from the image buffer.
   If x=0 start read, else read next character. Set lastchar
   to indicate that char returned is last on screen    */

char readbyte(x)
{
	if(!(x)){
		defseg=0xb800;
		count1=0;
		}
	else{
		if(!(count1 < NUMBYTES)){
			 lastchar=TRUE;
			}
		  else lastchar=FALSE;

		return(peek(count1++));
		}
}


main1()
{
	FILE *fp,*fopen();
	int charcount;	/* similar characters counter */
	char last;	/* last character read */
	int notdone;
	char c;		/* current character */
	int t,r;


	_fmode = TRANSLATE;

	printf("\n");
	printf("	Encode Color Graphics Screen Utility\n\n");
	printf("	Version 1.2	     W.J. Montgomery\n\n");
	printf("\n\n   Encoding screen...\n");


	charcount=1;  /* similar character counter */
	last=KEY;     /* start with illegal value  */

	readbyte(0);	/* init screen read */
	notdone=TRUE;
	bufcount=0;

	while(notdone){
		if(bufcount>NUMBYTES){
			 printf(" *** Warning Buffer Full ***\n");
			 exit();
			}

		c=readbyte(1);  /* read next character */
		if(c == KEY){
			screenbuf[bufcount++]=KEY;
			screenbuf[bufcount++]=1;
			screenbuf[bufcount++]=KEY; /* encode key byte  */
			if(lastchar) notdone=FALSE;
			}
		else{
			if(c==last){
				++charcount;

				if(charcount==256){  /* check for max length */
					screenbuf[bufcount++]=KEY;
					screenbuf[bufcount++]=0; /* 256 */
					screenbuf[bufcount++]=last;
					charcount=1;
					last=KEY;
					if(lastchar) notdone=FALSE;
					}
				}
			else{
				if(charcount==1){
					if(last==KEY);
					else screenbuf[bufcount++]=last;
					}
				  else if(charcount==2){
					screenbuf[bufcount++]=last;
					screenbuf[bufcount++]=last;
					charcount=1;
					}
				    else{
					screenbuf[bufcount++]=KEY;
					screenbuf[bufcount++]=charcount;
					screenbuf[bufcount++]=last;
					charcount=1;
					}
				last=c;
				}
			}
		if(lastchar) notdone=FALSE;
		}
	screenbuf[bufcount++]=last;
	
	
	printf(" Screen compressed to %d characters from 16 K\n",bufcount);

	printf("\n\n   Writing to disk...\n");

	/* now dump buffer to disk */
	
	_fmode=NOTRANSLATE;  /* set non translated mode (binary) */
	if((fp = fopen("compress.bin","w")) == NULL){
		_fmode=TRANSLATE;
		printf("\n Can't open %s\n");
		exit(1);
		}
	else{
		c=bufcount & 255; /* write out length LSB */
		if((r=putc(c,fp))== -1){
			_fmode= TRANSLATE;
			printf(" Write error in output file...\n");
			exit(1);
			}

		c=bufcount >> 8; /* write out length MSB */
		if((r=putc(c,fp))== -1){
			_fmode= TRANSLATE;
			printf(" Write error in output file...\n");
			exit(1);
			}

		_fmode=NOTRANSLATE;
		for(t=0;t<bufcount;t++){
			c=screenbuf[t];
			if((r=putc(c,fp))== -1){
				_fmode=TRANSLATE;
				printf(" Write error in output file...\n");
				exit(1);
				}
			}
		if((r = fclose(fp))== -1){
			_fmode=TRANSLATE;
			printf(" Close file error...\n");
			exit(1);
			}
		}
	_fmode=TRANSLATE;
	printf("\n\neoj");

}

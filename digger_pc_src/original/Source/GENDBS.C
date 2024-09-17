
/*

		GENDBS
		------

	8-FEB-83	W.J. Montgomery

	Version 1.0


	This utility reads a run length encoded file
	and generates a  .asm file of the form:

	rlesize	dw	nnnnnn
	rledata	db	nnn,nnn,nnn,nnn,nnn,nnn,nnn,nnn
		db	nnn,nnn,nnn,nnn,nnn,nnn,nnn,nnn
		.
		.
		.
		db	nnn,nnn,nnn (...)

*/


#include <stdio.h>

#define NOTRANSLATE 0x8000
#define TRANSLATE 0


int _fmode;



main(argc,argv)
int argc;
char *argv[];
{
	FILE *infile,*outfile,*fopen();
	static unsigned numbytes;
	static unsigned count1,count2;
	static unsigned linecount;
	static int first;

	_fmode=TRANSLATE;

	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\n\n	Translate .rle File to .asm Source\n");
	printf("	------------------------------------");
	printf("\n\n 	Version 1.0		8-FEB-83\n\n");
	printf("	W.J. Montgomery\n\n");


	if(argc!=3){
		printf("Usage: A>gendbs infile outfile <cr> \n");
		exit();
		}

	_fmode=NOTRANSLATE;
	if ((infile=fopen(argv[1],"r")) == NULL){
		_fmode=TRANSLATE;
		printf("\n Cannot open %s for reading from.\n",argv[1]);
		exit(1);
		}
	if ((outfile=fopen(argv[2],"w")) == NULL){
		_fmode=TRANSLATE;
		printf("\n Cannot open %s for writing to.\n",argv[2]);
		exit(1);
		}

	/* read number of bytes of data in rle file */
	numbytes=getc(infile);
	numbytes+= (getc(infile))<<8;
	fprintf(outfile,"rlesize\tdw\t%5d\r",numbytes);

	first=1;
	linecount=1;  /* one line written so far */
	count1=0;
	while(count1<numbytes){
		if(first){
			fprintf(outfile,"rledata\tdb\t");
			first=0;
			}
		else fprintf(outfile,"\tdb\t");
		for(count2=0;count2<8;count2++){
			if(count1>=numbytes) fprintf(outfile,"000");
			else fprintf(outfile,"%03d",getc(infile));
			if(count2<7) fprintf(outfile,",");
			count1++;
			}
		fprintf(outfile,"\r");
		linecount++;
		if((linecount&0x1f)==0){
			putch('*');
			}
		}



	fclose(infile);
	fclose(outfile);
	_fmode=TRANSLATE;
	printf("\n %d lines written to %s\n\n",linecount,argv[2]);
	printf("\neoj\n");


}


/*  non-encoded screen image load utility 

    2-FEB-83  W.J. Montgomery

*/


#include <stdio.h>

#define mode     0x3d8
#define colorsel 0x3d9
#define blackfor 0x20
#define m6845add 0x3d4
#define m6845dat 0x3d5

extern	int	defseg;

int _fmode = 0x8000; /* set disk I/O to non-translated (binary) mode */
char data[14]={0x38,0x28,0x2d,0x0a,0x7f,6,0x64,0x70,2,1,6,7,0,0};


setmedres()
{
	static int count;

	outp(mode,0); /* turn off video */

	outp(colorselect,blackfor); /* set border to black */

	/* setup 6845 */
	for(count=0;count<14;count++){
		outp(m6845add,count);
		outp(m6845data,data[count]);
		}

	defseg=0xb800;


	outp(mode,0x0a);
}


main(argc, argv)
int argc;
char *argv[];
{
    FILE *fp, *fopen();
    static int c,skipcount;
    static int spointer;

    if (argc != 2){      /* no input file specified  */
	printf("Usage A>loadima <filename> cr  \n");
	exit();
	}


    else{
	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("\n can't open %s\n", argv[1]);
		exit(1);
	    }
	 else {
	    spointer=0;
	    defseg=0xb800;
	    skipcount=0;
	    setmedres();

	    while( ((c = getc(fp)) != EOF) && (spointer<0x4000)){
		if(skipcount<7) skipcount++;
		else{
			poke(spointer++,c);
			if(spointer>0x4000){
				printf("\nScreen overflow\n");
				break;
				}
			}
		}
 	    fclose(fp);
	    }
	}
	main1();
}

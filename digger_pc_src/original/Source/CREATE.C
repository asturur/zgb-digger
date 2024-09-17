
#include	<stdio.h>

#define		FALSE	0
#define		TRUE	(!0)

extern	char	diskdat[];
char	bellstring []= { 7,7,7,7,7,7,0 };

int	 stat, errstat, length, loop, loop2, temp, c, endflag, start;
int	_fmode=0x8000; /* set disk to binary mode */
long	checksum;


main(argc, argv)
int	argc;
char	*argv[];
{

	FILE	*fp, *fopen();
	
	long	count;

			
	start=TRUE;
	while(start) {
		start=FALSE;	/* only loop one time */
		errstat=FALSE;	/* reset real error flag */

		stat=format();	/* format diskette */
		if(!stat) {
			errstat=TRUE;	/* Flag that an error occured */
			break;
		}

		stat=bootw();	/* create boot sectors */
		if(!stat) {
			errstat=TRUE;	/* Flaf that an error occured */
			break;
		}

		if(argc != 2) {	/* input file not specified */
			cprintf("\n\rUsage  A>Create <filename> cr  \n\r");	
			cprintf(bellstring);
			exit();
		}
		else {
			if((fp = fopen(argv[1], "r")) == NULL) {
				cprintf("\n\r can't open %s\n", argv[1]);
				cprintf(bellstring);
				exit(1);
			}
			else {

				count=0; checksum=0;
				endflag=FALSE;
				for(loop=0; loop<15; loop++) {	/* read up to 60K bytes */
					loop2=0;
					while(loop2<0x1000 && !endflag) {
						if((c=getc(fp)) !=EOF) {
							diskdat[loop2]=c;
							checksum+=c;
							count++;
							loop2++;
						}
						else
							endflag=TRUE;
					}					
					if(loop2) {	/* write new track if any data was read in */
						stat=trackw(23+loop);	/* write track on drive b: */ 
						if(!stat)
							errstat=TRUE;	/* Flaf that an error occured */
					}
				}
			}
		}
		fclose(fp);

	}
	if(!errstat)
		cprintf("\n\rRecord length = %ld    Checksum = %ld \n\r",count, checksum);
	else {
		cprintf("\n\rD i s k     E r r o r  \n\r");
		cprintf(bellstring);	/* ring bell */
	}

}



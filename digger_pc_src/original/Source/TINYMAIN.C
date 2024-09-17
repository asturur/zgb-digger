/**
*
* This module defines a version of _main which processes the
* command line for arguments but does not open "stdin", "stdout",
* and "stderr".  Since these files are not opened, the library
* functions "printf" and "scanf" will not work; however, the
* console functions "cprintf" and "cscanf" can be used instead.
*
**/
#define MAXARG 32		/* maximum command line arguments */

_main(line)
char *line;
{
static int argc = 0;
static char *argv[MAXARG];

main(argc, argv);	/* call main function */
_exit(0);
}

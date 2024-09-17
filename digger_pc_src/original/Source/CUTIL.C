

abs(value)
int	value;
{	/* return the absolute value */
	if(value<0)
		value=-value;
	return(value);
}




sleep(time)
int	time;
{
	int	a,b;

	for(a=0; a<time; a++) {
		for(b=0; b<100; b++);
	}
}

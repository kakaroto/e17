char * strlwr(char * str)
{
	char * i;

	if (!str)
		return 0;

	for (i = str; i[0]; i++)
		i[0] = tolower(i[0]);

	return str;
}

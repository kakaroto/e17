#include <stdio.h>
#include <string.h>
#include <Ewd.h>

#define WORDLEN 1024
#define DICTIONARY "/usr/share/dict/words"

int main()
{
	FILE *dict;
	char buffer[WORDLEN];
	Ewd_Hash *hash = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	dict = fopen(DICTIONARY, "r");
	if (!dict) {
		perror("fopen");
		exit(1);
	}

	while (fgets(buffer, WORDLEN, dict))
		ewd_hash_set(hash, strdup(buffer), 0);

	ewd_hash_dump_graph(hash);

	fclose(dict);
}

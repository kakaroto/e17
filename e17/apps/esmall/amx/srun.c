/*  Command-line shell for the "Small" Abstract Machine using JIT compiler.

 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  Copyright (c) Mark Peter, 1998-1999
 *
 *  This file may be freely used. No warranties of any kind.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>				/* for memset() (on some compilers) */
#include <time.h>
#include "amx.h"


void core_Init(void);			/* two functions from AMX_CORE.C */
void core_Exit(void);


void *loadprogram(AMX * amx, char *filename)
{

	FILE *fp;
	AMX_HEADER hdr;
	void *program;

	if ((fp = fopen(filename, "rb")) != NULL) {
		fread(&hdr, sizeof hdr, 1, fp);
		if ((program = malloc((int) hdr.stp)) != NULL) {
			rewind(fp);
			fread(program, 1, (int) hdr.size, fp);
			fclose(fp);
			memset(amx, 0, sizeof *amx);
			if (amx_Init(amx, program) != AMX_ERR_NONE) {
				free(program);
				return NULL;
			}
			/* standard virtual machine (no JIT) */
			return program;
		}						/* if */
	}							/* if */
	return NULL;

}



int main(int argc, char *argv[])
{

	extern AMX_NATIVE_INFO core_Natives[];
	extern AMX_NATIVE_INFO console_Natives[];

	AMX amx;
	cell ret;
	int err;
	void *program;
	clock_t start, end;

	if (argc != 2 || (program = loadprogram(&amx, argv[1])) == NULL) {
		printf("Usage: SRUN <filename>\n\n"
			   "The filename must include the extension\n");
		return 1;
	}							/* if */
	amx_Register(&amx, core_Natives, -1);
	core_Init();
	err = amx_Register(&amx, console_Natives, -1);

	start = clock();
	if (err == AMX_ERR_NONE)
		err = amx_Exec(&amx, &ret, AMX_EXEC_MAIN, 0);
	end = clock();

	if (err != AMX_ERR_NONE)
		printf("Run time error %d on line %ld\n", err, amx.curline);
	else if (ret != 0)
		printf("%s returns %ld\n", argv[1], (long) ret);

	free(program);
	core_Exit();

	printf("Run time: %.2f seconds", (double) (end - start) / CLK_TCK);
	return 0;
}

/*  Abstract Machine for the Small compiler

 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */
#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include "amx.h"
#include "osdefs.h"

typedef unsigned char uchar;

typedef unsigned short ushort;


#define _namemax        19
typedef struct {

	cell address;

	char name[_namemax + 1];

} AMX_FUNCSTUB;



typedef enum {

	OP_NONE,					/* invalid opcode */
	OP_LOAD_PRI,
	OP_LOAD_ALT,
	OP_LOAD_S_PRI,
	OP_LOAD_S_ALT,
	OP_LREF_PRI,
	OP_LREF_ALT,
	OP_LREF_S_PRI,
	OP_LREF_S_ALT,
	OP_LOAD_I,
	OP_LODB_I,
	OP_CONST_PRI,
	OP_CONST_ALT,
	OP_ADDR_PRI,
	OP_ADDR_ALT,
	OP_STOR_PRI,
	OP_STOR_ALT,
	OP_STOR_S_PRI,
	OP_STOR_S_ALT,
	OP_SREF_PRI,
	OP_SREF_ALT,
	OP_SREF_S_PRI,
	OP_SREF_S_ALT,
	OP_STOR_I,
	OP_STRB_I,
	OP_LIDX,
	OP_LIDX_B,
	OP_IDXADDR,
	OP_IDXADDR_B,
	OP_ALIGN_PRI,
	OP_ALIGN_ALT,
	OP_LCTRL,
	OP_SCTRL,
	OP_MOVE_PRI,
	OP_MOVE_ALT,
	OP_XCHG,
	OP_PUSH_PRI,
	OP_PUSH_ALT,
	OP_PUSH_R,
	OP_PUSH_C,
	OP_PUSH,
	OP_PUSH_S,
	OP_POP_PRI,
	OP_POP_ALT,
	OP_STACK,
	OP_HEAP,
	OP_PROC,
	OP_RET,
	OP_RETN,
	OP_CALL,
	OP_CALL_PRI,
	OP_JUMP,
	OP_JREL,
	OP_JZER,
	OP_JNZ,
	OP_JEQ,
	OP_JNEQ,
	OP_JLESS,
	OP_JLEQ,
	OP_JGRTR,
	OP_JGEQ,
	OP_JSLESS,
	OP_JSLEQ,
	OP_JSGRTR,
	OP_JSGEQ,
	OP_SHL,
	OP_SHR,
	OP_SSHR,
	OP_SHL_C_PRI,
	OP_SHL_C_ALT,
	OP_SHR_C_PRI,
	OP_SHR_C_ALT,
	OP_SMUL,
	OP_SDIV,
	OP_SDIV_ALT,
	OP_UMUL,
	OP_UDIV,
	OP_UDIV_ALT,
	OP_ADD,
	OP_SUB,
	OP_SUB_ALT,
	OP_AND,
	OP_OR,
	OP_XOR,
	OP_NOT,
	OP_NEG,
	OP_INVERT,
	OP_ADD_C,
	OP_SMUL_C,
	OP_ZERO_PRI,
	OP_ZERO_ALT,
	OP_ZERO,
	OP_ZERO_S,
	OP_SIGN_PRI,
	OP_SIGN_ALT,
	OP_EQ,
	OP_NEQ,
	OP_LESS,
	OP_LEQ,
	OP_GRTR,
	OP_GEQ,
	OP_SLESS,
	OP_SLEQ,
	OP_SGRTR,
	OP_SGEQ,
	OP_EQ_C_PRI,
	OP_EQ_C_ALT,
	OP_INC_PRI,
	OP_INC_ALT,
	OP_INC,
	OP_INC_S,
	OP_INC_I,
	OP_DEC_PRI,
	OP_DEC_ALT,
	OP_DEC,
	OP_DEC_S,
	OP_DEC_I,
	OP_MOVS,
	OP_CMPS,
	OP_FILL,
	OP_HALT,
	OP_BOUNDS,
	OP_SYSREQ_PRI,
	OP_SYSREQ_C,
	OP_FILE,
	OP_LINE,
	OP_SYMBOL,
	OP_SRANGE,
	OP_JUMP_PRI,
	OP_SWITCH,
	OP_CASETBL,
	// -----
	OP_NUM_OPCODES
} OPCODE;



int amx_LittleEndian;			/* set to true if the computer is little-endian */


static void init_little_endian(void)
{

	ushort val = 0x00ff;

	uchar *ptr = (uchar *) & val;


	/* "ptr" points to the starting address of "val". If that address
	 * holds the byte "0xff", the computer stored the low byte of "val"
	 * at the lower address, and so the memory lay out is Little Endian.
	 */
	assert(*ptr == 0xff || *ptr == 0x00);

	amx_LittleEndian = *ptr == 0xff;

}


static ucell *aligncell(ucell * v)
{

	unsigned char *s = (unsigned char *) v;

	unsigned char t;


#if defined BIT16
	assert(sizeof(cell) == 2);

	/* swap two bytes */
	t = s[0];
	s[0] = s[1];
	s[1] = t;

#else							/*  */
	assert(sizeof(cell) == 4);

	/* swap outer two bytes */
	t = s[0];
	s[0] = s[3];
	s[3] = t;

	/* swap inner two bytes */
	t = s[1];
	s[1] = s[2];
	s[2] = t;

#endif							/*  */
	return v;

}


int amx_Flags(AMX * amx, unsigned short *flags)
{

	AMX_HEADER *hdr;


	*flags = 0;

	if (amx == NULL)
		return AMX_ERR_FORMAT;

	hdr = (AMX_HEADER *) amx->base;

	if ((unsigned short) hdr->magic != AMX_MAGIC)
		return AMX_ERR_FORMAT;

	*flags = hdr->flags;

	return AMX_ERR_NONE;

}


int amx_Callback(AMX * amx, cell index, cell * result, cell * params)
{

	AMX_HEADER *hdr = (AMX_HEADER *) amx->base;
	AMX_FUNCSTUB *func = (AMX_FUNCSTUB *) (amx->base + (int) hdr->natives + (int) index * sizeof(AMX_FUNCSTUB));
	AMX_NATIVE f = (AMX_NATIVE) func->address;
	assert(f != NULL);
	assert(index < hdr->num_natives);

	/* Note:
	 *   params[0] == number of parameters passed to the native function
	 *   params[1] == first argument
	 *   etc.
	 */

	amx->error = AMX_ERR_NONE;
	*result = f(amx, params);

	return amx->error;

}


int amx_Debug(AMX * amx)
{

	amx = NULL;
	return AMX_ERR_DEBUG;

}


#if defined JIT
extern int getMaxCodeSize(void);

extern int asm_runJIT(void *sourceAMXbase, void *jumparray, void *compiledAMXbase);

#endif							/*  */

#define DBGPARAM(v)     ( v=*(cell *)(code+(int)cip), cip+=sizeof(cell) )

static int amx_BrowseRelocate(AMX * amx)
{

	AMX_HEADER *hdr;
	uchar *code;
	cell cip;
	long codesize;
	OPCODE op;
	int debug;

#if !defined NODBGCALLS
	int last_sym_global = 0;
#endif							/*  */
#if defined __GNUC__ || defined ASM32 || defined JIT
	ucell **opcode_list;
#endif							/*  */
#if defined JIT
	int opcode_count = 0;
	int reloc_count = 0;
#endif							/*  */

	hdr = (AMX_HEADER *) amx->base;
	code = amx->base + (int) hdr->cod;
	codesize = hdr->dat - hdr->cod;

	/* sanity checks */
	assert(OP_PUSH_PRI == 36);
	assert(OP_PROC == 46);
	assert(OP_SHL == 65);
	assert(OP_SMUL == 72);
	assert(OP_EQ == 95);
	assert(OP_INC_PRI == 107);
	assert(OP_MOVS == 117);
	assert(OP_SYMBOL == 126);

#if !defined NODBGCALLS
	amx->dbgcode = DBG_INIT;
	assert(amx->flags == 0);
	amx->flags = AMX_FLAG_BROWSE;
	debug = amx->debug(amx) == AMX_ERR_NONE;
	if (debug)
		amx->flags = AMX_FLAG_DEBUG | AMX_FLAG_BROWSE;
#endif							/*  */

#if defined __GNUC__ || defined ASM32 || defined JIT
	amx_Exec(NULL, (cell *) & opcode_list, 0, 0);
#endif							/*  */

	/* start browsing code */
	for (cip = 0; cip < codesize;) {
		op = (OPCODE) * (ucell *) (code + (int) cip);
		assert(op > 0 && op < OP_NUM_OPCODES);
		if (op >= 256)
			return AMX_ERR_INVINSTR;
#if defined __GNUC__ || defined ASM32 || defined JIT
		/* relocate symbol */
		*(ucell **) (code + (int) cip) = opcode_list[op];
#endif							/*  */
#if defined JIT
		opcode_count++;
#endif							/*  */
		cip += sizeof(cell);
		switch (op) {
			case OP_LOAD_PRI:	/* instructions with 1 parameter */
			case OP_LOAD_ALT:
			case OP_LOAD_S_PRI:
			case OP_LOAD_S_ALT:
			case OP_LREF_PRI:
			case OP_LREF_ALT:
			case OP_LREF_S_PRI:
			case OP_LREF_S_ALT:
			case OP_LODB_I:
			case OP_CONST_PRI:
			case OP_CONST_ALT:
			case OP_ADDR_PRI:
			case OP_ADDR_ALT:
			case OP_STOR_PRI:
			case OP_STOR_ALT:
			case OP_STOR_S_PRI:
			case OP_STOR_S_ALT:
			case OP_SREF_PRI:
			case OP_SREF_ALT:
			case OP_SREF_S_PRI:
			case OP_SREF_S_ALT:
			case OP_STRB_I:
			case OP_LIDX_B:
			case OP_IDXADDR_B:
			case OP_ALIGN_PRI:
			case OP_ALIGN_ALT:
			case OP_LCTRL:
			case OP_SCTRL:
			case OP_PUSH_R:
			case OP_PUSH_C:
			case OP_PUSH:
			case OP_PUSH_S:
			case OP_STACK:
			case OP_HEAP:
			case OP_JREL:
			case OP_SHL_C_PRI:
			case OP_SHL_C_ALT:
			case OP_SHR_C_PRI:
			case OP_SHR_C_ALT:
			case OP_ADD_C:
			case OP_SMUL_C:
			case OP_ZERO:
			case OP_ZERO_S:
			case OP_EQ_C_PRI:
			case OP_EQ_C_ALT:
			case OP_INC:
			case OP_INC_S:
			case OP_DEC:
			case OP_DEC_S:
			case OP_MOVS:
			case OP_CMPS:
			case OP_FILL:
			case OP_HALT:
			case OP_BOUNDS:
			case OP_SYSREQ_C:
				cip += sizeof(cell);
				break;

			case OP_LOAD_I:	/* instructions without parameters */
			case OP_STOR_I:
			case OP_LIDX:
			case OP_IDXADDR:
			case OP_MOVE_PRI:
			case OP_MOVE_ALT:
			case OP_XCHG:
			case OP_PUSH_PRI:
			case OP_PUSH_ALT:
			case OP_POP_PRI:
			case OP_POP_ALT:
			case OP_PROC:
			case OP_RET:
			case OP_RETN:
			case OP_CALL_PRI:
			case OP_SHL:
			case OP_SHR:
			case OP_SSHR:
			case OP_SMUL:
			case OP_SDIV:
			case OP_SDIV_ALT:
			case OP_UMUL:
			case OP_UDIV:
			case OP_UDIV_ALT:
			case OP_ADD:
			case OP_SUB:
			case OP_SUB_ALT:
			case OP_AND:
			case OP_OR:
			case OP_XOR:
			case OP_NOT:
			case OP_NEG:
			case OP_INVERT:
			case OP_ZERO_PRI:
			case OP_ZERO_ALT:
			case OP_SIGN_PRI:
			case OP_SIGN_ALT:
			case OP_EQ:
			case OP_NEQ:
			case OP_LESS:
			case OP_LEQ:
			case OP_GRTR:
			case OP_GEQ:
			case OP_SLESS:
			case OP_SLEQ:
			case OP_SGRTR:
			case OP_SGEQ:
			case OP_INC_PRI:
			case OP_INC_ALT:
			case OP_INC_I:
			case OP_DEC_PRI:
			case OP_DEC_ALT:
			case OP_DEC_I:
			case OP_SYSREQ_PRI:
			case OP_JUMP_PRI:
				break;

			case OP_CALL:		/* opcodes that need relocation */
			case OP_JUMP:
			case OP_JZER:
			case OP_JNZ:
			case OP_JEQ:
			case OP_JNEQ:
			case OP_JLESS:
			case OP_JLEQ:
			case OP_JGRTR:
			case OP_JGEQ:
			case OP_JSLESS:
			case OP_JSLEQ:
			case OP_JSGRTR:
			case OP_JSGEQ:
			case OP_SWITCH:
#if defined JIT
				reloc_count++;
#endif							/*  */
				*(ucell *) (code + (int) cip) += (ucell) code;
				cip += sizeof(cell);
				break;

			case OP_FILE:
				{
					cell num;
					DBGPARAM(num);
					DBGPARAM(amx->curfile);
					amx->dbgcode = DBG_FILE;
					amx->dbgname = (char *) (code + (int) cip);
					cip += num - sizeof(cell);
#if !defined NODBGCALLS
					if (debug) {
						assert(amx->flags == (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
						amx->debug(amx);
					}			/* if */
#endif
					break;
				}				/* case */

			case OP_LINE:
				amx->dbgcode = DBG_LINE;
				DBGPARAM(amx->curline);
				DBGPARAM(amx->curfile);
#if !defined NODBGCALLS
				if (debug) {
					assert(amx->flags == (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
					amx->debug(amx);
				}				/* if */
#endif
				break;

			case OP_SYMBOL:
				{
					cell num;
					DBGPARAM(num);
					DBGPARAM(amx->dbgaddr);
					DBGPARAM(amx->dbgparam);
					amx->dbgcode = DBG_SYMBOL;
					amx->dbgname = (char *) (code + (int) cip);
					cip += num - 2 * sizeof(cell);
#if !defined NODBGCALLS
					last_sym_global = (amx->dbgparam >> 8) == 0;
					if (debug && last_sym_global) {	/* do global symbols only */
						assert(amx->flags == (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
						amx->debug(amx);
					}			/* if */
#endif
					break;
				}				/* case */

			case OP_SRANGE:
				DBGPARAM(amx->dbgaddr);		/* dimension level */
				DBGPARAM(amx->dbgparam);	/* length */
				amx->dbgcode = DBG_SRANGE;
#if !defined NODBGCALLS
				if (debug && last_sym_global) {		/* do global symbols only */
					assert(amx->flags == (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
					amx->debug(amx);
				}				/* if */
#endif
				break;

			case OP_CASETBL:
				{
					cell num;
					int i;

					DBGPARAM(num);	/* number of records follows the opcode */
					for (i = 0; i <= num; i++) {
						*(ucell *) (code + (int) cip + 2 * i * sizeof(cell)) += (ucell) code;
#if defined JIT
						reloc_count++;
#endif							/*  */
					}			/* for */
					cip += (2 * num + 1) * sizeof(cell);
					break;
				}				/* case */

			default:
				return AMX_ERR_INVINSTR;
		}						/* switch */
	}							/* for */

#if defined JIT
	amx->code_size = getMaxCodeSize() * opcode_count + hdr->cod
		+ (hdr->stp - hdr->dat) + 4;
	amx->reloc_size = 2 * sizeof(cell) * reloc_count;
#endif							/*  */
	amx->flags |= AMX_FLAG_RELOC;

	return AMX_ERR_NONE;
}


int amx_Init(AMX * amx, void *program)
{

	AMX_HEADER *hdr;

	hdr = (AMX_HEADER *) program;

	if ((unsigned short) hdr->magic != 0xf1e0 || hdr->defsize != sizeof(AMX_FUNCSTUB))
		return AMX_ERR_FORMAT;
	if (hdr->amx_version > AMX_VERSION)
		return AMX_ERR_VERSION;
	assert(hdr->hea == hdr->size);
	if (hdr->stp >= INT_MAX || hdr->stp <= 0)
		return AMX_ERR_FORMAT;
	if ((hdr->flags & 1) != 0)
		return AMX_ERR_FORMAT;	/* 16-bit characters currently not supported */

	amx->base = (unsigned char *) program;

	/* Set a zero cell at the top of the stack, which functions
	 * as a sentinel for strings.
	 */
	hdr->stp -= sizeof(cell);
	*(cell *) (amx->base + (int) hdr->stp) = 0;

	/* set initial values */
	amx->hea = hdr->hea - hdr->dat;/* stack and heap relative to data segment */
	amx->stp = hdr->stp - hdr->dat;
	amx->stk = amx->stp;
	if (amx->callback == NULL)
		amx->callback = amx_Callback;
	if (amx->debug == NULL)
		amx->debug = amx_Debug;
	amx->curline = 0;
	amx->curfile = 0;

	/* check whether the computer is big endian */
	init_little_endian();

	/* relocate call and jump instructions,
	 * optionally gather debug information
	 */
	amx_BrowseRelocate(amx);


	return AMX_ERR_NONE;

}


#if defined JIT

#if defined __WIN32__			/* this also applies to Win32 "console" applications */

int memoryFullAccess(void *addr, int len)
{
	DWORD op;

	if (VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &op))
		return op;
	return 0;
}


int memorySetAccess(void *addr, int len, int access)
{
	DWORD op;

	if (access == 0)
		return 0;
	return VirtualProtect(addr, len, access, &op);
}


#else							/* #if defined __WIN32 __ */

/* TODO: Add cases for Linux, Unix, OS/2, ... */
/* DOS32 has no imposed limits on its segments. */

int memoryFullAccess(void *addr, int len)
{
	return 1;
}


int memorySetAccess(void *addr, int len, int access)
{
	return 1;
}


#endif							/* #if defined __WIN32 __ */

int amx_InitJIT(AMX * amx, void *reloc_table, void *native_code)
{

	int mac, res;
	AMX_HEADER *hdr;

	mac = memoryFullAccess(asm_runJIT, 20000);
	if (!mac)
		return AMX_ERR_INIT_JIT;

	/* copy the prefix */
	memcpy(native_code, amx->base, ((AMX_HEADER *) (amx->base))->cod);
	hdr = native_code;

	/* JIT rulz! (TM) */
	/* MP: added check for correct compilation */
	res = asm_runJIT(amx->base, reloc_table, native_code);
	if (res != 0) {
		memorySetAccess(asm_runJIT, 20000, mac);
		return AMX_ERR_INIT_JIT;
	}
	/* update the required memory size (the previous value was a
	 * conservative estimate, now we know the exact size)
	 */
	amx->code_size = (4 + hdr->dat + hdr->stp) & ~3;

	/* The compiled code is relocatable, since only relative jumps are
	 * used for destinations within the generated code and absoulute
	 * addresses for jumps into the runtime, which is fixed in memory.
	 */
	amx->base = (unsigned char *) native_code;
	amx->cip = hdr->cip;
	amx->hea = hdr->hea;
	amx->stp = hdr->stp;

	/* also put a sentinel for strings at the top the stack */
	*(cell *) ((char *) native_code + hdr->dat + hdr->stp) = 0;
	amx->stk = amx->stp;

	memorySetAccess(asm_runJIT, 20000, mac);
	return AMX_ERR_NONE;
}


#else							/* #if defined JIT */

int amx_InitJIT(AMX * amx, void *compiled_program, void *reloc_table)
{
	amx = NULL;
	compiled_program = NULL;
	reloc_table = NULL;

	return AMX_ERR_INIT_JIT;
}


#endif							/* #if defined JIT */

int amx_NameLength(AMX * amx, int *length)
{
	AMX_HEADER *hdr = (AMX_HEADER *) amx->base;
	assert(hdr != NULL);
	*length = hdr->defsize - sizeof(cell);
	return AMX_ERR_NONE;
}

int amx_NumPublics(AMX * amx, int *number)
{
	AMX_HEADER *hdr = (AMX_HEADER *) amx->base;
	assert(hdr != NULL);
	*number = hdr->num_publics;
	return AMX_ERR_NONE;
}

int amx_GetPublic(AMX * amx, int index, char *funcname)
{
	AMX_HEADER *hdr;
	AMX_FUNCSTUB *func;

	hdr = (AMX_HEADER *) amx->base;
	assert(hdr != NULL);
	if (index >= hdr->num_publics)
		return AMX_ERR_INDEX;

	func = (AMX_FUNCSTUB *) (amx->base + (int) hdr->publics + index * sizeof(AMX_FUNCSTUB));
	strcpy(funcname, func->name);
	return AMX_ERR_NONE;
}


int amx_FindPublic(AMX * amx, char *name, int *index)
{
	int first, last, mid, result;
	char pname[40];

	amx_NumPublics(amx, &last);
	last--;		/* last valid index is 1 less than the number of functions */
	first = 0;
	/* binary search */
	while (first <= last) {
		mid = (first + last) / 2;
		amx_GetPublic(amx, mid, pname);
		result = strcmp(pname, name);
		if (result > 0) {
			last = mid - 1;
		} else if (result < 0) {
			first = mid + 1;
		} else {
			*index = mid;
			return AMX_ERR_NONE;
		}						/* if */
	}							/* while */
	/* not found, set to an invalid index, so amx_Exec() will fail */
	*index = INT_MAX;
	return AMX_ERR_NOTFOUND;
}

int amx_GetUserData(AMX * amx, long tag, void **ptr)
{
	int index;

	assert(amx != NULL);
	assert(tag != 0);
	for (index = 0; index < AMX_USERNUM && amx->usertags[index] != tag; index++)
		/* nothing */ ;
	if (index >= AMX_USERNUM)
		return AMX_ERR_USERDATA;
	*ptr = amx->userdata[index];
	return AMX_ERR_NONE;
}

int amx_SetUserData(AMX * amx, long tag, void *ptr)
{
	int index;

	assert(amx != NULL);
	assert(tag != 0);
	/* try to find existing tag */
	for (index = 0; index < AMX_USERNUM && amx->usertags[index] != tag; index++)
		/* nothing */ ;
	/* if not found, try to find empty tag */
	if (index >= AMX_USERNUM)
		for (index = 0; index < AMX_USERNUM && amx->usertags[index] != 0; index++)
			/* nothing */ ;
	/* if still not found, quit with error */
	if (index >= AMX_USERNUM)
		return AMX_ERR_INDEX;
	/* set the tag and the value */
	amx->usertags[index] = tag;
	amx->userdata[index] = ptr;
	return AMX_ERR_NONE;
}

static AMX_NATIVE findfunction(char *name, AMX_NATIVE_INFO * list, int number)
{
	int i;

	for (i = 0; list[i].name != NULL && (i < number || number == -1); i++)
		if (strcmp(name, list[i].name) == 0)
			return list[i].func;
	return NULL;
}

int amx_Register(AMX * amx, AMX_NATIVE_INFO * list, int number)
{
	AMX_FUNCSTUB *func;
	AMX_HEADER *hdr;
	int i, err;
	AMX_NATIVE funcptr;

	hdr = (AMX_HEADER *) amx->base;
	assert(hdr != NULL);

	err = AMX_ERR_NONE;
	func = (AMX_FUNCSTUB *) (amx->base + (int) hdr->natives);
	for (i = 0; i < hdr->num_natives; i++) {
		if (func->address == 0) {
			/* this function is not yet located */
			funcptr = findfunction(func->name, list, number);
			if (funcptr != NULL)
				func->address = (ucell) funcptr;
			else
				err = AMX_ERR_NOTFOUND;
		}						/* if */
		func++;
	}							/* for */
	return err;
}

AMX_NATIVE_INFO *amx_NativeInfo(char *name, AMX_NATIVE func)
{
	static AMX_NATIVE_INFO n;
	n.name = name;
	n.func = func;
	return &n;
}

#define GETPARAM(v)     ( v=*(cell *)cip++ )
#define PUSH(v)         ( stk-=sizeof(cell), *(cell *)(data+(int)stk)=v )
#define POP(v)          ( v=*(cell *)(data+(int)stk), stk+=sizeof(cell) )

#define STKMARGIN       ((cell)(16*sizeof(cell)))
#if defined NOBOUNDSCHK
#define CHKMARGIN()
#define CHKSTACK()
#define CHKHEAP()
#else							/*  */
#define CHKMARGIN()   if (hea+STKMARGIN>stk) return AMX_ERR_STACKERR
#define CHKSTACK()    if (stk>hdr->stp) return AMX_ERR_STACKLOW
#define CHKHEAP()     if (hea<0) return AMX_ERR_HEAPLOW
#endif							/*  */

#if defined __GNUC__
/* GNU C version uses the "labels as values" extension to create
 * fast "indirect threaded" interpreter.
 */

#define NEXT(cip)       goto **cip++

int amx_Exec(AMX * amx, cell * retval, int index, int numparams,...)
{
	static void *labels[] =
	{
		&&op_none, &&op_load_pri, &&op_load_alt, &&op_load_s_pri,
		&&op_load_s_alt, &&op_lref_pri, &&op_lref_alt, &&op_lref_s_pri,
		&&op_lref_s_alt, &&op_load_i, &&op_lodb_i, &&op_const_pri,
		&&op_const_alt, &&op_addr_pri, &&op_addr_alt, &&op_stor_pri,
		&&op_stor_alt, &&op_stor_s_pri, &&op_stor_s_alt, &&op_sref_pri,
		&&op_sref_alt, &&op_sref_s_pri, &&op_sref_s_alt, &&op_stor_i,
		&&op_strb_i, &&op_lidx, &&op_lidx_b, &&op_idxaddr,
		&&op_idxaddr_b, &&op_align_pri, &&op_align_alt, &&op_lctrl,
		&&op_sctrl, &&op_move_pri, &&op_move_alt, &&op_xchg,
		&&op_push_pri, &&op_push_alt, &&op_push_r, &&op_push_c,
		&&op_push, &&op_push_s, &&op_pop_pri, &&op_pop_alt,
		&&op_stack, &&op_heap, &&op_proc, &&op_ret,
		&&op_retn, &&op_call, &&op_call_pri, &&op_jump,
		&&op_jrel, &&op_jzer, &&op_jnz, &&op_jeq,
		&&op_jneq, &&op_jless, &&op_jleq, &&op_jgrtr,
		&&op_jgeq, &&op_jsless, &&op_jsleq, &&op_jsgrtr,
		&&op_jsgeq, &&op_shl, &&op_shr, &&op_sshr,
		&&op_shl_c_pri, &&op_shl_c_alt, &&op_shr_c_pri, &&op_shr_c_alt,
		&&op_smul, &&op_sdiv, &&op_sdiv_alt, &&op_umul,
		&&op_udiv, &&op_udiv_alt, &&op_add, &&op_sub,
		&&op_sub_alt, &&op_and, &&op_or, &&op_xor,
		&&op_not, &&op_neg, &&op_invert, &&op_add_c,
		&&op_smul_c, &&op_zero_pri, &&op_zero_alt, &&op_zero,
		&&op_zero_s, &&op_sign_pri, &&op_sign_alt, &&op_eq,
		&&op_neq, &&op_less, &&op_leq, &&op_grtr,
		&&op_geq, &&op_sless, &&op_sleq, &&op_sgrtr,
		&&op_sgeq, &&op_eq_c_pri, &&op_eq_c_alt, &&op_inc_pri,
		&&op_inc_alt, &&op_inc, &&op_inc_s, &&op_inc_i,
		&&op_dec_pri, &&op_dec_alt, &&op_dec, &&op_dec_s,
		&&op_dec_i, &&op_movs, &&op_cmps, &&op_fill,
		&&op_halt, &&op_bounds, &&op_sysreq_pri, &&op_sysreq_c,
		&&op_file, &&op_line, &&op_symbol, &&op_srange,
		&&op_jump_pri, &&op_switch, &&op_casetbl};

		AMX_HEADER *hdr=NULL;
		AMX_FUNCSTUB *func=NULL;
		uchar *code=NULL;
		uchar *data=NULL;
		cell pri=0, alt=0, stk=0, frm=0, hea=0, *cip=NULL;
		cell offs=0;
		int num, i;
		va_list ap;
#if !defined NODBGCALLS
		int debug=0;
#endif							/*  */
		/* HACK: return label table (for amx_BrowseRelocate) if amx structure
		 * is not passed.
		 */
		if (amx == NULL) {
			assert(sizeof(cell) == sizeof(void *));
			assert(retval != NULL);
			*retval = (cell) labels;
			return 0;
		}							/* if */
		if (amx->callback == NULL)
			return AMX_ERR_CALLBACK;

		amx->flags &= ~AMX_FLAG_BROWSE;
		if ((amx->flags & AMX_FLAG_RELOC) == 0)
			return AMX_ERR_INIT;
#if !defined NODBGCALLS
		debug = (amx->flags & AMX_FLAG_DEBUG) != 0;
#endif							/*  */
		/* set up the registers */
		hdr = (AMX_HEADER *) amx->base;
		code = amx->base + (int) hdr->cod;
		data = amx->base + (int) hdr->dat;
		hea = amx->hea;
		stk = amx->stk;

		/* get the start address */
		if (index == AMX_EXEC_MAIN) {
			cip = (cell *) (code + (int) hdr->cip);
		} else if (index < 0 && index != AMX_EXEC_CONT) {
			return AMX_ERR_INDEX;
		} else {
			if (index >= hdr->num_publics)
				return AMX_ERR_INDEX;
			func = (AMX_FUNCSTUB *) (amx->base + (int) hdr->publics + index * sizeof(AMX_FUNCSTUB));
			cip = (cell *) (code + (int) func->address);
		}							/* if */
		/* sanity checks */
		assert(OP_PUSH_PRI == 36);
		assert(OP_PROC == 46);
		assert(OP_SHL == 65);
		assert(OP_SMUL == 72);
		assert(OP_EQ == 95);
		assert(OP_INC_PRI == 107);
		assert(OP_MOVS == 117);
		assert(OP_SYMBOL == 126);
		assert(sizeof(cell) == 4);

		/* push the parameters to the stack (in reverse order) */
		stk -= numparams * sizeof(cell);
		va_start(ap, numparams);
		for (i = 0; i < numparams; i++)
			*(cell *) (data + (int) stk + i * sizeof(cell)) = va_arg(ap, cell);
		va_end(ap);
		PUSH(numparams * sizeof(cell));
		PUSH(0);					/* zero return address */

		/* start running */
		frm = 0;					/* just to avoid compiler warnings */
		NEXT(cip);

op_none:
		return AMX_ERR_INVINSTR;

op_load_pri:
		GETPARAM(offs);
		pri = *(cell *) (data + (int) offs);
		NEXT(cip);

op_load_alt:
		GETPARAM(offs);
		alt = *(cell *) (data + (int) offs);
		NEXT(cip);

op_load_s_pri:
		GETPARAM(offs);
		pri = *(cell *) (data + (int) frm + (int) offs);
		NEXT(cip);

op_load_s_alt:
		GETPARAM(offs);
		alt = *(cell *) (data + (int) frm + (int) offs);
		NEXT(cip);

op_lref_pri:
		GETPARAM(offs);
		offs = *(cell *) (data + (int) offs);
		pri = *(cell *) (data + (int) offs);
		NEXT(cip);

op_lref_alt:
		GETPARAM(offs);
		offs = *(cell *) (data + (int) offs);
		alt = *(cell *) (data + (int) offs);
		NEXT(cip);

op_lref_s_pri:
		GETPARAM(offs);
		offs = *(cell *) (data + (int) frm + (int) offs);
		pri = *(cell *) (data + (int) offs);
		NEXT(cip);

op_lref_s_alt:
		GETPARAM(offs);
		offs = *(cell *) (data + (int) frm + (int) offs);
		alt = *(cell *) (data + (int) offs);
		NEXT(cip);

op_load_i:
		/* verify address */
		if ((pri >= hea && pri < stk) || pri >= amx->stp)
			return AMX_ERR_MEMACCESS;
		pri = *(cell *) (data + (int) pri);
		NEXT(cip);

op_lodb_i:
		GETPARAM(offs);
		/* verify address */
		if ((pri >= hea && pri < stk) || pri >= amx->stp)
			return AMX_ERR_MEMACCESS;
		switch (offs) {
			case 1:
				pri = *(data + (int) pri);
				break;
			case 2:
				pri = *(ushort *) (data + (int) pri);
				break;
			case 4:
				pri = *(cell *) (data + (int) pri);
				break;
		}							/* switch */
		NEXT(cip);

op_const_pri:
		GETPARAM(pri);
		NEXT(cip);

op_const_alt:
		GETPARAM(alt);
		NEXT(cip);

op_addr_pri:
		GETPARAM(pri);
		pri += frm;
		NEXT(cip);

op_addr_alt:
		GETPARAM(alt);
		alt += frm;
		NEXT(cip);

op_stor_pri:
		GETPARAM(offs);
		*(cell *) (data + (int) offs) = pri;
		NEXT(cip);

op_stor_alt:
		GETPARAM(offs);
		*(cell *) (data + (int) offs) = alt;
		NEXT(cip);

op_stor_s_pri:
		GETPARAM(offs);
		*(cell *) (data + (int) frm + (int) offs) = pri;
		NEXT(cip);

op_stor_s_alt:
		GETPARAM(offs);
		*(cell *) (data + (int) frm + (int) offs) = alt;
		NEXT(cip);

op_sref_pri:
		GETPARAM(offs);
		offs = *(cell *) (data + (int) offs);
		*(cell *) (data + (int) offs) = pri;
		NEXT(cip);

op_sref_alt:
		GETPARAM(offs);
		offs = *(cell *) (data + (int) offs);
		*(cell *) (data + (int) offs) = alt;
		NEXT(cip);

op_sref_s_pri:
		GETPARAM(offs);
		offs = *(cell *) (data + (int) frm + (int) offs);
		*(cell *) (data + (int) offs) = pri;
		NEXT(cip);

op_sref_s_alt:
		GETPARAM(offs);
		offs = *(cell *) (data + (int) frm + (int) offs);
		*(cell *) (data + (int) offs) = alt;
		NEXT(cip);

op_stor_i:
		/* verify address */
		if ((alt >= hea && alt < stk) || alt >= amx->stp)
			return AMX_ERR_MEMACCESS;
		*(cell *) (data + (int) alt) = pri;
		NEXT(cip);

op_strb_i:
		GETPARAM(offs);
		/* verify address */
		if ((alt >= hea && alt < stk) || alt >= amx->stp)
			return AMX_ERR_MEMACCESS;
		switch (offs) {
			case 1:
				*(data + (int) alt) = (uchar) pri;
				break;
			case 2:
				*(ushort *) (data + (int) alt) = (ushort) pri;
				break;
			case 4:
				*(cell *) (data + (int) alt) = pri;
				break;
		}							/* switch */
		NEXT(cip);

op_lidx:
		offs = pri * sizeof(cell) + alt;
		/* verify address */
		if ((offs >= hea && offs < stk) || offs >= amx->stp)
			return AMX_ERR_MEMACCESS;
		pri = *(cell *) (data + (int) offs);
		NEXT(cip);

op_lidx_b:
		GETPARAM(offs);
		offs = (pri << (int) offs) + alt;
		/* verify address */
		if ((offs >= hea && offs < stk) || offs >= amx->stp)
			return AMX_ERR_MEMACCESS;
		pri = *(cell *) (data + (int) offs);
		NEXT(cip);

op_idxaddr:
		pri = pri * sizeof(cell) + alt;
		NEXT(cip);

op_idxaddr_b:
		GETPARAM(offs);
		pri = (pri << (int) offs) + alt;
		NEXT(cip);

op_align_pri:
		GETPARAM(offs);
		if (amx_LittleEndian && offs < sizeof(cell))
			pri ^= sizeof(cell) - offs;
		NEXT(cip);

op_align_alt:
		GETPARAM(offs);
		if (amx_LittleEndian && offs < sizeof(cell))
			alt ^= sizeof(cell) - offs;
		NEXT(cip);

op_lctrl:
		GETPARAM(offs);
		switch (offs) {
			case 0:
				pri = hdr->cod;
				break;
			case 1:
				pri = hdr->dat;
				break;
			case 2:
				pri = hea;
				break;
			case 3:
				pri = amx->stp;
				break;
			case 4:
				pri = stk;
				break;
			case 5:
				pri = frm;
				break;
			case 6:
				pri = (cell) ((uchar *) cip - code);
				break;
		}							/* switch */
		NEXT(cip);

op_sctrl:
		GETPARAM(offs);
		switch (offs) {
			case 0:
			case 1:
			case 3:
				/* cannot change these parameters */
				break;
			case 2:
				hea = pri;
				break;
			case 4:
				stk = pri;
				break;
			case 5:
				frm = pri;
				break;
			case 6:
				cip = (cell *) (code + (int) pri);
				break;
		}							/* switch */
		NEXT(cip);

op_move_pri:
		pri = alt;
		NEXT(cip);

op_move_alt:
		alt = pri;
		NEXT(cip);

op_xchg:
		offs = pri;					/* offs is a temporary variable */
		pri = alt;
		alt = offs;
		NEXT(cip);

op_push_pri:
		PUSH(pri);
		NEXT(cip);

op_push_alt:
		PUSH(alt);
		NEXT(cip);

op_push_c:
		GETPARAM(offs);
		PUSH(offs);
		NEXT(cip);

op_push_r:
		GETPARAM(offs);
		while (offs--)
			PUSH(pri);
		NEXT(cip);

op_push:
		GETPARAM(offs);
		PUSH(*(cell *) (data + (int) offs));
		NEXT(cip);

op_push_s:
		GETPARAM(offs);
		PUSH(*(cell *) (data + (int) frm + (int) offs));
		NEXT(cip);

op_pop_pri:
		POP(pri);
		NEXT(cip);

op_pop_alt:
		POP(alt);
		NEXT(cip);

op_stack:
		GETPARAM(offs);
		alt = stk;
		stk += offs;
		CHKMARGIN();
		CHKSTACK();
#if !defined NODBGCALLS
		if (debug && offs > 0) {
			amx->dbgcode = DBG_CLRSYM;
			amx->stk = stk;
			amx->debug(amx);
		}							/* if */
#endif							/*  */
		NEXT(cip);

op_heap:
		GETPARAM(offs);
		alt = hea;
		hea += offs;
		CHKMARGIN();
		CHKHEAP();
		NEXT(cip);

op_proc:
		PUSH(frm);
		frm = stk;
		NEXT(cip);

op_ret:
		POP(frm);
		POP(offs);
		cip = (cell *) (code + (int) offs);
		if (offs == 0) {
			/* entry function returns -> end of program */
			if (retval != NULL)
				*retval = pri;
#if !defined NODBGCALLS
			if (debug) {
				amx->dbgcode = DBG_TERMINATE;
				amx->debug(amx);
			}						/* if */
#endif							/*  */
			amx->stk = stk;
			return AMX_ERR_NONE;
		} else {
#if !defined NODBGCALLS
			if (debug) {
				amx->dbgcode = DBG_RETURN;
				amx->debug(amx);
			}						/* if */
#endif							/*  */
		}							/* if */
		NEXT(cip);

op_retn:
		POP(frm);
		POP(offs);
		cip = (cell *) (code + (int) offs);
		stk += *(cell *) (data + (int) stk) + 4;
		amx->stk = stk;
		if (offs == 0) {
			/* entry function returns -> end of program */
			if (retval != NULL)
				*retval = pri;
#if !defined NODBGCALLS
			if (debug) {
				amx->dbgcode = DBG_TERMINATE;
				amx->debug(amx);
			}						/* if */
#endif							/*  */
			return AMX_ERR_NONE;
		} else {
#if !defined NODBGCALLS
			if (debug) {
				amx->dbgcode = DBG_RETURN;
				amx->debug(amx);
			}						/* if */
#endif							/*  */
		}							/* if */
#if !defined NODBGCALLS
		if (debug) {
			amx->dbgcode = DBG_CLRSYM;
			amx->debug(amx);
		}							/* if */
#endif							/*  */
		NEXT(cip);

op_call:
		PUSH(((uchar *) cip - code) + 4);	/* skip address */
		cip = (cell *) * cip;		/* jump to the address */
#if !defined NODBGCALLS
		if (debug) {
			amx->dbgcode = DBG_CALL;
			amx->debug(amx);
		}							/* if */
#endif							/*  */
		NEXT(cip);

op_call_pri:
		PUSH((uchar *) cip - code);
		cip = (cell *) (code + (int) pri);
#if !defined NODBGCALLS
		if (debug) {
			amx->dbgcode = DBG_CALL;
			amx->debug(amx);
		}							/* if */
#endif							/*  */
		NEXT(cip);

op_jump:
		/* since the GETPARAM() macro modifies cip, you cannot
		 * do GETPARAM(cip) directly */
		cip = (cell *) * cip;
		NEXT(cip);

op_jrel:
		offs = *cip;
		cip = (cell *) ((uchar *) cip + (int) offs + sizeof(cell));
		NEXT(cip);

op_jzer:
		if (pri == 0)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jnz:
		if (pri != 0)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jeq:
		if (pri == alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jneq:
		if (pri != alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jless:
		if ((ucell) pri < (ucell) alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jleq:
		if ((ucell) pri <= (ucell) alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jgrtr:
		if ((ucell) pri > (ucell) alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jgeq:
		if ((ucell) pri >= (ucell) alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jsless:
		if (pri < alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jsleq:
		if (pri <= alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jsgrtr:
		if (pri > alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_jsgeq:
		if (pri >= alt)
			cip = (cell *) * cip;
		else
			cip = (cell *) ((uchar *) cip + sizeof(cell));
		NEXT(cip);

op_shl:
		pri <<= alt;
		NEXT(cip);

op_shr:
		pri = (ucell) pri >> (ucell) alt;
		NEXT(cip);

op_sshr:
		pri >>= alt;
		NEXT(cip);

op_shl_c_pri:
		GETPARAM(offs);
		pri <<= offs;
		NEXT(cip);

op_shl_c_alt:
		GETPARAM(offs);
		alt <<= offs;
		NEXT(cip);

op_shr_c_pri:
		GETPARAM(offs);
		pri = (ucell) pri >> (ucell) offs;
		NEXT(cip);

op_shr_c_alt:
		GETPARAM(offs);
		alt = (ucell) alt >> (ucell) offs;
		NEXT(cip);

op_smul:
		pri *= alt;
		NEXT(cip);

op_sdiv:
		if (alt == 0)
			return AMX_ERR_DIVIDE;
		/* divide must always round down; this is a bit
		 * involved to do in a machine-independent way.
		 */
		offs = (pri % alt + alt) % alt;		/* true modulus */
		pri = (pri - offs) / alt;	/* division result */
		alt = offs;
		NEXT(cip);

op_sdiv_alt:
		if (pri == 0)
			return AMX_ERR_DIVIDE;
		/* divide must always round down; this is a bit
		 * involved to do in a machine-independent way.
		 */
		offs = (alt % pri + pri) % pri;		/* true modulus */
		pri = (alt - offs) / pri;	/* division result */
		alt = offs;
		NEXT(cip);

op_umul:
		pri = (ucell) pri *(ucell) alt;
		NEXT(cip);

op_udiv:
		if (alt == 0)
			return AMX_ERR_DIVIDE;
		offs = (ucell) pri % (ucell) alt;	/* temporary storage */
		pri = (ucell) pri / (ucell) alt;
		alt = offs;
		NEXT(cip);

op_udiv_alt:
		if (pri == 0)
			return AMX_ERR_DIVIDE;
		offs = (ucell) alt % (ucell) pri;	/* temporary storage */
		pri = (ucell) alt / (ucell) pri;
		alt = offs;
		NEXT(cip);

op_add:
		pri += alt;
		NEXT(cip);

op_sub:
		pri -= alt;
		NEXT(cip);

op_sub_alt:
		pri = alt - pri;
		NEXT(cip);

op_and:
		pri &= alt;
		NEXT(cip);

op_or:
		pri |= alt;
		NEXT(cip);

op_xor:
		pri ^= alt;
		NEXT(cip);

op_not:
		pri = !pri;
		NEXT(cip);

op_neg:
		pri = -pri;
		NEXT(cip);

op_invert:
		pri = ~pri;
		NEXT(cip);

op_add_c:
		GETPARAM(offs);
		pri += offs;
		NEXT(cip);

op_smul_c:
		GETPARAM(offs);
		pri *= offs;
		NEXT(cip);

op_zero_pri:
		pri = 0;
		NEXT(cip);

op_zero_alt:
		alt = 0;
		NEXT(cip);

op_zero:
		GETPARAM(offs);
		*(cell *) (data + (int) offs) = 0;
		NEXT(cip);

op_zero_s:
		GETPARAM(offs);
		*(cell *) (data + (int) frm + (int) offs) = 0;
		NEXT(cip);

op_sign_pri:
		if ((pri & 0xff) >= 0x80)
			pri |= ~(ucell) 0xff;
		NEXT(cip);

op_sign_alt:
		if ((alt & 0xff) >= 0x80)
			alt |= ~(ucell) 0xff;
		NEXT(cip);

op_eq:
		pri = pri == alt ? 1 : 0;
		NEXT(cip);

op_neq:
		pri = pri != alt ? 1 : 0;
		NEXT(cip);

op_less:
		pri = (ucell) pri < (ucell) alt ? 1 : 0;
		NEXT(cip);

op_leq:
		pri = (ucell) pri <= (ucell) alt ? 1 : 0;
		NEXT(cip);

op_grtr:
		pri = (ucell) pri > (ucell) alt ? 1 : 0;
		NEXT(cip);

op_geq:
		pri = (ucell) pri >= (ucell) alt ? 1 : 0;
		NEXT(cip);

op_sless:
		pri = pri < alt ? 1 : 0;
		NEXT(cip);

op_sleq:
		pri = pri <= alt ? 1 : 0;
		NEXT(cip);

op_sgrtr:
		pri = pri > alt ? 1 : 0;
		NEXT(cip);

op_sgeq:
		pri = pri >= alt ? 1 : 0;
		NEXT(cip);

op_eq_c_pri:
		GETPARAM(offs);
		pri = pri == offs ? 1 : 0;
		NEXT(cip);

op_eq_c_alt:
		GETPARAM(offs);
		pri = alt == offs ? 1 : 0;
		NEXT(cip);

op_inc_pri:
		pri++;
		NEXT(cip);

op_inc_alt:
		alt++;
		NEXT(cip);

op_inc:
		GETPARAM(offs);
		*(cell *) (data + (int) offs) += 1;
		NEXT(cip);

op_inc_s:
		GETPARAM(offs);
		*(cell *) (data + (int) frm + (int) offs) += 1;
		NEXT(cip);

op_inc_i:
		*(cell *) (data + (int) pri) += 1;
		NEXT(cip);

op_dec_pri:
		pri--;
		NEXT(cip);

op_dec_alt:
		alt--;
		NEXT(cip);

op_dec:
		GETPARAM(offs);
		*(cell *) (data + (int) offs) -= 1;
		NEXT(cip);

op_dec_s:
		GETPARAM(offs);
		*(cell *) (data + (int) frm + (int) offs) -= 1;
		NEXT(cip);

op_dec_i:
		*(cell *) (data + (int) pri) -= 1;
		NEXT(cip);

op_movs:
		GETPARAM(offs);
		memcpy(data + (int) alt, data + (int) pri, (int) offs);
		NEXT(cip);

op_cmps:
		GETPARAM(offs);
		pri = memcmp(data + (int) alt, data + (int) pri, (int) offs);
		NEXT(cip);

op_fill:
		GETPARAM(offs);
		for (i = (int) alt; offs >= sizeof(cell); i += sizeof(cell), offs -= sizeof(cell))
			*(cell *) (data + i) = pri;
		NEXT(cip);

op_halt:
		GETPARAM(offs);
		if (retval != NULL)
			*retval = pri;
		return (int) offs;

op_bounds:
		GETPARAM(offs);
		if (pri > offs || pri < 0)
			return AMX_ERR_BOUNDS;
		NEXT(cip);

op_sysreq_pri:
		/* save a few registers */
		amx->cip = (cell) ((uchar *) cip - code);
		amx->hea = hea;
		amx->frm = frm;
		amx->stk = stk;
		num = amx->callback(amx, pri, &pri, (cell *) (data + (int) stk));
		if (num != AMX_ERR_NONE)
			return num;
		NEXT(cip);

op_sysreq_c:
		GETPARAM(offs);
		/* save a few registers */
		amx->cip = (cell) ((uchar *) cip - code);
		amx->hea = hea;
		amx->frm = frm;
		amx->stk = stk;
		num = amx->callback(amx, offs, &pri, (cell *) (data + (int) stk));
		if (num != AMX_ERR_NONE)
			return num;
		NEXT(cip);

op_file:
		GETPARAM(offs);
		cip = (cell *) ((uchar *) cip + (int) offs);
		assert(0);		/* this code should not occur during execution */
		NEXT(cip);

op_line:
		assert((amx->flags & AMX_FLAG_BROWSE) == 0);
		GETPARAM(amx->curline);
		GETPARAM(amx->curfile);
#if !defined NODBGCALLS
		if (debug) {
			amx->frm = frm;
			amx->stk = stk;
			amx->dbgcode = DBG_LINE;
			amx->debug(amx);
		}							/* if */
#endif							/*  */
		NEXT(cip);

op_symbol:
		assert((amx->flags & AMX_FLAG_BROWSE) == 0);
		GETPARAM(offs);
		GETPARAM(amx->dbgaddr);
		GETPARAM(amx->dbgparam);
		amx->dbgname = (char *) cip;
		cip = (cell *) ((uchar *) cip + (int) offs - 2 * sizeof(cell));
#if !defined NODBGCALLS
		amx->dbgcode = DBG_SYMBOL;
		assert((amx->dbgparam >> 8) > 0);	/* local symbols only */
		if (debug) {
			amx->frm = frm;	/* debugger needs this to relocate the symbols */
			amx->debug(amx);
		}							/* if */
#endif							/*  */
		NEXT(cip);

op_srange:
		assert((amx->flags & AMX_FLAG_BROWSE) == 0);
		GETPARAM(amx->dbgaddr);		/* dimension level */
		GETPARAM(amx->dbgparam);	/* length */
#if !defined NODBGCALLS
		amx->dbgcode = DBG_SRANGE;
		if (debug) {
			amx->frm = frm;	/* debugger needs this to relocate the symbols */
			amx->debug(amx);
		}							/* if */
#endif							/*  */
		NEXT(cip);

op_jump_pri:
		cip = (cell *) (code + (int) pri);
		NEXT(cip);

op_switch:
		{
			cell *cptr;

			cptr = (cell *) * cip + 1;	/* +1, to skip the "casetbl" opcode */
			cip = (cell *) * (cptr + 1);	/* preset to "none-matched" case */
			num = (int) *cptr;		/* number of records in the case table */
			for (cptr += 2; num > 0 && *cptr != pri; num--, cptr += 2)
				/* nothing */ ;
			if (num > 0)
				cip = (cell *) * (cptr + 1);	/* case found */
		}
		NEXT(cip);

op_casetbl:
		assert(0);					/* this should not occur during execution */
		NEXT(cip);
}

#else							/*  */
/* ANSI C & assembler versions */

#if defined ASM32 || defined JIT
/* For Watcom C/C++ use register calling convention (faster); for
 * Microsoft C/C++ (and possibly others) use __cdecl.
 * The important point is that you assemble AMXEXEC.ASM with the matching
 * calling convention.
 */
#if defined __WATCOMC__
extern cell amx_exec_asm(cell * regs, cell * retval, cell stp, cell hea);

/* The following pragma tells the compiler into which registers
 * the parameters have to go. */
#pragma aux amx_exec_asm parm [eax] [edx] [ebx] [ecx];
#else							/*  */
extern cell __cdecl amx_exec_asm(cell * regs, cell * retval, cell stp, cell hea);

#endif							/*  */
#endif							/*  */

int amx_Exec(AMX * amx, cell * retval, int index, int numparams,...) {
	AMX_HEADER *hdr;
	AMX_FUNCSTUB *func;
	uchar *code, *data;
	cell pri, alt, stk, frm, hea, *cip;
	int i;
	va_list ap;
#if !defined NODBGCALLS
	int debug;
#endif							/*  */
#if defined ASM32 || defined JIT
	extern void *amx_opcodelist[];
	cell parms[9];			/* MP: registers for assembler AMX */
#else							/*  */
	OPCODE op;
	cell offs;
	int num;
#endif							/*  */

#if defined ASM32 || defined JIT
	/* HACK: return label table (for amx_BrowseRelocate) if amx structure
	 * is not passed.
	 */
	if (amx == NULL) {
		assert(sizeof(cell) == sizeof(void *));
		assert(retval != NULL);
		*retval = (cell) amx_opcodelist;
		return 0;
	}						/* if */
#endif							/*  */
   	if (amx->callback == NULL)
	   	return AMX_ERR_CALLBACK;

   	amx->flags &= ~AMX_FLAG_BROWSE;

	if ((amx->flags & AMX_FLAG_RELOC) == 0)
		return AMX_ERR_INIT;

#if !defined NODBGCALLS
	debug = (amx->flags & AMX_FLAG_DEBUG) != 0;
#endif							/*  */

	/* set up the registers */
	hdr = (AMX_HEADER *) amx->base;
	code = amx->base + (int) hdr->cod;
	data = amx->base + (int) hdr->dat;
	hea = amx->hea;
	stk = amx->stk;

	/* get the start address */
	if (index == AMX_EXEC_MAIN) {
		cip = (cell *) (code + (int) hdr->cip);
	} else if (index < 0 && index != AMX_EXEC_CONT) {
		return AMX_ERR_INDEX;
	} else {
		if (index >= hdr->num_publics)
			return AMX_ERR_INDEX;
		func = (AMX_FUNCSTUB *) (amx->base + (int) hdr->publics + index * sizeof(AMX_FUNCSTUB));
		cip = (cell *) (code + (int) func->address);
	}						/* if */

	/* sanity checks */
	assert(OP_PUSH_PRI == 36);
	assert(OP_PROC == 46);
	assert(OP_SHL == 65);
	assert(OP_SMUL == 72);
	assert(OP_EQ == 95);
	assert(OP_INC_PRI == 107);
	assert(OP_MOVS == 117);
	assert(OP_SYMBOL == 126);
	assert(sizeof(cell) == 4);

	/* push the parameters to the stack (in reverse order) */
	stk -= numparams * sizeof(cell);
	va_start(ap, numparams);
	for (i = 0; i < numparams; i++)
		*(cell *) (data + (int) stk + i * sizeof(cell)) = va_arg(ap, cell);
	va_end(ap);
	PUSH(numparams * sizeof(cell));
	PUSH(0);				/* zero return address */

	frm = alt = pri = 0;	/* silence up compiler */

	/* start running */
#if defined ASM32 || defined JIT
	/* either the assembler abstract machine or the JIT; both by Marc Peter */
	parms[0] = pri;
	parms[1] = alt;
	parms[2] = (cell) cip;
	parms[3] = (cell) data;
	parms[4] = stk;
	parms[5] = frm;
	parms[6] = (cell) amx;
	parms[7] = (cell) code;
	parms[8] = (cell) debug;

	return amx_exec_asm(parms, retval, hdr->stp, hea);

#else							/*  */
	for (;;) {
		op = (OPCODE) * cip++;
		switch (op) {
			case OP_LOAD_PRI:
				GETPARAM(offs);
				pri = *(cell *) (data + (int) offs);
				break;
			case OP_LOAD_ALT:
				GETPARAM(offs);
				alt = *(cell *) (data + (int) offs);
				break;
			case OP_LOAD_S_PRI:
				GETPARAM(offs);
				pri = *(cell *) (data + (int) frm + (int) offs);
				break;
			case OP_LOAD_S_ALT:
				GETPARAM(offs);
				alt = *(cell *) (data + (int) frm + (int) offs);
				break;
			case OP_LREF_PRI:
				GETPARAM(offs);
				offs = *(cell *) (data + (int) offs);
				pri = *(cell *) (data + (int) offs);
				break;
			case OP_LREF_ALT:
				GETPARAM(offs);
				offs = *(cell *) (data + (int) offs);
				alt = *(cell *) (data + (int) offs);
				break;
			case OP_LREF_S_PRI:
				GETPARAM(offs);
				offs = *(cell *) (data + (int) frm + (int) offs);
				pri = *(cell *) (data + (int) offs);
				break;
			case OP_LREF_S_ALT:
				GETPARAM(offs);
				offs = *(cell *) (data + (int) frm + (int) offs);
				alt = *(cell *) (data + (int) offs);
				break;
			case OP_LOAD_I:
				/* verify address */
				if (pri >= hea && pri < stk || pri >= amx->stp)
					return AMX_ERR_MEMACCESS;
				pri = *(cell *) (data + (int) pri);
				break;
			case OP_LODB_I:
				GETPARAM(offs);
				/* verify address */
				if (pri >= hea && pri < stk || pri >= amx->stp)
					return AMX_ERR_MEMACCESS;
				switch (offs) {
					case 1:
						pri = *(data + (int) pri);
						break;
					case 2:
						pri = *(ushort *) (data + (int) pri);
						break;
					case 4:
						pri = *(cell *) (data + (int) pri);
						break;
				}			/* switch */
				break;
			case OP_CONST_PRI:
				GETPARAM(pri);
				break;
			case OP_CONST_ALT:
				GETPARAM(alt);
				break;
			case OP_ADDR_PRI:
				GETPARAM(pri);
				pri += frm;
				break;
			case OP_ADDR_ALT:
				GETPARAM(alt);
				alt += frm;
				break;
			case OP_STOR_PRI:
				GETPARAM(offs);
				*(cell *) (data + (int) offs) = pri;
				break;
			case OP_STOR_ALT:
				GETPARAM(offs);
				*(cell *) (data + (int) offs) = alt;
				break;
			case OP_STOR_S_PRI:
				GETPARAM(offs);
				*(cell *) (data + (int) frm + (int) offs) = pri;
				break;
			case OP_STOR_S_ALT:
				GETPARAM(offs);
				*(cell *) (data + (int) frm + (int) offs) = alt;
				break;
			case OP_SREF_PRI:
				GETPARAM(offs);
				offs = *(cell *) (data + (int) offs);
				*(cell *) (data + (int) offs) = pri;
				break;
			case OP_SREF_ALT:
				GETPARAM(offs);
				offs = *(cell *) (data + (int) offs);
				*(cell *) (data + (int) offs) = alt;
				break;
			case OP_SREF_S_PRI:
				GETPARAM(offs);
				offs = *(cell *) (data + (int) frm + (int) offs);
				*(cell *) (data + (int) offs) = pri;
				break;
			case OP_SREF_S_ALT:
				GETPARAM(offs);
				offs = *(cell *) (data + (int) frm + (int) offs);
				*(cell *) (data + (int) offs) = alt;
				break;
			case OP_STOR_I:
				/* verify address */
				if (alt >= hea && alt < stk || alt >= amx->stp)
					return AMX_ERR_MEMACCESS;
				*(cell *) (data + (int) alt) = pri;
				break;
			case OP_STRB_I:
				GETPARAM(offs);
				/* verify address */
				if (alt >= hea && alt < stk || alt >= amx->stp)
					return AMX_ERR_MEMACCESS;
				switch (offs) {
					case 1:
						*(data + (int) alt) = (uchar) pri;
						break;
					case 2:
						*(ushort *) (data + (int) alt) = (ushort) pri;
						break;
					case 4:
						*(cell *) (data + (int) alt) = pri;
						break;
				}			/* switch */
				break;
			case OP_LIDX:
				offs = pri * sizeof(cell) + alt;
				/* verify address */
				if (offs >= hea && offs < stk || offs >= amx->stp)
					return AMX_ERR_MEMACCESS;
				pri = *(cell *) (data + (int) offs);
				break;
			case OP_LIDX_B:
				GETPARAM(offs);
				offs = (pri << (int) offs) + alt;
				/* verify address */
				if (offs >= hea && offs < stk || offs >= amx->stp)
					return AMX_ERR_MEMACCESS;
				pri = *(cell *) (data + (int) offs);
				break;
			case OP_IDXADDR:
				pri = pri * sizeof(cell) + alt;
				break;
			case OP_IDXADDR_B:
				GETPARAM(offs);
				pri = (pri << (int) offs) + alt;
				break;
			case OP_ALIGN_PRI:
				GETPARAM(offs);
				if (amx_LittleEndian && (size_t) offs < sizeof(cell))
					pri ^= sizeof(cell) - offs;
				break;
			case OP_ALIGN_ALT:
				GETPARAM(offs);
				if (amx_LittleEndian && (size_t) offs < sizeof(cell))
					alt ^= sizeof(cell) - offs;
				break;
			case OP_LCTRL:
				GETPARAM(offs);
				switch (offs) {
					case 0:
						pri = hdr->cod;
						break;
					case 1:
						pri = hdr->dat;
						break;
					case 2:
						pri = hea;
						break;
					case 3:
						pri = amx->stp;
						break;
					case 4:
						pri = stk;
						break;
					case 5:
						pri = frm;
						break;
					case 6:
						pri = (cell) ((uchar *) cip - code);
						break;
				}			/* switch */
				break;
			case OP_SCTRL:
				GETPARAM(offs);
				switch (offs) {
					case 0:
					case 1:
					case 3:
						/* cannot change these parameters */
						break;
					case 2:
						hea = pri;
						break;
					case 4:
						stk = pri;
						break;
					case 5:
						frm = pri;
						break;
					case 6:
						cip = (cell *) (code + (int) pri);
						break;
				}			/* switch */
				break;
			case OP_MOVE_PRI:
				pri = alt;
				break;
			case OP_MOVE_ALT:
				alt = pri;
				break;
			case OP_XCHG:
				offs = pri;	/* offs is a temporary variable */
				pri = alt;
				alt = offs;
				break;
			case OP_PUSH_PRI:
				PUSH(pri);
				break;
			case OP_PUSH_ALT:
				PUSH(alt);
				break;
			case OP_PUSH_C:
				GETPARAM(offs);
				PUSH(offs);
				break;
			case OP_PUSH_R:
				GETPARAM(offs);
				while (offs--)
					PUSH(pri);
				break;
			case OP_PUSH:
				GETPARAM(offs);
				PUSH(*(cell *) (data + (int) offs));
				break;
			case OP_PUSH_S:
				GETPARAM(offs);
				PUSH(*(cell *) (data + (int) frm + (int) offs));
				break;
			case OP_POP_PRI:
				POP(pri);
				break;
			case OP_POP_ALT:
				POP(alt);
				break;
			case OP_STACK:
				GETPARAM(offs);
				alt = stk;
				stk += offs;
				CHKMARGIN();
				CHKSTACK();
#if !defined NODBGCALLS
				if (debug && offs > 0) {
					amx->dbgcode = DBG_CLRSYM;
					amx->stk = stk;
					amx->debug(amx);
				}			/* if */
#endif							/*  */
				break;
			case OP_HEAP:
				GETPARAM(offs);
				alt = hea;
				hea += offs;
				CHKMARGIN();
				CHKHEAP();
				break;
			case OP_PROC:
				PUSH(frm);
				frm = stk;
				break;
			case OP_RET:
				POP(frm);
				POP(offs);
				cip = (cell *) (code + (int) offs);
				if (offs == 0) {
					/* entry function returns -> end of program */
					if (retval != NULL)
						*retval = pri;
#if !defined NODBGCALLS
					if (debug) {
						amx->dbgcode = DBG_TERMINATE;
						amx->debug(amx);
					}		/* if */
#endif							/*  */
					amx->stk = stk;
					return AMX_ERR_NONE;
				} else {
#if !defined NODBGCALLS
					if (debug) {
						amx->dbgcode = DBG_RETURN;
						amx->debug(amx);
					}		/* if */
#endif							/*  */
				}			/* if */
				break;
			case OP_RETN:
				POP(frm);
				POP(offs);
				cip = (cell *) (code + (int) offs);
				stk += *(cell *) (data + (int) stk) + 4;
				amx->stk = stk;
				if (offs == 0) {
					/* entry function returns -> end of program */
					if (retval != NULL)
						*retval = pri;
#if !defined NODBGCALLS
					if (debug) {
						amx->dbgcode = DBG_TERMINATE;
						amx->debug(amx);
					}		/* if */
#endif							/*  */
					return AMX_ERR_NONE;
				} else {
#if !defined NODBGCALLS
					if (debug) {
						amx->dbgcode = DBG_RETURN;
						amx->debug(amx);
					}		/* if */
#endif							/*  */
				}			/* if */
#if !defined NODBGCALLS
				if (debug) {
					amx->dbgcode = DBG_CLRSYM;
					amx->debug(amx);
				}			/* if */
#endif							/*  */
				break;
			case OP_CALL:
				PUSH(((uchar *) cip - code) + 4);	/* skip address */
				cip = (cell *) * cip;	/* jump to the address */
#if !defined NODBGCALLS
				if (debug) {
					amx->dbgcode = DBG_CALL;
					amx->debug(amx);
				}			/* if */
#endif							/*  */
				break;
			case OP_CALL_PRI:
				PUSH((uchar *) cip - code);
				cip = (cell *) (code + (int) pri);
#if !defined NODBGCALLS
				if (debug) {
					amx->dbgcode = DBG_CALL;
					amx->debug(amx);
				}			/* if */
#endif							/*  */
				break;
			case OP_JUMP:
				/* since the GETPARAM() macro modifies cip, you cannot
				 * do GETPARAM(cip) directly */
				cip = (cell *) * cip;
				break;
			case OP_JREL:
				offs = *cip;
				cip = (cell *) ((uchar *) cip + (int) offs + sizeof(cell));
				break;
			case OP_JZER:
				if (pri == 0)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JNZ:
				if (pri != 0)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JEQ:
				if (pri == alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JNEQ:
				if (pri != alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JLESS:
				if ((ucell) pri < (ucell) alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JLEQ:
				if ((ucell) pri <= (ucell) alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JGRTR:
				if ((ucell) pri > (ucell) alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JGEQ:
				if ((ucell) pri >= (ucell) alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JSLESS:
				if (pri < alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JSLEQ:
				if (pri <= alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JSGRTR:
				if (pri > alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_JSGEQ:
				if (pri >= alt)
					cip = (cell *) * cip;
				else
					cip = (cell *) ((uchar *) cip + sizeof(cell));
				break;
			case OP_SHL:
				pri <<= alt;
				break;
			case OP_SHR:
				pri = (ucell) pri >> (int) alt;
				break;
			case OP_SSHR:
				pri >>= alt;
				break;
			case OP_SHL_C_PRI:
				GETPARAM(offs);
				pri <<= offs;
				break;
			case OP_SHL_C_ALT:
				GETPARAM(offs);
				alt <<= offs;
				break;
			case OP_SHR_C_PRI:
				GETPARAM(offs);
				pri = (ucell) pri >> (int) offs;
				break;
			case OP_SHR_C_ALT:
				GETPARAM(offs);
				alt = (ucell) alt >> (int) offs;
				break;
			case OP_SMUL:
				pri *= alt;
				break;
			case OP_SDIV:
				if (alt == 0)
					return AMX_ERR_DIVIDE;

				/* divide must always round down; this is a bit
				 * involved to do in a machine-independent way.
				 */
				offs = (pri % alt + alt) % alt;		/* true modulus */
				pri = (pri - offs) / alt;	/* division result */
				alt = offs;
				break;
			case OP_SDIV_ALT:
				if (pri == 0)
					return AMX_ERR_DIVIDE;

				/* divide must always round down; this is a bit
				 * involved to do in a machine-independent way.
				 */
				offs = (alt % pri + pri) % pri;		/* true modulus */
				pri = (alt - offs) / pri;	/* division result */
				alt = offs;
				break;
			case OP_UMUL:
				pri = (ucell) pri *(ucell) alt;
				break;
			case OP_UDIV:
				if (alt == 0)
					return AMX_ERR_DIVIDE;
				offs = (ucell) pri % (ucell) alt;	/* temporary storage */
				pri = (ucell) pri / (ucell) alt;
				alt = offs;
				break;
			case OP_UDIV_ALT:
				if (pri == 0)
					return AMX_ERR_DIVIDE;
				offs = (ucell) alt % (ucell) pri;	/* temporary storage */
				pri = (ucell) alt / (ucell) pri;
				alt = offs;
				break;
			case OP_ADD:
				pri += alt;
				break;
			case OP_SUB:
				pri -= alt;
				break;
			case OP_SUB_ALT:
				pri = alt - pri;
				break;
			case OP_AND:
				pri &= alt;
				break;
			case OP_OR:
				pri |= alt;
				break;
			case OP_XOR:
				pri ^= alt;
				break;
			case OP_NOT:
				pri = !pri;
				break;
			case OP_NEG:
				pri = -pri;
				break;
			case OP_INVERT:
				pri = ~pri;
				break;
			case OP_ADD_C:
				GETPARAM(offs);
				pri += offs;
				break;
			case OP_SMUL_C:
				GETPARAM(offs);
				pri *= offs;
				break;
			case OP_ZERO_PRI:
				pri = 0;
				break;
			case OP_ZERO_ALT:
				alt = 0;
				break;
			case OP_ZERO:
				GETPARAM(offs);
				*(cell *) (data + (int) offs) = 0;
				break;
			case OP_ZERO_S:
				GETPARAM(offs);
				*(cell *) (data + (int) frm + (int) offs) = 0;
				break;
			case OP_SIGN_PRI:
				if ((pri & 0xff) >= 0x80)
					pri |= ~(ucell) 0xff;
				break;
			case OP_SIGN_ALT:
				if ((alt & 0xff) >= 0x80)
					alt |= ~(ucell) 0xff;
				break;
			case OP_EQ:
				pri = pri == alt ? 1 : 0;
				break;
			case OP_NEQ:
				pri = pri != alt ? 1 : 0;
				break;
			case OP_LESS:
				pri = (ucell) pri < (ucell) alt ? 1 : 0;
				break;
			case OP_LEQ:
				pri = (ucell) pri <= (ucell) alt ? 1 : 0;
				break;
			case OP_GRTR:
				pri = (ucell) pri > (ucell) alt ? 1 : 0;
				break;
			case OP_GEQ:
				pri = (ucell) pri >= (ucell) alt ? 1 : 0;
				break;
			case OP_SLESS:
				pri = pri < alt ? 1 : 0;
				break;
			case OP_SLEQ:
				pri = pri <= alt ? 1 : 0;
				break;
			case OP_SGRTR:
				pri = pri > alt ? 1 : 0;
				break;
			case OP_SGEQ:
				pri = pri >= alt ? 1 : 0;
				break;
			case OP_EQ_C_PRI:
				GETPARAM(offs);
				pri = pri == offs ? 1 : 0;
				break;
			case OP_EQ_C_ALT:
				GETPARAM(offs);
				pri = alt == offs ? 1 : 0;
				break;
			case OP_INC_PRI:
				pri++;
				break;
			case OP_INC_ALT:
				alt++;
				break;
			case OP_INC:
				GETPARAM(offs);
				*(cell *) (data + (int) offs) += 1;
				break;
			case OP_INC_S:
				GETPARAM(offs);
				*(cell *) (data + (int) frm + (int) offs) += 1;
				break;
			case OP_INC_I:
				*(cell *) (data + (int) pri) += 1;
				break;
			case OP_DEC_PRI:
				pri--;
				break;
			case OP_DEC_ALT:
				alt--;
				break;
			case OP_DEC:
				GETPARAM(offs);
				*(cell *) (data + (int) offs) -= 1;
				break;
			case OP_DEC_S:
				GETPARAM(offs);
				*(cell *) (data + (int) frm + (int) offs) -= 1;
				break;
			case OP_DEC_I:
				*(cell *) (data + (int) pri) -= 1;
				break;
			case OP_MOVS:
				GETPARAM(offs);
				memcpy(data + (int) alt, data + (int) pri, (int) offs);
				break;
			case OP_CMPS:
				GETPARAM(offs);
				pri = memcmp(data + (int) alt, data + (int) pri, (int) offs);
				break;
			case OP_FILL:
				GETPARAM(offs);
				for (i = (int) alt; (size_t) offs >= sizeof(cell); i += sizeof(cell), offs -= sizeof(cell))
					*(cell *) (data + i) = pri;
				break;
			case OP_HALT:
				GETPARAM(offs);
				if (retval != NULL)
					*retval = pri;
				return (int) offs;
			case OP_BOUNDS:
				GETPARAM(offs);
				if (pri > offs || pri < 0)
					return AMX_ERR_BOUNDS;
				break;
			case OP_SYSREQ_PRI:
				/* save a few registers */
				amx->cip = (cell) ((uchar *) cip - code);
				amx->hea = hea;
				amx->frm = frm;
				amx->stk = stk;
				num = amx->callback(amx, pri, &pri, (cell *) (data + (int) stk));
				if (num != AMX_ERR_NONE)
					return num;
				break;
			case OP_SYSREQ_C:
				GETPARAM(offs);
				/* save a few registers */
				amx->cip = (cell) ((uchar *) cip - code);
				amx->hea = hea;
				amx->frm = frm;
				amx->stk = stk;
				num = amx->callback(amx, offs, &pri, (cell *) (data + (int) stk));
				if (num != AMX_ERR_NONE)
					return num;
				break;
			case OP_FILE:
				GETPARAM(offs);
				cip = (cell *) ((uchar *) cip + (int) offs);
				assert(0);	/* this code should not occur during execution */
				break;
			case OP_LINE:
				assert((amx->flags & AMX_FLAG_BROWSE) == 0);
				GETPARAM(amx->curline);
				GETPARAM(amx->curfile);
#if !defined NODBGCALLS
				if (debug) {
					amx->frm = frm;
					amx->stk = stk;
					amx->dbgcode = DBG_LINE;
					amx->debug(amx);
				}			/* if */
#endif							/*  */
				break;
			case OP_SYMBOL:
				assert((amx->flags & AMX_FLAG_BROWSE) == 0);
				GETPARAM(offs);
				GETPARAM(amx->dbgaddr);
				GETPARAM(amx->dbgparam);
				amx->dbgname = (char *) cip;
				cip = (cell *) ((uchar *) cip + (int) offs - 2 * sizeof(cell));
#if !defined NODBGCALLS
				amx->dbgcode = DBG_SYMBOL;
				assert((amx->dbgparam >> 8) > 0);	/* local symbols only */
				if (debug) {
					amx->frm = frm;		/* debugger needs this to relocate the symbols */
					amx->debug(amx);
				}			/* if */
#endif							/*  */
				break;
			case OP_SRANGE:
				assert((amx->flags & AMX_FLAG_BROWSE) == 0);
				GETPARAM(amx->dbgaddr);		/* dimension level */
				GETPARAM(amx->dbgparam);	/* length */
#if !defined NODBGCALLS
				amx->dbgcode = DBG_SRANGE;
				if (debug) {
					amx->frm = frm;		/* debugger needs this to relocate the symbols */
					amx->debug(amx);
				}			/* if */
#endif							/*  */
				break;
			case OP_JUMP_PRI:
				cip = (cell *) (code + (int) pri);
				break;
			case OP_SWITCH:
				{
					cell *cptr;

					cptr = (cell *) * cip + 1;	/* +1, to skip the "casetbl" opcode */
					cip = (cell *) * (cptr + 1);	/* preset to "none-matched" case */
					num = (int) *cptr;	/* number of records in the case table */
					for (cptr += 2; num > 0 && *cptr != pri; num--, cptr += 2)
						/* nothing */ ;
					if (num > 0)
						cip = (cell *) * (cptr + 1);	/* case found */
					break;

				}			/* case */
			case OP_CASETBL:
				assert(0);	/* this should not occur during execution */
				break;
			default:
				return AMX_ERR_INVINSTR;
		}					/* switch */
	}						/* for */
#endif							/*  */
}

#endif							/* __GNUC__ */

int amx_SetCallback(AMX * amx, AMX_CALLBACK callback) {
	assert(amx != NULL);
	assert(callback != NULL);
	amx->callback = callback;
	return AMX_ERR_NONE;
}

int amx_SetDebugHook(AMX * amx, AMX_DEBUG debug) {
	assert(amx != NULL);
	assert(debug != NULL);
	amx->debug = debug;
	return AMX_ERR_NONE;
}

int amx_RaiseError(AMX * amx, int error) {
	assert(error > 0);
	amx->error = error;
	return AMX_ERR_NONE;
}

int amx_GetAddr(AMX * amx, cell v, cell ** addr) {
	AMX_HEADER *hdr = (AMX_HEADER *) amx->base;

	if ((v >= amx->hea && v < amx->stk) || v >= amx->stp)
		return AMX_ERR_MEMACCESS;
	*addr = (cell *) (amx->base + (int) (hdr->dat + v));
	return AMX_ERR_NONE;
}

int amx_Allot(AMX * amx, int cells, cell * amx_addr, cell ** native_addr) {
	AMX_HEADER *hdr = (AMX_HEADER *) amx->base;

	if (amx->stk - amx->hea - cells * sizeof(cell) < STKMARGIN)
		return AMX_ERR_MEMORY;
	assert(amx_addr != NULL);
	assert(native_addr != NULL);
	*amx_addr = amx->hea;
	*native_addr = (cell *) (amx->base + (int) (hdr->dat + amx->hea));
	amx->hea += cells * sizeof(cell);
	return AMX_ERR_NONE;
}

int amx_Release(AMX * amx, cell amx_addr) {
	if (amx->hea > amx_addr)
		amx->hea = amx_addr;
	return AMX_ERR_NONE;
}

#define CHARBITS        (8*sizeof(char))
#define CHARMASK      (0xffffffffuL << 8*(4-sizeof(char)))

int amx_StrLen(cell * cstr, int *length) {
	int len;

	if (*cstr > UCHAR_MAX) {
		/* packed string */
		assert(sizeof(char) == 1);	/* Unicode needs different functions */
		len = strlen((char *) cstr);	/* find '\0' */
		if (amx_LittleEndian) {
			/* on Little Endian machines, toggle the last bytes */
			cell c = cstr[len / sizeof(cell)];	/* get last cell */
			len = len - len % sizeof(cell);	/* len = multiple of "cell" bytes */
			while ((c & CHARMASK) != 0) {
				len++;
				c <<= 8 * sizeof(char);
			} /* if */ 
		} /* if */ 
	} else {
		for (len = 0; cstr[len] != 0; len++)
			/* nothing */ ;
	}						/* if */
	*length = len;
	return AMX_ERR_NONE;
}

int amx_SetString(cell * dest, char *source, int pack) {	
	/* the memory blocks should not overlap */
	int len = strlen(source);
	if (pack) {
		/* create a packed string */
		dest[len / sizeof(cell)] = 0;	
		/* clear last bytes of last (semi-filled) cell */
		memcpy(dest, source, len);
		/* On Big Endian machines, the characters are well aligned in the
		 * cells; on Little Endian machines, we must swap all cells.
		 */
		if (amx_LittleEndian) {
			len /= 4;
			while (len >= 0)
				aligncell((ucell *) & dest[len--]);
		} /* if */ 
	} else {
		/* create an unpacked string */
		int i;
		for (i = 0; i < len; i++)
			dest[i] = (cell) source[i];
		dest[len] = 0;
	}						/* if */
	return AMX_ERR_NONE;
}

int amx_GetString(char *dest, cell * source) {
	if (*source > UCHAR_MAX) {
		/* source string is packed */
		cell c = 0;
		int i = sizeof(cell) - 1;

		do {
			if (i == sizeof(cell) - 1)
				c = *source++;
			*dest = (char) (c >> i * CHARBITS);
			i = (i + sizeof(cell) - 1) % sizeof(cell);
		} while (*dest++ != '\0');
	} else {
		/* source string is unpacked */
		while (*source != 0)
			*dest++ = (char) *source++;
		*dest = '\0';		/* store terminator */
	}						/* if */
	return AMX_ERR_NONE;
}

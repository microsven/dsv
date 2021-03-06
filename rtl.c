/* Manage RTL for C-Compiler
	 Copyright (C) 1987 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.	No author or distributor
accepts responsibility to anyone for the consequences of using it
or for whether it serves any particular purpose or works at all,
unless he says so in writing.	Refer to the GNU CC General Public
License for full details.

Everyone is granted permission to copy, modify and redistribute
GNU CC, but only under the conditions described in the
GNU CC General Public License.	 A copy of this license is
supposed to have been given to you along with GNU CC so you
can know your rights and responsibilities.	It should be in a
file named COPYING.	Among other things, the copyright notice
and this notice must be preserved on all copies.	*/


/* This file contains the low level primitives for mallocting,
	 printing and reading rtl expressions and vectors.
	 It also contains some functions for semantic analysis
	 on rtl expressions.	*/

#include "config.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "rtl.h"

/* We use the same obstack used for the tree.
	 That way, the TREE_RTL of top-level variables and functions
	 is permanent.	*/

#define MIN(x,y) ((x < y) ? x : y)

/* Indexed by rtx code, gives number of operands for an rtx with that code.
	 Does NOT include rtx header data (code and links).
	 This array is initialized in init_rtx.	*/

int rtx_length[NUM_RTX_CODE + 1];

/* Indexed by rtx code, gives the name of that kind of rtx, as a C string.	*/

#define DEF_RTL_EXPR(ENUM, NAME, FORMAT)	 NAME ,

char *rtx_name[] = {
#include "rtl.def"	/* rtl expressions are documented here */
};

#undef DEF_RTL_EXPR

/* Indexed by machine mode, gives the name of that machine mode.
	 This name does not include the letters "mode".	*/

#define DEF_MACHMODE(SYM, NAME, TYPE, SIZE, UNIT)	NAME,

char *mode_name[] = {
#include "machmode.def"
};

#undef DEF_MACHMODE

/* Indexed by machine mode, gives the length of the mode, in bytes.
	 GET_MODE_SIZE uses this.	*/

#define DEF_MACHMODE(SYM, NAME, TYPE, SIZE, UNIT)	SIZE,

int mode_size[] = {
#include "machmode.def"
};

#undef DEF_MACHMODE

/* Indexed by machine mode, gives the length of the mode's subunit.
	 GET_MODE_UNIT_SIZE uses this.	*/

#define DEF_MACHMODE(SYM, NAME, TYPE, SIZE, UNIT)	UNIT,

int mode_unit_size[] = {
#include "machmode.def"	/* machine modes are documented here */
};

#undef DEF_MACHMODE

/* Indexed by rtx code, gives a sequence of operand-types for
	 rtx's of that code.	The sequence is a C string in which
	 each charcter describes one operand.	*/

char *rtx_format[] = {
	/* "*" undefined.
		 can cause a warning message
	 "0" field is unused (or used in a phase-dependent manner)
		 prints nothing
	 "i" an integer
		 prints the integer
	 "s" a pointer to a string
		 prints the string
	 "e" a pointer to an rtl expression
		 prints the expression
	 "E" a pointer to a vector that points to a number of rtl expressions
		 prints a list of the rtl expressions
	 "u" a pointer to another insn
		 prints the uid of the insn.	*/

#define DEF_RTL_EXPR(ENUM, NAME, FORMAT)	 FORMAT ,
#include "rtl.def"	/* rtl expressions are defined here */
#undef DEF_RTL_EXPR
};

/* Allocate an rtx vector of N elements.
	 Store the length, and initialize all elements to zero.	*/

rtvec rtvec_alloc ( int n)
{
	rtvec rt;
	int i;

	rt = (rtvec) malloc (sizeof (struct rtvec_def) + (( n - 1) * sizeof (rtunion)));

	/* clear out the vector */
	PUT_NUM_ELEM(rt, n);
	for (i=0; i < n; i++)
		rt->elem[i].rtvec = NULL;	/* @@ not portable due to rtunion */

	return rt;
}

/* Allocate an rtx of code CODE.	The CODE is stored in the rtx;
	 all the rest is initialized to zero.	*/

rtx rtx_alloc ( RTX_CODE code)
{
	rtx rt;
	 int nelts = GET_RTX_LENGTH (code);
	 int length = sizeof (struct rtx_def)
	+ (nelts - 1) * sizeof (rtunion);

	rt = (rtx) malloc(length);

	* (int *) rt = 0;
	PUT_CODE (rt, code);

	return rt;
}

/* Create a new copy of an rtx.
	 Recursively copies the operands of the rtx,
	 except for those few rtx codes that are sharable.	*/

rtx copy_rtx (  rtx orig)
{
	 rtx copy;
	 int i, j;
	 RTX_CODE code;
	 char *format_ptr;

	code = GET_CODE (orig);
	if (code == REG
		|| code == CONST_INT
		|| code == CONST_DOUBLE
		|| code == SYMBOL_REF
		|| code == CODE_LABEL
		|| code == PC
		|| code == CC0)
	return orig;

	copy = rtx_alloc (code);
	PUT_MODE (copy, GET_MODE (orig));
	
	format_ptr = GET_RTX_FORMAT (GET_CODE (copy));

	for (i = 0; i < GET_RTX_LENGTH (GET_CODE (copy)); i++) {
		switch (*format_ptr++) {
		case 'e':
			XEXP (copy, i) = copy_rtx (XEXP (orig, i));
		break;
	
		case 'E':
			XVEC (copy, i) = XVEC (orig, i);
			if (XVEC (orig, i) != NULL) {
				XVEC (copy, i) = rtvec_alloc (XVECLEN (orig, i));
				for (j = 0; j < XVECLEN (copy, i); j++)
					XVECEXP (copy, i, j) = copy_rtx (XVECEXP (orig, i, j));
				}
		break;
	
		default:
			XINT (copy, i) = XINT (orig, i);
		break;
	}
	}
	return copy;
}

/* Return 1 unless X is a value effectively constant.
	 The frame pointer, arg pointer, etc. are considered constant.	*/

int rtx_varies_p ( rtx x)
{
	 RTX_CODE code = GET_CODE (x);
	 int i;
	 char *fmt;

	if (code == MEM || code == QUEUED)
		return 1;

	if (code == CONST || code == CONST_INT || code == UNCHANGING)
		return 0;

	if (code == REG)
		return ! (REGNO (x) == FRAME_POINTER_REGNUM
			|| REGNO (x) == ARG_POINTER_REGNUM);

	fmt = GET_RTX_FORMAT (code);
	for (i = GET_RTX_LENGTH (code) - 1; i >= 0; i--)
		if (fmt[i] == 'e')
			if (rtx_varies_p (XEXP (x, i)))
				return 1;
	return 0;
}

/* Return 1 if X refers to a memory location whose address 
	 is not effectively constant, or if X refers to a BLKmode memory object.	*/

int rtx_addr_varies_p ( rtx x)
{
	 RTX_CODE code = GET_CODE (x);
	 int i;
	 char *fmt;

	if (code == MEM)
		return GET_MODE (x) == BLKmode || rtx_varies_p (XEXP (x, 0));

	fmt = GET_RTX_FORMAT (code);
	for (i = GET_RTX_LENGTH (code) - 1; i >= 0; i--)
		if (fmt[i] == 'e')
			if (rtx_addr_varies_p (XEXP (x, i)))
				return 1;
	return 0;
}

/* Return nonzero if INSN alters memory at an address that is not fixed.	*/

int insn_store_addr_varies_p ( rtx insn)
{
	 rtx x = PATTERN (insn);
	if (GET_CODE (x) == SET || GET_CODE (x) == CLOBBER)
		return rtx_addr_varies_p (SET_DEST (x));
	else if (GET_CODE (x) == PARALLEL) {
		 int i;
		for (i = XVECLEN (x, 0) - 1; i >= 0; i--) {
			rtx y = XVECEXP (x, 0, i);
			if (GET_CODE (y) == SET || GET_CODE (y) == CLOBBER)
				if (rtx_addr_varies_p (SET_DEST (y)))
					return 1;
		}
	}
	return 0;
}

/* Nonzero if  REG appears somewhere within IN.	*/

int reg_mentioned_p( rtx reg, rtx in)
{
	 char *fmt;
	 int i;
	 enum rtx_code code = GET_CODE (in);

	if (GET_CODE (in) == REG)
		return REGNO (in) == REGNO (reg);

	fmt = GET_RTX_FORMAT (code);

	for (i = GET_RTX_LENGTH (code); i >= 0; i--) {
		if (fmt[i] == 'E') {
		 int j;
	for (j = XVECLEN (in, i) - 1; j >= 0; j--)
		if (reg_mentioned_p (reg, XVECEXP (in, i, j)))
		return 1;
	}
		else if (fmt[i] == 'e'
		 && reg_mentioned_p (reg, XEXP (in, i)))
	return 1;
	}
	return 0;
}

/* Nonzero if  REG is used in an insn between
	 FROM_INSN and TO_INSN (exclusive of those two).	*/

int
reg_used_between_p (reg, from_insn, to_insn)
	 rtx reg, from_insn, to_insn;
{
	 rtx insn;
	 RTX_CODE code;
	for (insn = NEXT_INSN (from_insn); insn != to_insn; insn = NEXT_INSN (insn))
	if (((code = GET_CODE (insn)) == INSN
	 || code == JUMP_INSN || code == CALL_INSN)
	&& reg_mentioned_p (reg, PATTERN (insn)))
		return 1;
	return 0;
}

/* Call FUN on each  or MEM that is stored into or clobbered by X.
	 (X would be the pattern of an insn).
	 FUN receives two arguments:
	 the REG, MEM, CC0 or PC being stored in or clobbered,
	 the SET or CLOBBER rtx that does the store.	*/
	 
void
note_stores (x, fun)
	  rtx x;
	 void (*fun) ();
{
	if ((GET_CODE (x) == SET || GET_CODE (x) == CLOBBER))
	{
		 rtx dest = SET_DEST (x);
		while (GET_CODE (dest) == SUBREG
		 || GET_CODE (dest) == VOLATILE
		 || GET_CODE (dest) == ZERO_EXTRACT
		 || GET_CODE (dest) == SIGN_EXTRACT
		 || GET_CODE (dest) == STRICT_LOW_PART)
	dest = XEXP (dest, 0);
		(*fun) (dest, x);
	}
	else if (GET_CODE (x) == PARALLEL)
	{
		 int i;
		for (i = XVECLEN (x, 0) - 1; i >= 0; i--)
	{
	 rtx y = XVECEXP (x, 0, i);
	if (GET_CODE (y) == SET || GET_CODE (y) == CLOBBER)
		{
		 rtx dest = SET_DEST (y);
		while (GET_CODE (dest) == SUBREG
		 || GET_CODE (dest) == VOLATILE
		 || GET_CODE (dest) == ZERO_EXTRACT
		 || GET_CODE (dest) == SIGN_EXTRACT
		 || GET_CODE (dest) == STRICT_LOW_PART)
	dest = XEXP (dest, 0);
		(*fun) (dest, y);
		}
	}
	}
}

/* Return nonzero if  REG's old contents don't survive after INSN.
	 This can be because REG dies in INSN or because INSN entirely sets REG.

	 "Entirely set" means set directly and not through a SUBREG,
	 ZERO_EXTRACT or SIGN_EXTRACT, so no trace of the old contents remains.

	 REG may be a hard or pseudo reg.	Renumbering is not taken into account,
	 but for this use that makes no difference, since regs don't overlap
	 during their lifetimes.	Therefore, this function may be used
	 at any time after deaths have been computed (in flow.c).	*/

int
dead_or_set_p (insn, reg)
	 rtx insn;
	 rtx reg;
{
	 rtx link;
	 int regno = REGNO (reg);

	for (link = REG_NOTES (insn); link; link = XEXP (link, 1))
	if (REGNO (XEXP (link, 0)) == regno
	&& ((enum reg_note) GET_MODE (link) == REG_DEAD
		|| (enum reg_note) GET_MODE (link) == REG_INC))
		return 1;

	if (GET_CODE (PATTERN (insn)) == SET)
	return SET_DEST (PATTERN (insn)) == reg;
	else if (GET_CODE (PATTERN (insn)) == PARALLEL)
	{
		 int i;
		for (i = XVECLEN (PATTERN (insn), 0) - 1; i >= 0; i--)
	{
	if (GET_CODE (XVECEXP (PATTERN (insn), 0, i)) == SET
		&& SET_DEST (XVECEXP (PATTERN (insn), 0, i)) == reg)
		return 1;
	}
	}
	return 0;
}

/* Printing rtl for debugging dumps.	*/

static FILE *outfile;

char spaces[] = "																																								";

static int sawclose = 0;

/* Print IN_RTX onto OUTFILE.	This is the recursive part of printing.	*/

static void
print_rtx (in_rtx)
	  rtx in_rtx;
{
	static int indent;
	 int i, j;
	 char *format_ptr;

	if (sawclose)
	{
		fprintf (outfile, "\n%s",
		 (spaces + (sizeof spaces - indent * 2)));
		sawclose = 0;
	}

	if (in_rtx == 0)
	{
		fprintf (outfile, "(nil)");
		sawclose = 1;
		return;
	}

	/* print name of expression code */
	fprintf (outfile, "(%s", GET_RTX_NAME (GET_CODE (in_rtx)));

	if (in_rtx->in_struct)
	fprintf (outfile, "/s");

	if (GET_MODE (in_rtx) != VOIDmode)
	fprintf (outfile, ":%s", GET_MODE_NAME (GET_MODE (in_rtx)));

	format_ptr = GET_RTX_FORMAT (GET_CODE (in_rtx));

	for (i = 0; i < GET_RTX_LENGTH (GET_CODE (in_rtx)); i++)
	switch (*format_ptr++)
		{
		case 's':
	if (XSTR (in_rtx, i) == 0)
	fprintf (outfile, " \"\"");
	else
	fprintf (outfile, " (\"%s\")", XSTR (in_rtx, i));
	sawclose = 1;
	break;

	/* 0 indicates a field for internal use that should not be printed.	*/
		case '0':
	break;

		case 'e':
	indent += 2;
	if (!sawclose)
	fprintf (outfile, " ");
	print_rtx (XEXP (in_rtx, i));
	indent -= 2;
	break;

		case 'E':
	indent += 2;
	if (sawclose)
	{
		fprintf (outfile, "\n%s",
		 (spaces + (sizeof spaces - indent * 2)));
		sawclose = 0;
	}
	fprintf (outfile, "[ ");
	if (NULL != XVEC (in_rtx, i))
	{
		indent += 2;
		if (XVECLEN (in_rtx, i))
		sawclose = 1;

		for (j = 0; j < XVECLEN (in_rtx, i); j++)
		print_rtx (XVECEXP (in_rtx, i, j));

		indent -= 2;
	}
	if (sawclose)
	fprintf (outfile, "\n%s",
		 (spaces + (sizeof spaces - indent * 2)));

	fprintf (outfile, "] ");
	sawclose = 1;
	indent -= 2;
	break;

		case 'i':
	fprintf (outfile, " %d", XINT (in_rtx, i));
	sawclose = 0;
	break;

		case 'u':
	if (XEXP (in_rtx, i) != NULL)
	fprintf(outfile, " %d", INSN_UID (XEXP (in_rtx, i)));
	else
	fprintf(outfile, " 0");
	sawclose = 0;
	break;

		default:
	fprintf (stderr,
	 "switch format wrong in rtl.print_rtx(). format was: %c.\n",
	 format_ptr[-1]);
	abort ();
		}

	fprintf (outfile, ")");
	sawclose = 1;
}

/* External entry point for printing a chain of INSNs
	 starting with RTX_FIRST onto file OUTF.	*/

void
print_rtl (outf, rtx_first)
	 FILE *outf;
	 rtx rtx_first;
{
	 rtx tmp_rtx;

	outfile = outf;
	sawclose = 0;

	for (tmp_rtx = rtx_first; NULL != tmp_rtx; tmp_rtx = NEXT_INSN (tmp_rtx))
	{
		print_rtx (tmp_rtx);
		fprintf (outfile, "\n");
	}
}

/* Subroutines of read_rtx.	*/

/* Dump code after printing a message.	Used when read_rtx finds
	 invalid data.	*/

static void dump_and_abort (int expected_c, int actual_c, FILE *infile)
{
	int c, i;

	fprintf (stderr, "Expected character %c. Read character %c. At file position: %ld\n",
	 expected_c, actual_c, ftell (infile));
	fprintf (stderr, "Following characters are:\n\t");
	for (i = 0; i < 200; i++)
	{
		c = getc (infile);
		if (EOF == c) break;
		putc (c, stderr);
	}
	fprintf (stderr, "Aborting.\n");
	abort ();
}

/* Read chars from INFILE until a non-whitespace char
	 and return that.	Comments, both Lisp style and C style,
	 are treated as whitespace.
	 Tools such as genflags use this function.	*/

int read_skip_spaces (FILE *infile)
{
	 int c;
	while (c = getc (infile)) {
		if (c == ' ' || c == '\n' || c == '\t' || c == '\f')
			;
		else if (c == ';') {
			while ((c = getc (infile)) && c != '\n') ;
		} else if (c == '/') {
			 int prevc;
			c = getc (infile);
			if (c != '*')
				dump_and_abort ('*', c, infile);
	
			prevc = 0;
			while (c = getc (infile)) {
				if (prevc == '*' && c == '/')
					break;
				prevc = c;
			}
		}
		else break;
	}
	return c;
}

/* Read an rtx code name into the buffer STR[].
	 It is terminated by any of the punctuation chars of rtx printed syntax.	*/

static void read_name ( char *str, FILE *infile)
{
	 char *p;
	 int c;

	c = read_skip_spaces(infile);

	p = str;
	while (1) {
		if (c == ' ' || c == '\n' || c == '\t' || c == '\f')
			break;
		if (c == ':' || c == ')' || c == ']' || c == '"' || c == '/' || c == '(' || c == '[') {
			ungetc (c, infile);
			break;
		}
		*p++ = c;
		c = getc (infile);
	}
	*p = '\0';
}

/* Read an rtx in printed representation from INFILE
	 and return an actual rtx in core constructed accordingly.
	 read_rtx is not used in the compiler proper, but rather in
	 the utilities gen*.c that construct C code from machine descriptions.	*/

rtx read_rtx ( FILE *infile)
{
	 int i, j, list_counter;
	RTX_CODE tmp_code;
	 char *format_ptr;
	/* tmp_char is a buffer used for reading decimal integers
	 and names of rtx types and machine modes.
	 Therefore, 256 must be enough.	*/
	char tmp_char[256];
	rtx return_rtx;
	 int c;
	int tmp_int;

	/* Linked list structure for making RTXs: */
	struct rtx_list
	{
		struct rtx_list *next;
		rtx value;	/* Value of this node...	*/
	};

	c = read_skip_spaces (infile); /* Should be open paren.	*/
	if (c != '(')
		dump_and_abort ('(', c, infile);

	read_name (tmp_char, infile);

	tmp_code = UNKNOWN;

	/* @@ might speed this search up */
	for (i=0; i < NUM_RTX_CODE; i++) {
		if (!(strcmp (tmp_char, GET_RTX_NAME (i)))) {
			tmp_code = (RTX_CODE) i;	/* get value for name */
			break;
		}
	}
	if (tmp_code == UNKNOWN) {
		fprintf (stderr,
		 "Unknown rtx read in rtl.read_rtx(). Code name was %s .",
		 tmp_char);
	}
	/* (NIL) stands for an expression that isn't there.	*/
	if (tmp_code == NIL) {
		/* Discard the closeparen.	*/
		while ((c = getc (infile)) && c != ')');
			return 0;
	}

	return_rtx = rtx_alloc (tmp_code); /* if we end up with an insn expression
				 then we free this space below.	*/
	format_ptr = GET_RTX_FORMAT (GET_CODE (return_rtx));

	/* If what follows is `: mode ', read it and
	 store the mode in the rtx.	*/

	i = read_skip_spaces (infile);
	if (i == ':') {
		 int k;
		read_name (tmp_char, infile);
		for (k = 0; k < NUM_MACHINE_MODE; k++)
			if (!strcmp (GET_MODE_NAME (k), tmp_char))
				break;

		PUT_MODE (return_rtx, (enum machine_mode) k );
	} else
		ungetc (i, infile);

	for (i = 0; i < GET_RTX_LENGTH (GET_CODE (return_rtx)); i++)
		switch (*format_ptr++) {
	/* 0 means a field for internal use only.
	 Don't expect it to be present in the input.	*/
		case '0':
		break;

		case 'e':
		case 'u':
			XEXP (return_rtx, i) = read_rtx (infile);
		break;

		case 'E':
		{
			struct rtx_list *next_rtx, *rtx_list_link;
			struct rtx_list *list_rtx;

			c = read_skip_spaces (infile);
			if (c != '[')
				dump_and_abort ('[', c, infile);

	/* add expressions to a list, while keeping a count */
			next_rtx = NULL;
			list_counter = 0;
			while ((c = read_skip_spaces (infile)) && c != ']') {
				ungetc (c, infile);
				list_counter++;
				rtx_list_link = (struct rtx_list *)
				malloc (sizeof (struct rtx_list));
				rtx_list_link->value = read_rtx (infile);
				if (next_rtx == 0)
					list_rtx = rtx_list_link;
				else
					next_rtx->next = rtx_list_link;
				next_rtx = rtx_list_link;
				rtx_list_link->next = 0;
			}
	/* get vector length and mallocte it */
			XVEC (return_rtx, i) = (list_counter
								? rtvec_alloc (list_counter)
								: NULL);
			if (list_counter > 0) {
				next_rtx = list_rtx;
				for (j = 0; j < list_counter; j++, next_rtx = next_rtx->next)
					XVECEXP (return_rtx, i, j) = next_rtx->value;
			}
	/* close bracket gotten */
	}
	break;

	case 's':
	{
		int saw_paren = 0;
		char *stringbuf;
		int stringbufsize;

		c = read_skip_spaces (infile);
		if (c == '(') {
			saw_paren = 1;
			c = read_skip_spaces (infile);
		}
		if (c != '"')
			dump_and_abort ('"', c, infile);
		j = 0;
		stringbufsize = 10;
		stringbuf = (char *) malloc (stringbufsize + 1);

		while (1) {
			if (j >= stringbufsize - 4) {
				stringbufsize *= 2;
				stringbuf = (char *) realloc (stringbuf, stringbufsize + 1);
			}
			stringbuf[j] = getc(infile); /* Read the string	*/
			if (stringbuf[j] == '\\') {
				stringbuf[j] = getc (infile);	/* Read the string	*/
		/* \; makes stuff for a C string constant containing
		 newline and tab.	*/
				if (stringbuf[j] == ';') {
					strcpy (&stringbuf[j], "\\n\\t");
					j += 3;
				}
			} else if (stringbuf[j] == '"')
				break;
			j++;
		}

		stringbuf[j] = 0;	/* NUL terminate the string	*/
		stringbuf = (char *) realloc (stringbuf, j + 1);

		if (saw_paren) {
			c = read_skip_spaces (infile);
			if (c != ')')
				dump_and_abort (')', c, infile);
		}
		XSTR (return_rtx, i) = stringbuf;
	}
	break;

	case 'i':
		read_name (tmp_char, infile);
		tmp_int = atoi (tmp_char);
		XINT (return_rtx, i) = tmp_int;
	break;

	default:
		fprintf (stderr, "switch format wrong in rtl.read_rtx(). format was: %c.\n", format_ptr[-1]);
		fprintf (stderr, "\tfile position: %ld\n", ftell (infile));
	assert(0);
	}

	c = read_skip_spaces (infile);
	if (c != ')')
		dump_and_abort (')', c, infile);

	return return_rtx;
}

/* This is called once per compilation, before any rtx's are constructed.
	 It initializes the vector `rtx_length'.	*/

void init_rtl()
{
	int i;

	for (i = 0; i < NUM_RTX_CODE; i++)
		rtx_length[i] = strlen (rtx_format[i]);
}

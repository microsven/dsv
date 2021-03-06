/* Generate from machine description:

	 - some flags HAVE_... saying which simple standard instructions are
	 available for this machine.
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


#include <stdio.h>
#include "rtl.h"
#include <assert.h>

int max_recog_operands_flag;
int max_dup_operands_flag;
int max_sets_per_insn_flag;
int register_constraint_flag;

int sets_seen_this_insn;
int dup_operands_seen_this_insn;


void walk_insn_part (rtx part)
{
	register int i, j;
	register RTX_CODE code = GET_CODE (part);
	register char *format_ptr;

	switch (code)
		{
		case SET:
			sets_seen_this_insn++;
			break;

		case MATCH_OPERAND:
			if (XINT (part, 0) > max_recog_operands_flag)
				max_recog_operands_flag = XINT (part, 0);
			if (XSTR (part, 2) && *XSTR (part, 2))
				register_constraint_flag = 1;
			return;

		case MATCH_DUP:
			++dup_operands_seen_this_insn;

		case REG: case CONST_INT: case SYMBOL_REF:
		case LABEL_REF: case PC: case CC0:
			return;
		}

	format_ptr = GET_RTX_FORMAT (GET_CODE (part));

	for (i = 0; i < GET_RTX_LENGTH (GET_CODE (part)); i++)
		switch (*format_ptr++) {
		case 'e':
			walk_insn_part (XEXP (part, i));
		break;
		case 'E':
			if (XVEC (part, i) != NULL)
				for (j = 0; j < XVECLEN (part, i); j++)
				walk_insn_part (XVECEXP (part, i, j));
		break;
		}
}

void gen_insn(rtx insn)
{
	int i;

	/* Walk the insn pattern to gather the #define's status.	*/
	sets_seen_this_insn = 0;
	dup_operands_seen_this_insn = 0;
	for (i = 0; i < XVECLEN (insn, 1); i++) {
		walk_insn_part (XVECEXP (insn, 1, i));
	}
	if (sets_seen_this_insn > max_sets_per_insn_flag)
		max_sets_per_insn_flag = sets_seen_this_insn;
	if (dup_operands_seen_this_insn > max_dup_operands_flag)
		max_dup_operands_flag = dup_operands_seen_this_insn;

	/* Don't mention instructions whose names are the null string.
		 They are in the machine description just to be recognized.	*/
	if (strlen (XSTR (insn, 0)) == 0)
		return;

	printf ("#define HAVE_%s %s\n", XSTR (insn, 0),
		strlen (XSTR (insn, 2)) ? XSTR (insn, 2) : "1");
	printf ("rtx gen_%s();\n", XSTR (insn, 0));
}

int main ( int argc, char **argv)
{
	rtx desc;
	FILE *infile;
	register int c;


	assert(argc > 1);

	infile = fopen (argv[1], "r");
	assert (infile);

	init_rtl ();
	printf ("/* Generated automatically by the program `genflags'\n"
	"from the machine description file `md'.	*/\n\n");

	/* Read the machine description.	*/

	while (1) {
			c = read_skip_spaces (infile);
			if (c == EOF)
				break;
			ungetc (c, infile);

			desc = read_rtx(infile);
			gen_insn(desc);
	}

	return 0;
}

/* Generate from machine description:

	 - some macros CODE_FOR_... giving the insn_code_number value
	 for each of the defined standard insn names.
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

static int insn_code_number;

void gen_insn ( rtx insn)
{
	/* Don't mention instructions whose names are the null string.
	 They are in the machine description just to be recognized.	*/
	if (strlen (XSTR (insn, 0)) != 0)
		printf ("	CODE_FOR_%s = %d,\n", XSTR (insn, 0), insn_code_number);

	insn_code_number++;
}

int main (int argc, char **argv)
{
	rtx desc;
	FILE *infile;
	register int c;

	assert(argc > 1);

	infile = fopen (argv[1], "r");
	assert(infile);

	init_rtl ();

	printf ("/* Generated automatically by the program `gencodes'\n"
		"from the machine description file `md'.	*/\n\n");

	printf ("#ifndef MAX_INSN_CODE\n\n");

	/* Read the machine description.	*/

	insn_code_number = 0;
	printf ("enum insn_code {\n");

	while (1) {
		c = read_skip_spaces (infile);
		if (c == EOF)
			break;
		ungetc (c, infile);

		desc = read_rtx(infile);
		gen_insn (desc);
	}

	printf ("	CODE_FOR_nothing };\n");

	printf ("\n#define MAX_INSN_CODE ((int) CODE_FOR_nothing)\n");

	printf ("#endif /* MAX_INSN_CODE */\n");

	return 0;
}

;;- Machine description for GNU compiler
;;- Motorola 68000 Version
;;   Copyright (C) 1987 Free Software Foundation, Inc.

;; This file is part of GNU CC.

;; GNU CC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY.  No author or distributor
;; accepts responsibility to anyone for the consequences of using it
;; or for whether it serves any particular purpose or works at all,
;; unless he says so in writing.  Refer to the GNU CC General Public
;; License for full details.

;; Everyone is granted permission to copy, modify and redistribute
;; GNU CC, but only under the conditions described in the
;; GNU CC General Public License.   A copy of this license is
;; supposed to have been given to you along with GNU CC so you
;; can know your rights and responsibilities.  It should be in a
;; file named COPYING.  Among other things, the copyright notice
;; and this notice must be preserved on all copies.


;;- instruction definitions

;;- @@The original PO technology requires these to be ordered by speed,
;;- @@    so that assigner will pick the fastest.

;;- See file "rtl.def" for documentation on define_insn, match_*, et. al.

;;- When naming insn's (operand 0 of define_insn) be careful about using
;;- names from other targets machine descriptions.

;;- cpp macro #define NOTICE_UPDATE_CC in file tm.h handles condition code
;;- updates for most instructions.

;;- Operand classes for the register allocator:
;;- 'a' one of the address registers can be used.
;;- 'd' one of the data registers can be used.
;;- 'r' either a data or an address register can be used.

;;- Some of these insn's are composites of several m68000 op codes.
;;- The assembler (or final @@??) insures that the appropriate one is
;;- selected.

;; Note that DBcc cannot be used, since it deals with a 16-bit count only.
;;-  Scc should be added.

(define_insn ""
  [(set (match_operand:DF 0 "push_operand" "=m")
	(match_operand:DF 1 "general_operand" "rofF"))]
  ""
  "*
{
  extern rtx adj_offsetable_operand();
  extern void output_asm_insn();
  rtx newops[2];

  if (FP_REG_P (operands[1]))
    return \"fmoved %1,%0\";
  if (REG_P (operands[1]))
    newops[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
  else
    newops[1] = adj_offsetable_operand (operands[1], 4);
  output_asm_insn (\"movl %1,sp@-\", newops);
  return \"movl %1,sp@-\";
}")

(define_insn ""
  [(set (match_operand:DI 0 "push_operand" "=m")
	(match_operand:DI 1 "general_operand" "ro"))]
  ""
  "*
{
  extern rtx adj_offsetable_operand();
  extern void output_asm_insn();
  rtx newops[2];

  if (REG_P (operands[1]))
    newops[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
  else
    newops[1] = adj_offsetable_operand (operands[1], 4);
  output_asm_insn (\"movl %1,sp@-\", newops);
  return \"movl %1,sp@-\";
}")

(define_insn "tstsi"
  [(set (cc0)
	(match_operand:SI 0 "general_operand" "rm"))]
  ""
  "*
{ if (TARGET_68020 || ! ADDRESS_REG_P (operands[0]))
    return \"tstl %0\";
  return \"cmpl #0,%0\"; }")

(define_insn "tsthi"
  [(set (cc0)
	(match_operand:HI 0 "general_operand" "rm"))]
  ""
  "*
{ if (TARGET_68020 || ! ADDRESS_REG_P (operands[0]))
    return \"tstw %0\";
  return \"cmpw #0,%0\"; }")

(define_insn "tstqi"
  [(set (cc0)
	(match_operand:QI 0 "general_operand" "dm"))]
  ""
  "tstb %0")

(define_insn "tstsf"
  [(set (cc0)
	(match_operand:SF 0 "general_operand" "fdm"))]
  "TARGET_68881"
  "*
{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    return \"ftstx %0\";
  return \"ftsts %0\";
}")

(define_insn "tstdf"
  [(set (cc0)
	(match_operand:DF 0 "general_operand" "fm"))]
  "TARGET_68881"
  "*
{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    return \"ftstx %0\";
  return \"ftstd %0\";
}")

;;- compare instructions
;;- A composite of the cmp, cmpa, & cmpi m68000 op codes.
(define_insn "cmpsi"
  [(set (cc0)
	(minus (match_operand:SI 0 "general_operand" "rKs,mrKs")
	       (match_operand:SI 1 "general_operand" "mrKs,Ksr")))]
  ""
  "*
{
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED; return \"cmpl %0,%1\"; }
  return \"cmpl %1,%0\";
}")

(define_insn "cmphi"
  [(set (cc0)
	(minus (match_operand:HI 0 "general_operand" "ri,g")
	       (match_operand:HI 1 "general_operand" "g,ir")))]
  ""
  "*
{
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED; return \"cmpw %0,%1\"; }
  return \"cmpw %1,%0\";
}")

(define_insn ""
  [(set (cc0)
	(minus (mem:QI (post_inc:SI (match_operand:SI 0 "general_operand" "+a")))
	       (mem:QI (post_inc:SI (match_operand:SI 1 "general_operand" "+a")))))]
  ""
  "cmpmb %1@+,%0@+")

(define_insn "cmpqi"
  [(set (cc0)
	(minus (match_operand:QI 0 "general_operand" "di,m")
	       (match_operand:QI 1 "general_operand" "dmi,id")))]
  ""
  "*
{
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED; return \"cmpb %0,%1\"; }
  return \"cmpb %1,%0\";
}")

(define_insn "cmpdf"
  [(set (cc0)
	(minus:DF (match_operand:DF 0 "general_operand" "f,m")
		  (match_operand:DF 1 "general_operand" "fm,f")))]
  "TARGET_68881"
  "*
{
  cc_status.flags = CC_IN_68881;
  if (REG_P (operands[0]))
    {
      if (REG_P (operands[1]))
	return \"fcmpx %1,%0\";
      else
        return \"fcmpd %1,%0\";
    }
  cc_status.flags |= CC_REVERSED;
  return \"fcmpd %0,%1\";
}")

(define_insn "cmpsf"
  [(set (cc0)
	(minus:SF (match_operand:SF 0 "general_operand" "f,md")
		  (match_operand:SF 1 "general_operand" "fmd,f")))]
  "TARGET_68881"
  "*
{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return \"fcmpx %1,%0\";
      else
        return \"fcmps %1,%0\";
    }
  cc_status.flags |= CC_REVERSED;
  return \"fcmps %0,%1\";
}")

;; Recognizers for btst instructions.

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:QI 0 "general_operand" "dm")
			    (const_int 1)
			    (minus:SI (const_int 7)
				      (match_operand:SI 1 "general_operand" "di"))))]
  ""
  "* { return output_btst (operands, operands[1], operands[0], insn, 7); }")

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:SI 0 "general_operand" "d")
			    (const_int 1)
			    (minus:SI (const_int 31)
				      (match_operand:SI 1 "general_operand" "di"))))]
  ""
  "* { return output_btst (operands, operands[1], operands[0], insn, 31); }")

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:QI 0 "general_operand" "dm")
			    (const_int 1)
			    (match_operand:SI 1 "general_operand" "i")))]
  "GET_CODE (operands[1]) == CONST_INT"
  "*
{ operands[1] = gen_rtx (CONST_INT, VOIDmode, 7 - INTVAL (operands[1]));
  return output_btst (operands, operands[1], operands[0], insn, 7); }")

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:SI 0 "general_operand" "do")
			    (const_int 1)
			    (match_operand:SI 1 "general_operand" "i")))]
  "GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (GET_CODE (operands[0]) == MEM)
    {
      operands[0] = adj_offsetable_operand (operands[0],
					    INTVAL (operands[1]) / 8);
      operands[1] = gen_rtx (CONST_INT, VOIDmode, 
			     7 - INTVAL (operands[1]) % 8);
      return output_btst (operands, operands[1], operands[0], insn, 7);
    }
  operands[1] = gen_rtx (CONST_INT, VOIDmode,
			 31 - INTVAL (operands[1]));
  return output_btst (operands, operands[1], operands[0], insn, 31);
}")

(define_insn ""
  [(set (cc0) (subreg:SI (lshiftrt:QI (match_operand:QI 0 "general_operand" "dm")
				      (const_int 7))
			 0))]
  ""
  "*
{
  cc_status.flags = CC_Z_IN_NOT_N | CC_NOT_NEGATIVE;
  return \"tstb %0\";
}")

(define_insn ""
  [(set (cc0) (and:SI (sign_extend:SI (sign_extend:HI (match_operand:QI 0 "general_operand" "dm")))
		      (match_operand:SI 1 "general_operand" "i")))]
  "(GET_CODE (operands[1]) == CONST_INT
    && (unsigned) INTVAL (operands[1]) < 0x100
    && exact_log2 (INTVAL (operands[1])) >= 0)"
  "*
{ register int log = exact_log2 (INTVAL (operands[1]));
  operands[1] = gen_rtx (CONST_INT, VOIDmode, log);
  return output_btst (operands, operands[1], operands[0], insn, 7);
}")

;;- move instructions
(define_insn "swapsi"
  [(set (match_operand:SI 0 "general_operand" "r")
	(match_operand:SI 1 "general_operand" "r"))
   (set (match_dup 1) (match_dup 0))]
  ""
  "exg %1,%2")

;; Special case of fullword move when source is zero.
;; The reason this is special is to avoid loading a zero
;; into a data reg with moveq in order to store it elsewhere.
   
(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=g")
	(const_int 0))]
  ""
  "*
{
  if (ADDRESS_REG_P (operands[0]))
    return \"movw #0,%0\";
  return \"clrl %0\";
}")
   
;; Another special case in which it is not desirable
;; to reload the constant into a data register.
(define_insn ""
  [(set (match_operand:SI 0 "push_operand" "=m")
	(match_operand:SI 1 "general_operand" "J"))]
  "GET_CODE (operands[1]) == CONST_INT
   && INTVAL (operands[1]) >= -0x8000
   && INTVAL (operands[1]) < 0x8000"
  "pea %a1")

;; General case of fullword move.  The register constraints
;; force integer constants in range for a moveq to be reloaded
;; if they are headed for memory.
(define_insn "movsi"
  [(set (match_operand:SI 0 "general_operand" "=g,da")
	(match_operand:SI 1 "general_operand" "damKs,i"))]
  ""
  "*
{
  if (GET_CODE (operands[1]) == CONST_INT)
    {
      if (operands[1] == const0_rtx
	  && (DATA_REG_P (operands[0])
	      || GET_CODE (operands[0]) == MEM))
	return \"clrl %0\";
      else if (DATA_REG_P (operands[0])
	       && INTVAL (operands[1]) < 128
	       && INTVAL (operands[1]) >= -128)
        return \"moveq %1,%0\";
      else if (ADDRESS_REG_P (operands[0])
	       && INTVAL (operands[1]) < 0x8000
	       && INTVAL (operands[1]) >= -0x8000)
	return \"movw %1,%0\";
      else if (push_operand (operands[0], SImode)
	       && INTVAL (operands[1]) < 0x8000
	       && INTVAL (operands[1]) >= -0x8000)
        return \"pea %a1\";
    }
  else if ((GET_CODE (operands[1]) == SYMBOL_REF
	    || GET_CODE (operands[1]) == CONST)
	   && push_operand (operands[0], SImode))
    return \"pea %a1\";
  else if ((GET_CODE (operands[1]) == SYMBOL_REF
	    || GET_CODE (operands[1]) == CONST)
	   && ADDRESS_REG_P (operands[0]))
    return \"lea %a1,%0\";
  return \"movl %1,%0\";
}")

(define_insn "movhi"
  [(set (match_operand:HI 0 "general_operand" "=g")
	(match_operand:HI 1 "general_operand" "g"))]
  ""
  "*
{
  if (GET_CODE (operands[1]) == CONST_INT)
    {
      if (operands[1] == const0_rtx
	  && (DATA_REG_P (operands[0])
	      || GET_CODE (operands[0]) == MEM))
	return \"clrw %0\";
    }
  return \"movw %1,%0\";
}")

(define_insn "movstricthi"
  [(set (strict_low_part (match_operand:HI 0 "general_operand" "=dm"))
	(match_operand:HI 1 "general_operand" "g"))]
  ""
  "*
{
  if (GET_CODE (operands[1]) == CONST_INT)
    {
      if (operands[1] == const0_rtx
	  && (DATA_REG_P (operands[0])
	      || GET_CODE (operands[0]) == MEM))
	return \"clrw %0\";
    }
  return \"movw %1,%0\";
}")

(define_insn "movqi"
  [(set (match_operand:QI 0 "general_operand" "=d,a,m")
	(match_operand:QI 1 "general_operand" "g,d,dmi"))]
  ""
  "*
{
  if (operands[1] == const0_rtx)
    return \"clrb %0\";
  if (ADDRESS_REG_P (operands[0]) || ADDRESS_REG_P (operands[1]))
    return \"movw %1,%0\";
  return \"movb %1,%0\";
}")

(define_insn "movestrictqi"
  [(set (strict_low_part (match_operand:QI 0 "general_operand" "=d,m"))
	(match_operand:QI 1 "general_operand" "g,dmi"))]
  ""
  "*
{
  if (operands[1] == const0_rtx)
    return \"clrb %0\";
  if (ADDRESS_REG_P (operands[0]) || ADDRESS_REG_P (operands[1]))
    return \"movw %1,%0\";
  return \"movb %1,%0\";
}")

(define_insn "movsf"
  [(set (match_operand:SF 0 "general_operand" "=gf")
	(match_operand:SF 1 "general_operand" "gf"))]
  ""
  "*
{
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return \"fmovex %1,%0\";
      else
        return \"fmoves %1,%0\";
    }
  if (FP_REG_P (operands[1]))
    return \"fmoves %1,%0\";
  return \"movl %1,%0\";
}")

(define_insn "movdf"
  [(set (match_operand:DF 0 "general_operand" "=rmf")
	(match_operand:DF 1 "general_operand" "rmfF"))]
  ""
  "*
{
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return \"fmovex %1,%0\";
      if (DATA_REG_P (operands[1]))
	{
	  rtx xoperands[2];
	  xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
	  output_asm_insn (\"movl %1,sp@-\", xoperands);
	  output_asm_insn (\"movl %1,sp@-\", operands);
	  return \"fmoved sp@+,%0\";
	}
      if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_double (operands);
      return \"fmoved %1,%0\";
    }
  else if (FP_REG_P (operands[1]))
    {
      if (DATA_REG_P (operands[0]))
	{
	  output_asm_insn (\"fmoved %1,sp@-\;movl sp@+,%0\", operands);
	  operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
	  return \"movl sp@+,%0\";
	}
      else
        return \"fmoved %1,%0\";
    }
  return output_move_double (operands);
}
")

(define_insn "movdi"
  [(set (match_operand:DI 0 "general_operand" "=rmf")
	(match_operand:DI 1 "general_operand" "rmifF"))]
  ""
  "*
{
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return \"fmovex %1,%0\";
      if (DATA_REG_P (operands[1]))
	{
	  rtx xoperands[2];
	  xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
	  output_asm_insn (\"movl %1,sp@-\", xoperands);
	  output_asm_insn (\"movl %1,sp@-\", operands);
	  return \"fmoved sp@+,%0\";
	}
      if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_double (operands);
      return \"fmoved %1,%0\";
    }
  else if (FP_REG_P (operands[1]))
    {
      if (DATA_REG_P (operands[0]))
	{
	  output_asm_insn (\"fmoved %1,sp@-\;movl sp@+,%0\", operands);
	  operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
	  return \"movl sp@+,%0\";
	}
      else
        return \"fmoved %1,%0\";
    }
  return output_move_double (operands);
}
")

;; These go after the move instructions
;; because the move instructions are better (require no spilling)
;; when they can apply.  But these go before the add and subtract insns
;; because it is often shorter to use these when both apply.
(define_insn "pushasi"
  [(set (match_operand:SI 0 "push_operand" "=m")
	(match_operand:SI 1 "address_operand" "p"))]
  ""
  "pea %a1")


(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=a")
	(match_operand:QI 1 "address_operand" "p"))]
  ""
  "lea %a1,%0")

;;- conversion instructions
;;- NONE

;;- truncation instructions
(define_insn "truncsiqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(truncate:QI
	 (match_operand:SI 1 "general_operand" "doi")))]
  ""
  "*
{
  if (! REG_P (operands[1]))
    operands[1] = adj_offsetable_operand (operands[1], 3);
  return \"movb %1,%0\";
}")

(define_insn "trunchiqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(truncate:QI
	 (match_operand:HI 1 "general_operand" "doi")))]
  ""
  "*
{
  if (! REG_P (operands[1]))
    operands[1] = adj_offsetable_operand (operands[1], 1);
  return \"movb %1,%0\";
}")

(define_insn "truncsihi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(truncate:HI
	 (match_operand:SI 1 "general_operand" "roi")))]
  ""
  "*
{
  if (! REG_P (operands[1]))
    operands[1] = adj_offsetable_operand (operands[1], 2);
  return \"movw %1,%0\";
}")

;;- zero extension instructions
(define_insn "zero_extendqihi2"
  [(set (match_operand:HI 0 "general_operand" "=do<>")
	(zero_extend:HI
	 (match_operand:QI 1 "general_operand" "dmi")))]
  ""
  "*
{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return \"andw #0xFF,%0\";
      if (reg_mentioned_p (operands[0], operands[1]))
        return \"movb %1,%0\;andw #0xFF,%0\";
      return \"clrw %0\;movb %1,%0\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    {
      if (REGNO (XEXP (XEXP (operands[0], 0), 0))
	  == STACK_POINTER_REGNUM)
	return \"clrw sp@-\;movb %1,%0\";
      else
	return \"movb %1,%0\;clrb %0\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    return \"clrb %0\;movb %1,%0\";
  else
    {
      output_asm_insn (\"clrb %0\", operands);
      operands[0] = adj_offsetable_operand (operands[0], 1);
      return \"movb %1,%0\";
    }
}")

(define_insn "zero_extendqisi2"
  [(set (match_operand:SI 0 "general_operand" "=do<>")
	(zero_extend:SI
	 (match_operand:QI 1 "general_operand" "dmi")))]
  ""
  "*
{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return \"andl #0xFF,%0\";
      if (reg_mentioned_p (operands[0], operands[1]))
        return \"movb %1,%0\;andl #0xFF,%0\";
      return \"clrl %0\;movb %1,%0\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    {
      operands[0] = XEXP (XEXP (operands[0], 0), 0);
      return \"clrl %0@-\;movb %1,%0@(3)\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    {
      operands[0] = XEXP (XEXP (operands[0], 0), 0);
      return \"clrl %0@+\;movb %1,%0@(-1)\";
    }
  else
    {
      output_asm_insn (\"clrl %0\", operands);
      operands[0] = adj_offsetable_operand (operands[0], 3);
      return \"movb %1,%0\";
    }
}")

(define_insn "zero_extendhisi2"
  [(set (match_operand:SI 0 "general_operand" "=do<")
	(zero_extend:SI
	 (match_operand:HI 1 "general_operand" "g")))]
  ""
  "*
{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return \"andl #0xFFFF,%0\";
      if (reg_mentioned_p (operands[0], operands[1]))
        return \"movw %1,%0\;andl #0xFFFF,%0\";
      return \"clrl %0\;movw %1,%0\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    return \"movw %1,%0\;clrw %0\";
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    return \"clrw %0\;movw %1,%0\";
  else
    {
      output_asm_insn (\"clrw %0\", operands);
      operands[0] = adj_offsetable_operand (operands[0], 2);
      return \"movw %1,%0\";
    }
}")

;;- sign extension instructions
(define_insn "extendqihi2"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(sign_extend:HI
	 (match_operand:QI 1 "general_operand" "0")))]
  ""
  "extw %0")

(define_insn "extendqisi2"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(sign_extend:SI
	 (match_operand:QI 1 "general_operand" "0")))]
  "TARGET_68020"
  "extbl %0")

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "general_operand" "=*d,a")
	(sign_extend:SI
	 (match_operand:HI 1 "general_operand" "0,g")))]
  ""
  "*
{
  if (ADDRESS_REG_P (operands[0]))
    return \"movw %1,%0\";
  return \"extl %0\";
}")

;; Conversions between float and double.

(define_insn "extendsfdf2"
  [(set (match_operand:DF 0 "general_operand" "=*fg,f")
	(float_extend:DF
	 (match_operand:SF 1 "general_operand" "f,g")))]
  ""
  "*
{
  if (FP_REG_P (operands[0]) && FP_REG_P (operands[1]))
    return \"fmovex %1,%0\";
  if (FP_REG_P (operands[0]))
    return \"fmoves %1,%0\";
  if (DATA_REG_P (operands[0]) && FP_REG_P (operands[1]))
    {
      output_asm_insn (\"fmoved %1,sp@-\;movl sp@+,%0\", operands);
      operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
      return \"movl sp@+,%0\";
    }
  return \"fmoved %1,%0\";
}")

;; This cannot output into an f-reg because there is no way to be
;; sure of truncating in that case.
(define_insn "truncdfsf2"
  [(set (match_operand:SF 0 "general_operand" "=g")
	(float_truncate:SF
	 (match_operand:DF 1 "general_operand" "f")))]
  ""
  "*
{
  if (FP_REG_P (operands[0]) && FP_REG_P (operands[1]))
    return \"fmovex %1,%0\";
  if (FP_REG_P (operands[0]) && DATA_REG_P (operands[1]))
    {
      rtx xoperands[2];
      xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
      output_asm_insn (\"movl %1,sp@-\", xoperands);
      output_asm_insn (\"movl %1,sp@-\", operands);
      return \"fmoved sp@+,%0\";
    }
  if (FP_REG_P (operands[0]))
    return \"fmoved %1,%0\";
  return \"fmoves %1,%0\";
}")

;; Conversion between fixed point and floating point.

(define_insn "floatqisf2"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(float:SF (match_operand:QI 1 "general_operand" "dmi")))]
  "TARGET_68881"
  "fmoveb %1,%0")

(define_insn "floatqidf2"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(float:DF (match_operand:QI 1 "general_operand" "dmi")))]
  "TARGET_68881"
  "fmoveb %1,%0")

(define_insn "floathisf2"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(float:SF (match_operand:HI 1 "general_operand" "dmi")))]
  "TARGET_68881"
  "fmovew %1,%0")

(define_insn "floathidf2"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(float:DF (match_operand:HI 1 "general_operand" "dmi")))]
  "TARGET_68881"
  "fmovew %1,%0")

(define_insn "floatsisf2"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(float:SF (match_operand:SI 1 "general_operand" "dmi")))]
  "TARGET_68881"
  "fmovel %1,%0")

(define_insn "floatsidf2"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(float:DF (match_operand:SI 1 "general_operand" "dmi")))]
  "TARGET_68881"
  "fmovel %1,%0")

(define_insn "fixsfqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(fix:QI (match_operand:SF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmoveb %1,%0")

(define_insn "fixsfhi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(fix:HI (match_operand:SF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovew %1,%0")

(define_insn "fixsfsi2"
  [(set (match_operand:SI 0 "general_operand" "=dm")
	(fix:SI (match_operand:SF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovel %1,%0")

(define_insn "fixdfqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(fix:QI (match_operand:DF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmoveb %1,%0")

(define_insn "fixdfhi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(fix:HI (match_operand:DF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovew %1,%0")

(define_insn "fixdfsi2"
  [(set (match_operand:SI 0 "general_operand" "=dm")
	(fix:SI (match_operand:DF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovel %1,%0")

;;- add instructions
;;- A composite of the add, adda, addi, & addq m68000 op codes.

(define_insn "addsi3"
  [(set (match_operand:SI 0 "general_operand" "=%m,r,!a")
	(plus:SI (match_operand:SI 1 "general_operand" "0,0,a")
		 (match_operand:SI 2 "general_operand" "dIKLs,mrIKLs,rJ")))]
  ""
  "*
{
  if (! rtx_equal_p (operands[0], operands[1]))
    {
      if (GET_CODE (operands[2]) == REG)
	return \"lea %1@(%2),%0\";
      else
	return \"lea %1@(%c2),%0\";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return \"addql %2,%0\";
      if (INTVAL (operands[2]) < 0
	  && INTVAL (operands[2]) >= -8)
        {
	  operands[2] = gen_rtx (CONST_INT, VOIDmode,
			         - INTVAL (operands[2]));
	  return \"subql %2,%0\";
	}
      if (ADDRESS_REG_P (operands[0])
	  && INTVAL (operands[2]) >= -0x8000
	  && INTVAL (operands[2]) < 0x8000)
	return \"addw %2,%0\";
    }
  return \"addl %2,%0\";
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=a")
	(plus:SI (match_operand:SI 1 "general_operand" "0")
		 (sign_extend:SI (match_operand:HI 2 "general_operand" "g"))))]
  ""
  "addw %2,%0")

(define_insn "addhi3"
  [(set (match_operand:HI 0 "general_operand" "=%m,r")
	(plus:HI (match_operand:HI 1 "general_operand" "0,0")
		 (match_operand:HI 2 "general_operand" "di,g")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return \"addqw %2,%0\";
    }
  return \"addw %2,%0\";
}")

(define_insn ""
  [(set (strict_low_part (match_operand:HI 0 "general_operand" "+m,d"))
	(plus:HI (match_dup 0)
		 (match_operand:HI 1 "general_operand" "di,g")))]
  ""
  "addw %1,%0")

(define_insn "addqi3"
  [(set (match_operand:QI 0 "general_operand" "=%m,d")
	(plus:QI (match_operand:QI 1 "general_operand" "0,0")
		 (match_operand:QI 2 "general_operand" "di,dmi")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return \"addqb %2,%0\";
    }
  return \"addb %2,%0\";
}")

(define_insn ""
  [(set (strict_low_part (match_operand:QI 0 "general_operand" "+m,d"))
	(plus:QI (match_dup 0)
		 (match_operand:QI 1 "general_operand" "di,dmi")))]
  ""
  "addb %1,%0")

(define_insn "adddf3"
  [(set (match_operand:DF 0 "general_operand" "=%f")
	(plus:DF (match_operand:DF 1 "general_operand" "0")
		 (match_operand:DF 2 "general_operand" "fmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]))
    return \"faddx %2,%0\";
  return \"faddd %2,%0\";
}")

(define_insn "addsf3"
  [(set (match_operand:SF 0 "general_operand" "=%f")
	(plus:SF (match_operand:SF 1 "general_operand" "0")
		 (match_operand:SF 2 "general_operand" "fdmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return \"faddx %2,%0\";
  return \"fadds %2,%0\";
}")

;;- subtract instructions
;;- A composite of the sub, suba, subi, & subq m68000 op codes.
(define_insn "subsi3"
  [(set (match_operand:SI 0 "general_operand" "=m,r,!a,r")
	(minus:SI (match_operand:SI 1 "general_operand" "0,0,a,mrIKs")
		  (match_operand:SI 2 "general_operand" "dIKs,mrIKs,J,0")))]
  ""
  "*
{
  if (! rtx_equal_p (operands[0], operands[1]))
    {
      if (rtx_equal_p (operands[0], operands[2]))
	{
	  if (GET_CODE (operands[1]) == CONST_INT)
	    {
	      if (INTVAL (operands[1]) > 0
		  && INTVAL (operands[1]) <= 8)
		return \"subql %1,%0\;negl %0\";
	      if (ADDRESS_REG_P (operands[0])
		  && INTVAL (operands[1]) >= -0x8000
		  && INTVAL (operands[1]) < 0x8000)
		return \"subw %1,%0\;negl %0\";
	    }
	  return \"subl %1,%0\;negl %0\";
	}
      /* This case is matched by J, but negating -0x8000
         in an lea would give an invalid displacement.
	 So do this specially.  */
      if (INTVAL (operands[2]) == -0x8000)
	return \"movl %1,%0\;subl %2,%0\";
      return \"lea %1@(%n2),%0\";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return \"subql %2,%0\";
      if (ADDRESS_REG_P (operands[0])
	  && INTVAL (operands[2]) >= -0x8000
	  && INTVAL (operands[2]) < 0x8000)
	return \"subw %2,%0\";
    }
  return \"subl %2,%0\";
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=a")
	(minus:SI (match_operand:SI 1 "general_operand" "0")
		  (sign_extend:SI (match_operand:HI 2 "general_operand" "g"))))]
  ""
  "subw %2,%0")

(define_insn "subhi3"
  [(set (match_operand:HI 0 "general_operand" "=m,r")
	(minus:HI (match_operand:HI 1 "general_operand" "0,0")
		  (match_operand:HI 2 "general_operand" "di,g")))]
  ""
  "subw %2,%0")

(define_insn ""
  [(set (strict_low_part (match_operand:HI 0 "general_operand" "+m,d"))
	(minus:HI (match_dup 0)
		  (match_operand:HI 1 "general_operand" "di,g")))]
  ""
  "subw %1,%0")

(define_insn "subqi3"
  [(set (match_operand:QI 0 "general_operand" "=m,d")
	(minus:QI (match_operand:QI 1 "general_operand" "0,0")
		  (match_operand:QI 2 "general_operand" "di,dmi")))]
  ""
  "subb %2,%0")

(define_insn ""
  [(set (strict_low_part (match_operand:QI 0 "general_operand" "+m,d"))
	(minus:QI (match_dup 0)
		  (match_operand:QI 1 "general_operand" "di,dmi")))]
  ""
  "subb %1,%0")

(define_insn "subdf3"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(minus:DF (match_operand:DF 1 "general_operand" "0")
		  (match_operand:DF 2 "general_operand" "fmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]))
    return \"fsubx %2,%0\";
  return \"fsubd %2,%0\";
}")

(define_insn "subsf3"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(minus:SF (match_operand:SF 1 "general_operand" "0")
		  (match_operand:SF 2 "general_operand" "fdmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return \"fsubx %2,%0\";
  return \"fsubs %2,%0\";
}")

;;- multiply instructions

(define_insn "mulhi3"
  [(set (match_operand:HI 0 "general_operand" "=%d")
	(mult:HI (match_operand:HI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "muls %2,%0")

(define_insn "mulhisi3"
  [(set (match_operand:SI 0 "general_operand" "=%d")
	(mult:SI (match_operand:HI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "muls %2,%0")

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "general_operand" "=%d")
	(mult:SI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:SI 2 "general_operand" "dmsK")))]
  "TARGET_68020"
  "mulsl %2,%0")

(define_insn "umulhi3"
  [(set (match_operand:HI 0 "general_operand" "=%d")
	(umult:HI (match_operand:HI 1 "general_operand" "0")
		  (match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "mulu %2,%0")

(define_insn "umulhisi3"
  [(set (match_operand:SI 0 "general_operand" "=%d")
	(umult:SI (match_operand:HI 1 "general_operand" "0")
		  (match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "mulu %2,%0")

(define_insn "umulsi3"
  [(set (match_operand:SI 0 "general_operand" "=%d")
	(umult:SI (match_operand:SI 1 "general_operand" "0")
		  (match_operand:SI 2 "general_operand" "dmsK")))]
  "TARGET_68020"
  "mulul %2,%0")

(define_insn "muldf3"
  [(set (match_operand:DF 0 "general_operand" "=%f")
	(mult:DF (match_operand:DF 1 "general_operand" "0")
		 (match_operand:DF 2 "general_operand" "fmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]))
    return \"fmulx %2,%0\";
  return \"fmuld %2,%0\";
}")

(define_insn "mulsf3"
  [(set (match_operand:SF 0 "general_operand" "=%f")
	(mult:SF (match_operand:SF 1 "general_operand" "0")
		 (match_operand:SF 2 "general_operand" "fdmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return \"fsglmulx %2,%0\";
  return \"fsglmuls %2,%0\";
}")

;;- divide instructions

(define_insn "divhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(div:HI (match_operand:HI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "extw %0\;divs %2,%0")

(define_insn "divhisi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(div:HI (match_operand:SI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "divs %2,%0")

(define_insn "divsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(div:SI (match_operand:SI 1 "general_operand" "0")
		(match_operand:SI 2 "general_operand" "dmsK")))]
  ""
  "divsl %2,%0")

(define_insn "udivhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(udiv:HI (match_operand:HI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "andl #0xFFFF,%0\;divu %2,%0")

(define_insn "udivhisi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(udiv:HI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "divu %2,%0")

(define_insn "udivsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(udiv:SI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:SI 2 "general_operand" "dmsK")))]
  ""
  "divul %2,%0")

(define_insn "divdf3"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(div:DF (match_operand:DF 1 "general_operand" "0")
		(match_operand:DF 2 "general_operand" "fmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]))
    return \"fdivx %2,%0\";
  return \"fdivd %2,%0\";
}")

(define_insn "divsf3"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(div:SF (match_operand:SF 1 "general_operand" "0")
		(match_operand:SF 2 "general_operand" "fdmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return \"fsgldivx %2,%0\";
  return \"fsgldivs %2,%0\";
}")

;; Remainder instructions.

(define_insn "modhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(mod:HI (match_operand:HI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "extw %0\;divs %2,%0\;swap %0")

(define_insn "modhisi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(mod:HI (match_operand:SI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "divs %2,%0\;swap %0")

(define_insn "umodhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(umod:HI (match_operand:HI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "andl #0xFFFF,%0\;divu %2,%0\;swap %0")

(define_insn "umodhisi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(umod:HI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmi")))]
  ""
  "divu %2,%0\;swap %0")

(define_insn "divmodsi4"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(div:SI (match_operand:SI 1 "general_operand" "0")
		(match_operand:SI 2 "general_operand" "dmsK")))
   (set (match_operand:SI 3 "general_operand" "=d")
	(mod:SI (match_dup 1) (match_dup 2)))]
  "TARGET_68020"
  "divsll %2,%3:%0")

(define_insn "udivmodsi4"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(udiv:SI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:SI 2 "general_operand" "dmsK")))
   (set (match_operand:SI 3 "general_operand" "=d")
	(umod:SI (match_dup 1) (match_dup 2)))]
  "TARGET_68020"
  "divull %2,%3:%0")

;;- and instructions
;;- A composite of the and, & andi m68000 op codes.

;; First, recognize a subcase of andsi that can usually be optimized.
;; Doing this with the main pattern for andsi would require a new
;; register-class letter for negative constants
;; and that's not worth adding for one use.
(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=%md")
	(and:SI (match_operand:SI 1 "general_operand" "0")
		(match_operand:SI 2 "general_operand" "i")))]
  "(GET_CODE (operands[1]) == CONST_INT && (INTVAL (operands[1]) | 0xffff) == 0xffffffff)
   || (GET_CODE (operands[2]) == CONST_INT && (INTVAL (operands[2]) | 0xffff) == 0xffffffff)"
  "*
{
  if (DATA_REG_P (operands[0])
      || offsetable_address_p (operands[0]))
    { 
      if (GET_CODE (operands[0]) != REG)
        operands[0] = adj_offsetable_operand (operands[0], 2);
      return \"andw %2,%0\";
    }
  return \"andl %2,%0\";
}")

(define_insn "andsi3"
  [(set (match_operand:SI 0 "general_operand" "=%m,d")
	(and:SI (match_operand:SI 1 "general_operand" "0,0")
		(match_operand:SI 2 "general_operand" "dKs,dmKs")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT
      && (INTVAL (operands[2]) | 0xffff) == 0xffffffff
      && (DATA_REG_P (operands[0])
	  || offsetable_address_p (operands[0])))
    { 
      if (GET_CODE (operands[0]) != REG)
        operands[0] = adj_offsetable_operand (operands[0], 2);
      return \"andw %2,%0\";
    }
  return \"andl %2,%0\";
}")

(define_insn "andhi3"
  [(set (match_operand:HI 0 "general_operand" "=%m,d")
	(and:HI (match_operand:HI 1 "general_operand" "0,0")
		(match_operand:HI 2 "general_operand" "di,dmi")))]
  ""
  "andw %2,%0")

(define_insn "andqi3"
  [(set (match_operand:QI 0 "general_operand" "=%m,d")
	(and:QI (match_operand:QI 1 "general_operand" "0,0")
		(match_operand:QI 2 "general_operand" "di,dmi")))]
  ""
  "andb %2,%0")

;;- Bit set (inclusive or) instructions
;;- A composite of the or, & ori m68000 op codes.
(define_insn "iorsi3"
  [(set (match_operand:SI 0 "general_operand" "=%m,d")
	(ior:SI (match_operand:SI 1 "general_operand" "0,0")
		(match_operand:SI 2 "general_operand" "dKs,dmKs")))]
  ""
  "*
{
  register int logval;
  if (GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[2]) >> 16 == 0
      && (DATA_REG_P (operands[0])
	  || offsetable_address_p (operands[0])))
    { 
      if (GET_CODE (operands[0]) != REG)
        operands[0] = adj_offsetable_operand (operands[0], 2);
      return \"orw %2,%0\";
    }
  if (GET_CODE (operands[2]) == CONST_INT
      && (logval = exact_log2 (INTVAL (operands[2]))) >= 0
      && (DATA_REG_P (operands[0])
	  || offsetable_address_p (operands[0])))
    { 
      if (DATA_REG_P (operands[0]))
	{
	  operands[1] = gen_rtx (CONST_INT, VOIDmode, logval);
	}
      else
        {
	  operands[0] = adj_offsetable_operand (operands[0], 3 - (logval / 8));
	  operands[1] = gen_rtx (CONST_INT, VOIDmode, logval % 8);
	}
      return \"bset %1,%0\";
    }
  return \"orl %2,%0\";
}")

(define_insn "iorhi3"
  [(set (match_operand:HI 0 "general_operand" "=%m,d")
	(ior:HI (match_operand:HI 1 "general_operand" "0,0")
		(match_operand:HI 2 "general_operand" "di,dmi")))]
  ""
  "orw %2,%0")

(define_insn "iorqi3"
  [(set (match_operand:QI 0 "general_operand" "=%m,d")
	(ior:QI (match_operand:QI 1 "general_operand" "0,0")
		(match_operand:QI 2 "general_operand" "di,dmi")))]
  ""
  "orb %2,%0")

;;- xor instructions
;;- A composite of the eor, & eori m68000 op codes.
(define_insn "xorsi3"
  [(set (match_operand:SI 0 "general_operand" "=%dm")
	(xor:SI (match_operand:SI 1 "general_operand" "0")
		(match_operand:SI 2 "general_operand" "dKs")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[2]) >> 16 == 0
      && offsetable_address_p (operands[0]))
    { 
      operands[0] = adj_offsetable_operand (operands[0], 2);
      return \"eorw %2,%0\";
    }
  return \"eorl %2,%0\";
}")

(define_insn "xorhi3"
  [(set (match_operand:HI 0 "general_operand" "=%dm")
	(xor:HI (match_operand:HI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "di")))]
  ""
  "eorw %2,%0")

(define_insn "xorqi3"
  [(set (match_operand:QI 0 "general_operand" "=%dm")
	(xor:QI (match_operand:QI 1 "general_operand" "0")
		(match_operand:QI 2 "general_operand" "di")))]
  ""
  "eorb %2,%0")

;;- negation instructions
(define_insn "negsi2"
  [(set (match_operand:SI 0 "general_operand" "=dm")
	(neg:SI (match_operand:SI 1 "general_operand" "0")))]
  ""
  "negl %0")

(define_insn "neghi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(neg:HI (match_operand:HI 1 "general_operand" "0")))]
  ""
  "negw %0")

(define_insn "negqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(neg:QI (match_operand:QI 1 "general_operand" "0")))]
  ""
  "negb %0")

(define_insn "negsf2"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(neg:SF (match_operand:SF 1 "general_operand" "fdm")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return \"fnegx %1,%0\";
  return \"fnegs %1,%0\";
}")

(define_insn "negdf2"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(neg:DF (match_operand:DF 1 "general_operand" "fmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return \"fnegx %1,%0\";
  return \"fnegd %1,%0\";
}")

;; Absolute value instructions

(define_insn "abssf2"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(abs:SF (match_operand:SF 1 "general_operand" "fdm")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return \"fabsx %1,%0\";
  return \"fabss %1,%0\";
}")

(define_insn "absdf2"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(abs:DF (match_operand:DF 1 "general_operand" "fmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return \"fabsx %1,%0\";
  return \"fabsd %1,%0\";
}")

;;- one complement instructions
(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "general_operand" "=dm")
	(not:SI (match_operand:SI 1 "general_operand" "0")))]
  ""
  "notl %0")

(define_insn "one_cmplhi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(not:HI (match_operand:HI 1 "general_operand" "0")))]
  ""
  "notw %0")

(define_insn "one_cmplqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(not:QI (match_operand:QI 1 "general_operand" "0")))]
  ""
  "notb %0")

;; Optimized special case of shifting.
;; Must precede the general case.

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ashiftrt:SI (match_operand:SI 1 "memory_operand" "m")
		     (const_int 24)))]
  ""
  "*
{
  if (TARGET_68020)
    return \"movb %1,%0\;extbl %0\";
  return \"movb %1,%0\;extw %0\;extl %0\";
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(lshiftrt:SI (match_operand:SI 1 "memory_operand" "m")
		     (const_int 24)))]
  ""
  "*
{
  if (reg_mentioned_p (operands[0], operands[1]))
    return \"movb %1,%0\;andl #0xFF,%0\";
  return \"clrl %0\;movb %1,%0\";
}")

(define_insn ""
  [(set (cc0) (minus (match_operand:QI 0 "general_operand" "i")
		     (lshiftrt:SI (match_operand:SI 1 "memory_operand" "m")
				  (const_int 24))))]
  "(GET_CODE (operands[0]) == CONST_INT
    && (INTVAL (operands[0]) & ~0xff) == 0)"
  "* cc_status.flags |= CC_REVERSED; return \"cmpb %0,%1\"; ")

(define_insn ""
  [(set (cc0) (minus (lshiftrt:SI (match_operand:SI 0 "memory_operand" "m")
				  (const_int 24))
		     (match_operand:QI 1 "general_operand" "i")))]
  "(GET_CODE (operands[1]) == CONST_INT
    && (INTVAL (operands[1]) & ~0xff) == 0)"
  "cmpb %1,%0")

(define_insn ""
  [(set (cc0) (minus (match_operand:QI 0 "general_operand" "i")
		     (ashiftrt:SI (match_operand:SI 1 "memory_operand" "m")
				  (const_int 24))))]
  "(GET_CODE (operands[0]) == CONST_INT
    && ((INTVAL (operands[0]) + 0x80) & ~0xff) == 0)"
  "* cc_status.flags |= CC_REVERSED; return \"cmpb %0,%1\"; ")

(define_insn ""
  [(set (cc0) (minus (ashiftrt:SI (match_operand:SI 0 "memory_operand" "m")
				  (const_int 24))
		     (match_operand:QI 1 "general_operand" "i")))]
  "(GET_CODE (operands[1]) == CONST_INT
    && ((INTVAL (operands[1]) + 0x80) & ~0xff) == 0)"
  "cmpb %1,%0")

;;- arithmetic shift instructions
;;- don't need the shift memory by 1 bit instruction
(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ashift:SI (match_operand:SI 1 "general_operand" "0")
		   (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "asll %2,%0")

(define_insn "ashlhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(ashift:HI (match_operand:HI 1 "general_operand" "0")
		   (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "aslw %2,%0")

(define_insn "ashlqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(ashift:QI (match_operand:QI 1 "general_operand" "0")
		   (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "aslb %2,%0")

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ashiftrt:SI (match_operand:SI 1 "general_operand" "0")
		     (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "asrl %2,%0")

(define_insn "ashrhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(ashiftrt:HI (match_operand:HI 1 "general_operand" "0")
		     (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "asrw %2,%0")

(define_insn "ashrqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(ashiftrt:QI (match_operand:QI 1 "general_operand" "0")
		     (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "asrb %2,%0")

;;- logical shift instructions
;;- don't need the shift memory by 1 bit instruction
(define_insn "lshlsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(lshift:SI (match_operand:SI 1 "general_operand" "0")
		   (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "lsll %2,%0")

(define_insn "lshlhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(lshift:HI (match_operand:HI 1 "general_operand" "0")
		   (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "lslw %2,%0")

(define_insn "lshlqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(lshift:QI (match_operand:QI 1 "general_operand" "0")
		   (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "lslb %2,%0")

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(lshiftrt:SI (match_operand:SI 1 "general_operand" "0")
		     (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "lsrl %2,%0")

(define_insn "lshrhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(lshiftrt:HI (match_operand:HI 1 "general_operand" "0")
		     (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "lsrw %2,%0")

(define_insn "lshrqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(lshiftrt:QI (match_operand:QI 1 "general_operand" "0")
		     (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "lsrb %2,%0")

;;- rotate instructions
;;- don't need the shift memory by 1 bit instruction

(define_insn "rotlsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(rotate:SI (match_operand:SI 1 "general_operand" "0")
		   (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "roll %2,%0")

(define_insn "rotlhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(rotate:HI (match_operand:HI 1 "general_operand" "0")
		   (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "rolw %2,%0")

(define_insn "rotlqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(rotate:QI (match_operand:QI 1 "general_operand" "0")
		   (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "rolb %2,%0")

(define_insn "rotrsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(rotatert:SI (match_operand:SI 1 "general_operand" "0")
		     (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "rorl %2,%0")

(define_insn "rotrhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(rotatert:HI (match_operand:HI 1 "general_operand" "0")
		     (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "rorw %2,%0")

(define_insn "rotrqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(rotatert:QI (match_operand:QI 1 "general_operand" "0")
		     (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "rorb %2,%0")

;; Bit field instructions.

(define_insn "extv"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(sign_extract:SI (match_operand:QI 1 "general_operand" "dm")
			 (match_operand:SI 2 "general_operand" "di")
			 (match_operand:SI 3 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfexts %1{%3:%2},%0")

(define_insn "extzv"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(zero_extract:SI (match_operand:QI 1 "general_operand" "dm")
			 (match_operand:SI 2 "general_operand" "di")
			 (match_operand:SI 3 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfextu %1{%3:%2},%0")

(define_insn ""
  [(set (zero_extract:SI (match_operand:QI 0 "general_operand" "+dm")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(const_int 0))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfclr %0{%2:%1}")

(define_insn ""
  [(set (zero_extract:SI (match_operand:QI 0 "general_operand" "+dm")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(const_int -1))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfset %0{%2:%1}")

;; double d in "+ddm" increases pressure to put operand 0 in data reg.
(define_insn "insv"
  [(set (zero_extract:SI (match_operand:QI 0 "general_operand" "+ddm")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(match_operand:SI 3 "general_operand" "d"))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfins %3,%0{%2:%1}")

;; Now recognize bit field insns that operate on registers
;; (or at least were intended to do so).

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(sign_extract:SI (match_operand:SI 1 "general_operand" "d")
			 (match_operand:SI 2 "general_operand" "di")
			 (match_operand:SI 3 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfexts %1{%3:%2},%0")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(zero_extract:SI (match_operand:SI 1 "general_operand" "d")
			 (match_operand:SI 2 "general_operand" "di")
			 (match_operand:SI 3 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfextu %1{%3:%2},%0")

(define_insn ""
  [(set (zero_extract:SI (match_operand:SI 0 "general_operand" "+d")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(const_int 0))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfclr %0{%2:%1}")

(define_insn ""
  [(set (zero_extract:SI (match_operand:SI 0 "general_operand" "+d")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(const_int -1))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfset %0{%2:%1}")

;; double d in "+ddm" increases pressure to put operand 0 in data reg.
(define_insn ""
  [(set (zero_extract:SI (match_operand:SI 0 "general_operand" "+d")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(match_operand:SI 3 "general_operand" "d"))]
  "TARGET_68020 && TARGET_BITFIELD"
  "*
{
  if (GET_CODE (operands[1]) == CONST_INT && GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[1]) == 16 && INTVAL (operands[2]) == 16)
    return \"movw %3,%0\";
  if (GET_CODE (operands[1]) == CONST_INT && GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[1]) == 24 && INTVAL (operands[2]) == 8)
    return \"movb %3,%0\";
  return \"bfins %3,%0{%2:%1}\";
}")

;; Special patterns for optimizing bit-field instructions.

(define_insn ""
  [(set (cc0)
	(zero_extract:SI (match_operand:QI 0 "general_operand" "dm")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst %0{%2:%1}\";
}")

(define_insn ""
  [(set (cc0)
	(subreg:QI
	 (zero_extract:SI (match_operand:QI 0 "general_operand" "dm")
			  (match_operand:SI 1 "general_operand" "di")
			  (match_operand:SI 2 "general_operand" "di"))
	 0))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst %0{%2:%1}\";
}")

(define_insn ""
  [(set (cc0)
	(subreg:HI
	 (zero_extract:SI (match_operand:QI 0 "general_operand" "dm")
			  (match_operand:SI 1 "general_operand" "di")
			  (match_operand:SI 2 "general_operand" "di"))
	 0))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst %0{%2:%1}\";
}")


(define_insn "seqsi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(eq (cc0) (const_int 0)))]
  ""
  "* OUTPUT_JUMP (\"seq %0\", \"fseq %0\", \"seq %0\"); ")

(define_insn "snesi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ne (cc0) (const_int 0)))]
  ""
  "* OUTPUT_JUMP (\"sne %0\", \"fsne %0\", \"sne %0\"); ")

(define_insn "sgtsi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(gt (cc0) (const_int 0)))]
  ""
  "* OUTPUT_JUMP (\"sgt %0\", \"fsgt %0\", \"andb #0xc,cc\;sgt %0\"); ")

(define_insn "sgtusi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(gtu (cc0) (const_int 0)))]
  ""
  "shi %0")

(define_insn "sltsi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(lt (cc0) (const_int 0)))]
  ""
  "* OUTPUT_JUMP (\"slt %0\", \"fslt %0\", \"smi %0\"); ")

(define_insn "sltusi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ltu (cc0) (const_int 0)))]
  ""
  "scs %0")

(define_insn "sgesi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ge (cc0) (const_int 0)))]
  ""
  "* OUTPUT_JUMP (\"sge %0\", \"fsge %0\", \"spl %0\"); ")

(define_insn "sgeusi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(geu (cc0) (const_int 0)))]
  ""
  "scc %0")  

(define_insn "slesi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(le (cc0) (const_int 0)))]
  ""
  "* OUTPUT_JUMP (\"sle %0\", \"fsle %0\", \"andb #0xc,cc\;sle %0\"); ")

(define_insn "sleusi"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(leu (cc0) (const_int 0)))]
  ""
  "sls %0")

;; Basic conditional jump instructions.

(define_insn "beq"
  [(set (pc)
	(if_then_else (eq (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
{
  if (cc_status.flags & CC_Z_IN_NOT_N)
    return \"jpl %l0\";
  OUTPUT_JUMP (\"jeq %l0\", \"fjeq %l0\", \"jeq %l0\");
}")

(define_insn "bne"
  [(set (pc)
	(if_then_else (ne (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
{
  if (cc_status.flags & CC_Z_IN_NOT_N)
    return \"jmi %l0\";
  OUTPUT_JUMP (\"jne %l0\", \"fjne %l0\", \"jne %l0\");
}")

(define_insn "bgt"
  [(set (pc)
	(if_then_else (gt (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "* OUTPUT_JUMP (\"jgt %l0\", \"fjgt %l0\", \"andb #0xc,cc\;jgt %l0\"); ")

(define_insn "bgtu"
  [(set (pc)
	(if_then_else (gtu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "jhi %l0")

(define_insn "blt"
  [(set (pc)
	(if_then_else (lt (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "* OUTPUT_JUMP (\"jlt %l0\", \"fjlt %l0\", \"jmi %l0\"); ")

(define_insn "bltu"
  [(set (pc)
	(if_then_else (ltu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "jcs %l0")

(define_insn "bge"
  [(set (pc)
	(if_then_else (ge (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "* OUTPUT_JUMP (\"jge %l0\", \"fjge %l0\", \"jpl %l0\"); ")

(define_insn "bgeu"
  [(set (pc)
	(if_then_else (geu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "jcc %l0")

(define_insn "ble"
  [(set (pc)
	(if_then_else (le (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "* OUTPUT_JUMP (\"jle %l0\", \"fjle %l0\", \"andb #0xc,cc\;jle %l0\"); ")

(define_insn "bleu"
  [(set (pc)
	(if_then_else (leu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "jls %l0")

;; Negated conditional jump instructions.

(define_insn ""
  [(set (pc)
	(if_then_else (eq (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
{
  if (cc_status.flags & CC_Z_IN_NOT_N)
    return \"jmi %l0\";
  OUTPUT_JUMP (\"jne %l0\", \"fjne %l0\", \"jne %l0\");
}")

(define_insn ""
  [(set (pc)
	(if_then_else (ne (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
{
  if (cc_status.flags & CC_Z_IN_NOT_N)
    return \"jpl %l0\";
  OUTPUT_JUMP (\"jeq %l0\", \"fjeq %l0\", \"jeq %l0\");
}")

(define_insn ""
  [(set (pc)
	(if_then_else (gt (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "* OUTPUT_JUMP (\"jle %l0\", \"fjngt %l0\", \"andb #0xc,cc\;jle %l0\"); ")

(define_insn ""
  [(set (pc)
	(if_then_else (gtu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "jls %l0")

(define_insn ""
  [(set (pc)
	(if_then_else (lt (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "* OUTPUT_JUMP (\"jge %l0\", \"fjnlt %l0\", \"jpl %l0\"); ")

(define_insn ""
  [(set (pc)
	(if_then_else (ltu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "jcc %l0")

(define_insn ""
  [(set (pc)
	(if_then_else (ge (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "* OUTPUT_JUMP (\"jlt %l0\", \"fjnge %l0\", \"jmi %l0\"); ")

(define_insn ""
  [(set (pc)
	(if_then_else (geu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "jcs %l0")

(define_insn ""
  [(set (pc)
	(if_then_else (le (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "* OUTPUT_JUMP (\"jgt %l0\", \"fjnle %l0\", \"andb #0xc,cc\;jgt %l0\"); ")

(define_insn ""
  [(set (pc)
	(if_then_else (leu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "jhi %l0")

;; Unconditional and other jump instructions
(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "jra %l0")

(define_insn "tablejump"
  [(set (pc)
	(plus:SI (pc) (match_operand:HI 0 "general_operand" "r")))]
  ""
  "jmp pc@(2,%0:w)")

(define_insn ""
  [(set (pc)
	(if_then_else
	 (ne (minus (minus:HI (match_operand:HI 0 "general_operand" "g")
			      (const_int 1))
		    (const_int -1))
	     (const_int 0))
	 (label_ref (match_operand 1 "" "g"))
	 (pc)))
   (set (match_dup 0)
	(minus:HI (match_dup 0)
		  (const_int 1)))]
  ""
  "*
{
  if (DATA_REG_P (operands[0]))
    return \"jdbra %0,%l1\";
  return \"subqw #1,%0\;cmpw #-1,%0\;jne %l1\";
}")

(define_insn ""
  [(set (pc)
	(if_then_else
	 (ne (minus (const_int -1)
		    (minus:SI (match_operand:SI 0 "general_operand" "g")
			      (const_int 1)))
	     (const_int 0))
	 (label_ref (match_operand 1 "" "g"))
	 (pc)))
   (set (match_dup 0)
	(minus:SI (match_dup 0)
		  (const_int 1)))]
  ""
  "*
{
  if (DATA_REG_P (operands[0]))
    return \"jdbra %0,%l1\;clrw %0\;subql #1,%0\;cmpl #-1,%0\;jne %l1\";
  return \"subql #1,%0\;cmpl #-1,%0\;jne %l1\";
}")

;;- jump to subroutine
(define_insn "call"
  [(call (match_operand:QI 0 "general_operand" "m")
	 (match_operand:SI 1 "general_operand" "g"))]
  ;;- Don't use operand 1 for the m68000.
  ""
  "jbsr %0")

(define_insn "return"
  [(return)]
  ""
  "rts")

;;- Local variables:
;;- mode:emacs-lisp
;;- comment-start: ";;- "
;;- eval: (set-syntax-table (copy-sequence (syntax-table)))
;;- eval: (modify-syntax-entry ?[ "(]")
;;- eval: (modify-syntax-entry ?] ")[")
;;- eval: (modify-syntax-entry ?{ "(}")
;;- eval: (modify-syntax-entry ?} "){")
;;- End:

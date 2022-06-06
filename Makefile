objs:= insn-recog.o insn-emit.o insn-extract.o insn-output.o
headers=$(patsubst %.o, %.h, $(objs))
srcs:=$(patsubst %.o, %.c, $(objs))

.PHONY: all clean
all: insn-recog.o insn-emit.o insn-extract.o insn-output.o

insn-config.h : md genconfig
	./genconfig md > insn-config.h

insn-flags.h : md genflags
	./genflags md > insn-flags.h

insn-codes.h : md gencodes
	./gencodes md > insn-codes.h

insn-emit.o : insn-emit.c rtl.h insn-config.h
	$(CC) -c $(CFLAGS) insn-emit.c

insn-emit.c : md genemit
	./genemit md > insn-emit.c

insn-recog.o : insn-recog.c config.h rtl.h insn-config.h
	$(CC) -c $(CFLAGS) insn-recog.c

insn-recog.c : md genrecog
	./genrecog md > insn-recog.c

insn-extract.o : insn-extract.c rtl.h
	$(CC) -c $(CFLAGS) insn-extract.c

insn-extract.c : md genextract
	./genextract md > insn-extract.c

insn-output.o : insn-output.c config.h rtl.h regs.h insn-config.h insn-flags.h conditions.h aux-output.c
	$(CC) -c $(CFLAGS) insn-output.c

insn-output.c : md genoutput
	./genoutput md > insn-output.c

# Now the programs that generate those files.
# rtl.o is omitted as a dependency so that rtl.c can be recompiled
# to fix its tables without forcing us to regenerate insn-*.c
# and recompile them and regenerate insn-flags.h and recompile
# everything that depends on it.

genconfig : genconfig.o  rtl.o
	$(CC) -o genconfig  genconfig.o rtl.o 

genconfig.o : genconfig.c rtl.def
	$(CC) -c $(CFLAGS) genconfig.c

genflags : genflags.o  rtl.o
	$(CC) -o genflags  genflags.o rtl.o 

genflags.o : genflags.c rtl.def
	$(CC) -c $(CFLAGS) genflags.c

gencodes : gencodes.o rtl.o
	$(CC) -o gencodes  gencodes.o rtl.o 

gencodes.o : gencodes.c rtl.def
	$(CC) -c $(CFLAGS) gencodes.c

genemit : genemit.o rtl.o
	$(CC) -o genemit  genemit.o rtl.o 

genemit.o : genemit.c rtl.def
	$(CC) -c $(CFLAGS) genemit.c

genrecog : genrecog.o rtl.o
	$(CC) -o genrecog  genrecog.o rtl.o 

genrecog.o : genrecog.c rtl.def
	$(CC) -c $(CFLAGS) genrecog.c

genextract : genextract.o 
	$(CC) -o genextract  genextract.o rtl.o 

genextract.o : genextract.c rtl.def
	$(CC) -c $(CFLAGS) genextract.c

genoutput : genoutput.o 
	$(CC) -o genoutput  genoutput.o rtl.o 

genoutput.o : genoutput.c rtl.def
	$(CC) -c $(CFLAGS) genoutput.c

rtl.o : rtl.c
	$(CC) -c $(CFLAGS) rtl.c

clean:
	rm -f *.o insn-*
	rm -f $(srcs) $(headers)
	rm -f genoutput genextract genrecog genemit genconfig genflags gencodes

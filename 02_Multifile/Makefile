.PHONY: clean all distclean
CFLAGS = -Wall -fPIC
GENERATES = prog prog-a prog-so README
TRASH = *.o *~ o.* *.a *.so outfile*

%.o:     %.c
	cc $< $(CFLAGS) -c -o $@

all:     README prog prog-a prog-so

prog:    const.o fun.o prog.o
	cc $^ -o $@

prog-a:  prog.o liboutput_static.a
	cc -static -o prog-a prog.o -L. -loutput_static

prog-so: prog.o liboutput.so
	cc -L. prog.o -loutput -o prog-so

liboutput_static.a: fun.o const.o
	ar rcs liboutput_static.a const.o fun.o

liboutput.so: fun.o const.o
	cc -shared fun.o const.o -o liboutput.so	

fun.o:  outlib.h

README: prog
	./$< 2> $@

test: prog prog-a prog-so
	$(call test_run,0,)
	$(call test_run,1,test)
	$(call test_run,3,123 456 789)

define test_run
	./prog $(2) > outfile$(1) 2>&1
	./prog-a $(2) > outfile$(1)-a 2>&1
	LD_LIBRARY_PATH=./ ./prog-so $(2) > outfile$(1)-so 2>&1
	cmp outfile$(1) outfile$(1)-a
	cmp outfile$(1) outfile$(1)-so
	cmp outfile$(1)-a outfile$(1)-so
endef

clean:
	rm -f $(TRASH)

distclean:      clean
	rm -rf $(GENERATES)

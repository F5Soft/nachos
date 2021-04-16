MAKE = make
LPR = lpr

all: 
	cd threads; $(MAKE) depend
	cd threads; $(MAKE) nachos
	cd userprog; $(MAKE) depend 
	cd userprog; $(MAKE) nachos 
	cd vm; $(MAKE) depend
	cd vm; $(MAKE) nachos 
	cd filesys; $(MAKE) depend
	cd filesys; $(MAKE) nachos 
	cd network; $(MAKE) depend
	cd network; $(MAKE) nachos 
	cd bin; make all
	cd test; make all

clean:
	rm -f *~ 
	rm -f */*~
	rm -f */core
	rm -f */nachos
	rm -f */DISK
	rm -f */*.o
	rm -f */swtch.s
	rm -f */*.coff
	rm -f test/*.coff
	rm -f test/halt
	rm -f test/matmult
	rm -f test/shell
	rm -f test/sort
	rm -f bin/coff2flat
	rm -f bin/coff2noff
	rm -f bin/disassemble
	rm -f bin/out  

print:
	/bin/csh -c "$(LPR) Makefile* */Makefile"
	/bin/csh -c "$(LPR) threads/*.h threads/*.cc threads/*.s"
	/bin/csh -c "$(LPR) userprog/*.h userprog/*.cc" 
	/bin/csh -c "$(LPR) filesys/*.h filesys/*.cc
	/bin/csh -c "$(LPR) network/*.h network/*.cc 
	/bin/csh -c "$(LPR) machine/*.h machine/*.cc
	/bin/csh -c "$(LPR) bin/noff.h bin/coff.h bin/coff2noff.c"
	/bin/csh -c "$(LPR) test/*.h test/*.c test/*.s"

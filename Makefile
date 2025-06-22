.PHONY: linux
.PHONY: olivetti
.PHONY: h8_80816
.PHONY: isis

linux: say
olivetti: olivetti/say.cmd
h8-80186: h8-80186/say.com
isis: isis/say

say: tabc.py rules.c tts.c ttsmain.c rules.h tts.h test.c outphon.c
	python ./tabc.py > rules.c 	
	gcc -DLINUX -c rules.c
	gcc -DLINUX -c tts.c
	gcc -DLINUX -c ttsmain.c
	gcc -DLINUX -c test.c
	gcc -DLINUX -c outphon.c
	gcc -o say rules.o tts.o ttsmain.o outphon.o
	gcc -o test rules.o tts.o test.o outphon.o

olivetti/say.cmd: tabc.py rules.c tts.c ttsmain.c oliport.c outphone.c oliport.h rules.h tts.h outphon.h
	z8k-pcos-gcc -o olivetti/say.cmd tts.c ttsmain.c rules.c oliport.c outphon.c -Wl,-multi,-map,olivetti/say.map -O2 -D OLIVETTI

h8-80186/say.com: tabc.py rules.c tts.c ttsmain.c outphon.c rules.h tts.h outphon.h
	WATCOM=/usr/bin/watcom INCLUDE=/usr/bin/watcom/h PATH=$PATH:/usr/bin/watcom/binl64 wcl -mt tts.c ttsmain.c rules.c outphon.c -dH8_80186 -fe=h8-80186/say.com
	cp h8-80186/say.com /home/smbaker/projects/pi/h8/h8-80186/commands/

# for ISIS
ISIS=../ipds/simulator/isis/gnu/isis
ZCC = ../z88dk/bin/zcc
export PATH := $(PATH):../z88dk/bin
export ZCCCFG := ../z88dk/lib/config
export ISIS_F1=../ipds/simulator/isis/intel/plm80
export ISIS_F2=../ipds/simulator/isis/intel/utils
export ISIS_F3=../ipds/simulator/isis/intel/asm80
export MAKEISIS=../ipds/asm/makeisis.py
export SYSPDS_LIB=../ipds/asm/syspds.lib

isis: isis/say

isis/say.obj: tabc.py rules.c tts.c ttsmain.c oliport.c outphon.c oliport.h rules.h tts.h outphon.h
	mkdir -p isis
	rm -f isis/say.tmp isis/say.asm
	$(ZCC) +isis tts.c ttsmain.c rules.c outphon.c -DFULLPROTO -DISIS -DMUTEPORT=0x64 -DSPEECHPORT=0x62 --list -m -s -O0 --c-code-in-asm  -o isis/say.tmp
	python3 $(MAKEISIS) < isis/say.tmp > isis/say.asm
	unix2dos isis/say.asm
	cp isis/say.lis isis/say.clis
	cp isis/say.map isis/say.cmap
	$(ISIS) :f3:asm80 isis/say.asm

isis/say: isis/say.obj
	cp isis/say.obj .
	cp $(SYSPDS_LIB) .
	$(ISIS) :f2:link say.obj,syspds.lib to say.lnk MAP
	$(ISIS) :f2:locate say.lnk to say PURGE MAP PRINT\(say.map\)
	rm -f say.obj
	mv say.lnk isis/
	mv say.map isis/
	mv say isis/

isis/say.hex: isis/say.obj
	cp isis/say .
	$(ISIS) :f2:objhex say to say.hex
	cp say.hex isis/
	rm -f say
	rm -f say.hex

say-run:
	$(ISIS) isis/say

# isisrb is like isis but with the ports on the ramboard, not the ports on the ipds

isisrb: isisrb/say

isisrb/say.obj: tabc.py rules.c tts.c ttsmain.c oliport.c outphon.c oliport.h rules.h tts.h outphon.h
	mkdir -p isisrb
	rm -f isisrb/say.tmp isisrb/say.asm
	$(ZCC) +isis tts.c ttsmain.c rules.c outphon.c -DFULLPROTO -DISIS -DMUTEPORT=0xA4 -DSPEECHPORT=0xA2 --list -m -s -O0 --c-code-in-asm  -o isisrb/say.tmp
	python3 $(MAKEISIS) < isisrb/say.tmp > isisrb/say.asm
	unix2dos isisrb/say.asm
	cp isisrb/say.lis isisrb/say.clis
	cp isisrb/say.map isisrb/say.cmap
	$(ISIS) :f3:asm80 isisrb/say.asm

isisrb/say: isisrb/say.obj
	cp isisrb/say.obj .
	cp $(SYSPDS_LIB) .
	$(ISIS) :f2:link say.obj,syspds.lib to say.lnk MAP
	$(ISIS) :f2:locate say.lnk to say PURGE MAP PRINT\(say.map\)
	rm -f say.obj
	mv say.lnk isisrb/
	mv say.map isisrb/
	mv say isisrb/

isisrb/say.hex: isisrb/say.obj
	cp isisrb/say .
	$(ISIS) :f2:objhex say to say.hex
	cp say.hex isisrb/
	rm -f say
	rm -f say.hex

up:
	rm -rf holding
	mkdir holding
	cp rules.c rules.h tts.c tts.h ttsmain.c makesay.sub holding/
	python ~/projects/pi/z8000/cpm8kdisks/addeof.py holding/*.c holding/*.h holding/*.sub
	cpmrm -f cpm8k ~/projects/pi/z8000/super/sup.img rules.c rules.h tts.c tts.h ttsmain.c makesay.sub || true
	cpmcp -f cpm8k ~/projects/pi/z8000/super/sup.img holding/* 0:

.PHONY: down
down:
	mkdir -p down
	rm -f down/say.z8k
	cpmcp -f cpm8k ~/projects/pi/z8000/super/sup.img 0:SAY.Z8K down/

clean:
	rm -f *.o say test isis/* isisrb/*

listimg:
	cpmls -f cpm8k -D ~/projects/pi/z8000/super/sup.img

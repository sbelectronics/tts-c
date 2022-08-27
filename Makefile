say: tabc.py rules.c tts.c ttsmain.c rules.h tts.h test.c
	python ./tabc.py > rules.c 	
	gcc -DLINUX -c rules.c
	gcc -DLINUX -c tts.c
	gcc -DLINUX -c ttsmain.c
	gcc -DLINUX -c test.c
	gcc -o say rules.o tts.o ttsmain.o
	gcc -o test rules.o tts.o test.o

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
	rm *.o say test

listimg:
	cpmls -f cpm8k -D ~/projects/pi/z8000/super/sup.img

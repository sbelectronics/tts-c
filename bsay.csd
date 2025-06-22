cc86 outphon.c include(:sd:inc/) define(multibus) define(SPEECHPORT,0XA2) define(MUTEPORT,0XA4)
cc86 tts.c include(:sd:inc/) define(multibus)
cc86 ttsmain.c include(:sd:inc/) define(multibus)
cc86 rules.c include(:sd:inc/) define(multibus)
link86 outphon.obj,tts.obj,ttsmain.obj,rules.obj,ports.obj,/lib/cc86/sqmain.obj,/lib/cc86/sclib.lib,/lib/small.lib,/lib/cc86/87null.lib to say BIND SEGSIZE(STACK(2000H),MEMORY(2000H)) MAP

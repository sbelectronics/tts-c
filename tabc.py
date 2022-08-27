#
# tabc.py
# Convert retrospeak python rules to C.
#

from __future__ import print_function

from en_US_rules import Rules, Anything, Nothing, Pause, Silent

allophones = { 'PA1':0, 'PA2':1, 'PA3':2, 'PA4':3, 'PA5':4, 'OY':5, 'AY':6, 'EH':7, 
             'KK3':8, 'PP':9, 'JH':10, 'NN1':11, 'IH':12, 'TT2':13, 'RR1':14, 'AX':15, 
             'MM':16, 'TT1':17, 'DH1':18, 'IY':19, 'EY':20, 'DD1':21, 'UW1':22, 'AO':23, 
             'AA':24, 'YY2':25, 'AE':26, 'HH1':27, 'BB1':28, 'TH':29, 'UH':30, 'UW2':31, 
             'AW':32, 'DD2':33, 'GG3':34, 'VV':35, 'GG1':36, 'SH':37, 'ZH':38, 'RR2':39, 
             'FF':40, 'KK2':41, 'KK1':42, 'ZZ':43, 'NG':44, 'LL':45, 'WW':46, 'XR':47, 
             'WH':48, 'YY1':49, 'CH':50, 'ER1':51, 'ER2':52, 'OW':53, 'DH2':54, 'SS':55, 
             'NN2':56, 'HH2':57, 'OR':58, 'AR':59, 'YR':60, 'GG2':61, 'EL':62, 'BB2':63,
             "STOP": 0x7F };

NRLIPAtoSPO256 = { 'AA':'AA', 'AE':'AE', 'AH':'AX AX', 'AO':'AO', 'AW':'AW',  'AX':'AX',
                   'AY':'AY', 'b':'BB1', 'CH':'CH',  'd':'DD1', 'DH':'DH1', 'EH':'EH',
                   'ER':'ER1','EY':'EY', 'f':'FF',   'g':'GG2', 'h':'HH1',  'IH':'IH',
                   'IY':'IY', 'j':'JH',  'k':'KK1',  'l':'LL',  'm':'MM',   'n':'NN1',
                   'NG':'NG', 'OW':'OW', 'OY':'OY',  'p':'PP',  'r':'RR1',  's':'SS',
                   'SH':'SH', 't':'TT1', 'TH':'TH',  'UH':'UH',  'UW':'UW2','v':'VV',
                   'w':'WW', 'WH':'WH', 'y':'YY1', 'z':'ZZ', 'ZH':'ZH', 'PAUSE':'PA4'}

def translate(s):
    return NRLIPAtoSPO256[s]

print("/*")
print(" * 'dis file be autogenerated. Abandon all hope, all ye who edit here.")
print(" *")
print(" * Converted from retrospeak text-to-speech rules.")
print(" * Scott Baker, https://www.smbaker.com/")
print(" */")
print("")


print('#include "rules.h"')

print("")

alloReverse = {}

for (k,v) in allophones.items():
    if v!=0x7F:  # STOP is defined elsewhere
       print("#define %s %d" % (k, v))
    alloReverse[v] = k;

print("");

print("char *phones[] = {")
alloStr = []
for i in range(0,128):
    alloStr.append('  /* %03d */ "%s"' % (i, alloReverse.get(i,"")))
print(",\n".join(alloStr))
print("};")

print()

print("rule rules[] = {")

rulemap={}

line=0
for letter in sorted(Rules.keys()):
    group = Rules[letter]
    print("/* %s */" % letter);
    if len(letter)==1:
        rulemap[letter.lower()] = line
        rulemap[letter.upper()] = line
    elif letter=="punctuation":
        rulemap[" "]=line
        rulemap["-"]=line
        rulemap["."]=line
        rulemap[":"]=line
        rulemap["'"]=line
        rulemap[","]=line
        rulemap["?"]=line
        rulemap["!"]=line
        rulemap["@"]=line
    elif letter=="number":
        rulemap["0"]=line
        rulemap["1"]=line
        rulemap["2"]=line
        rulemap["3"]=line
        rulemap["4"]=line
        rulemap["5"]=line
        rulemap["6"]=line
        rulemap["7"]=line
        rulemap["8"]=line
        rulemap["9"]=line
    else:
        print("UNKOWN %c" % letter)
        sys.exit(-1)

    for rule in group:
        print("  /* %04d */ " % line, end="")
        print("{", end="")
        if rule[0] == Anything:
            print("ANYTHING", end="");
        elif rule[0] == Nothing:
            print("NOTHING", end="");
        else:
            print('"%s"' % rule[0], end="")
        
        print(', "%s"' % rule[1], end="")

        if rule[2] == Anything:
            print(", ANYTHING", end="");
        elif rule[2] == Nothing:
            print(", NOTHING", end="");
        else:
            print(', "%s"' % rule[2], end="")

        output=[]
        for part in rule[3].split():
            part = part.strip()
            sp_phones = translate(part)
            for part2 in sp_phones.split():
                part2 = part2.strip()
                output.append(part2)

        output.append("STOP")

        print(', {%s}},' % ", ".join(output))

        line += 1

    print("  /* %04d */ {NOMORE, NOMORE, NOMORE, {STOP}}," % line)
    line += 1

print("  /* %04d */ {NOMORE, NOMORE, NOMORE, {STOP}}};" % line)

print();

ruleline=[]
for i in range(0,127):
    c = chr(i)
    if c in rulemap:
        ruleline.append("  %d /* %c */" %(rulemap[c], c))
    else:
        ruleline.append("  -1")

print("int rulemap[128]={")
print("%s" % (",\n".join(ruleline)))
print("};")

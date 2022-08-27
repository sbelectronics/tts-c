# tts-c: text-to-speech in C
Scott Baker, https://www.smbaker.com/

This is a conversion of Jason Lane's [retroTTS](https://github.com/jbeuckm/retroSpeak) from python to C. An early dialect of K&R C is used, intended to be compatible with the CP/M-8000 distribution, for use in [Scott Baker's Z-8000 computer](https://github.com/sbelectronics/z8000). 

In addition to building on CP/M-8000, the code will also compile on Linux.

Phonemes output are for the SP0256A-AL2. For speed and size considerations, there is no intermediate translation step between IPA and SP0256A-AL2. The python program `atoc.py` handles the translation from the original retroTTS python rules into a C-based table format.

It should be easily portable to other C-based environments, for example would make a reasonable starting place to implement text-to-speech on an Arduino or a PIC.

The algorithm used is the traditional Naval Research Laboratories (NRL) Text-to-Speech algorithm that makes use of left and right context to determine which phoneme to output.  A general description, taken from retroTTS.py:

```
   Rules are made up of four parts:
   
           The left context.
           The text to match.
           The right context.
           The phonemes to substitute for the matched text.

   Procedure:
 
           Seperate each block of letters (apostrophes included) 
           and add a space on each side.  For each unmatched 
           letter in the word, look through the rules where the 
           text to match starts with the letter in the word.  If 
           the text to match is found and the right and left 
           context patterns also match, output the phonemes for 
           that rule and skip to the next unmatched letter.
 
   Special Context Symbols:
 
           #       One or more vowels
           :       Zero or more consonants
           ^       One consonant.
           .       One of B, D, V, G, J, L, M, N, R, W or Z (voiced 
                   consonants)
           %       One of ER, E, ES, ED, ING, ELY (a suffix)
                   (Found in right context only)
           +       One of E, I or Y (a "front" vowel)
```


# mfcd
Program to show the dump of a mifare classic 1k card and scan for value block pattern

## Compile
gcc mfcd.c -o mfcd -Wall -Wextra -pedantic

## Use
Use: ./mfcd -I file [-h] [-v] [-V verbose] [-a to not stamp ascii carachters] [-b scan for value block]

The verbose option also explains the notation

## Example of use
![Output example](/doc/Example.png)

## Future potential improvements
- Make the program compatible with 4k mifare

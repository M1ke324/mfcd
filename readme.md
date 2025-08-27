# mfcd
Program to show the dump of a mifare classic 1k card and scan for value block pattern

### Compile
```Bash
make
```

### Install
```Bash
sudo make install
```

### Uninstall
```Bash
sudo make uninstall
```

### Clean build files
```Bash
make clean
```

## Usage
```Bash
./mfcd -I file [-h] [-v] [-V] [-a] [-b]
```
*(if installed, you run it without `./`)*  

Options:  
- `-I file` → input dump file  
- `-h` → show help  
- `-v` → print version
- `-V` → verbose output (also explains the notation)    
- `-a` → do not print ASCII characters  
- `-b` → scan for value blocks

## Example
![Output example](/doc/Example.png)

## Documentation
If you need help and the program is installed, the manual page is available with:
```bash
man mfcd
```

## Future improvements
- Add support for MIFARE Classic 4K cards

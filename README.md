# Vox

## An emulation of the Diablo II Expansion client.

### Requirements
* POCO Library, minimum version 1.4.5. [Get it here](http://pocoproject.org/)
* Intermediate understanding of C++ and Networking.
* Diablo II Expansion binary files

```
# Create data/ in your binary directory
# Copy the following files

Game.exe
D2Client.dll
Bnclient.dll
```

### Makefile
```make
# Compiles binary into bin/
make

# Removes binary from bin/
make clean
```

### Usage
```
./vox [server] [account] [password] [classic_key] [expansion_key]
```

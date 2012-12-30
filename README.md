# Vox

## An emulation of the Diablo II Expansion client.

### Requirements
* POCO Library, minimum version 1.4.5. [Get it here](http://pocoproject.org/)
* Intermediate understanding of C++ and Networking.

### Makefile
```make
# Compiles binary into bin/ d
make vox

# Removes binary from bin/
make clean
```

### run.sh
Runs binary in bin/ directory
```sh
./vox "SERVER" "ACCOUNT" "PASSWORD" "CDKEY" "EXPANSION_KEY"

# Encase all fields in quotes ("")
```

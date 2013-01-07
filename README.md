# Mercury

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
./mercury [server] [realm] [account] [password] [character] [classic_key] [expansion_key] [exe_info] [key_owner]
# Servers, uswest.battle.net, useast.battle.net, europe.battle.net, asia.battle.net
# Sample exe_info "game.exe 10/19/11 19:48:14 65536"
# Key owner must be less then 16 characters
```


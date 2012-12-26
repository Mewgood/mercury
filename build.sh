cd src
g++ -Wall -Wextra -Wno-multichar -lPocoNet -o vox vox.cpp Buffer.cpp ByteReader.cpp TcpConnection.cpp ChatProtocol.cpp Bot.cpp
cd ..
mkdir -p bin
mv src/vox bin/vox

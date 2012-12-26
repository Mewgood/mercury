cd src
g++ -Wall -Wextra -Wno-multichar -lPocoNet -o d2-client d2-client.cpp Buffer.cpp ByteReader.cpp TcpConnection.cpp ChatProtocol.cpp Bot.cpp
cd ..
mkdir -p bin
mv src/d2-client bin/d2-client

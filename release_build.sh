cd src
g++ -Wall -Wextra -03 -lPocoNet -o d2-client d2-client.cpp Buffer.cpp ByteReader.cpp TcpConnection.cpp
cd ..
mkdir -p bin
mv src/d2-client bin/d2-client_rel

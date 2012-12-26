cd src
g++ -Wall -Wextra -03 -lPocoNet -o vox vox.cpp Buffer.cpp ByteReader.cpp TcpConnection.cpp
cd ..
mkdir -p bin
mv src/d2-client bin/d2-client_rel

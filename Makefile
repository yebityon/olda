BOOST_PATH = -I/usr/local/opt/boost/include
BYTECODE_PATH = -I~/Documents/Github/diffanalysis/OmniLogAnalyzer/src
OPENSSL_PATH = -I/usr/local/opt/openssl@1.1/include  
OPENSSL_LIB = -L/usr/local/opt/openssl@1.1/lib -lcrypto



SRC = ~/Documents/Github/olda

all: $(SRC)/main.cpp $(SRC)/graph/*.cpp

	g++-8 $(BOOST_PATH) $(BYTECODE_PATH) -c -std=c++17 -lstdc++fs $(SRC)/main.cpp 
	g++-8 $(BOOST_PATH) -c $(SRC)/graph/*.cpp
	g++-8 -o olda *.o -lstdc++fs
	rm -rf *.o

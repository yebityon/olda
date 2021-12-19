BOOST_PATH = -I/usr/local/opt/boost/include
BYTECODE_PATH = -I~/Documents/Github/diffanalysis/OmniLogAnalyzer/src
OPENSSL_PATH = -I/usr/local/opt/openssl@1.1/include  
OPENSSL_LIB = -L/usr/local/opt/openssl@1.1/lib -lcrypto



SRC = .

all: $(SRC)/main.cpp $(SRC)/graph/*.cpp $(SRC)/omni_graph/*.cpp

	g++-8 $(BOOST_PATH) -c -O2 -std=c++17 -lstdc++fs $(SRC)/main.cpp 
	g++-8 $(BOOST_PATH) -c -O2 -std=c++17 -lstdc++fs $(SRC)/omni_graph/*.cpp
	g++-8 $(BOOST_PATH) -c -O2 -std=c++17 -lstdc++fs $(SRC)/graph_diff/*.cpp
	g++-8 $(BOOST_PATH) -c -O2 -std=c++17 -lstdc++fs $(SRC)/graph/*.cpp
	g++-8 $(BOOST_PATH) -c -O2 -std=c++17 -lstdc++fs $(SRC)/writer/*.cpp
	g++-8 -o olda *.o -lstdc++fs

diff : $(SRC)/graph_diff/*.cpp
	g++-8 $(BOOST_PATH) -c -O2 -std=c++17 -lstdc++fs $(SRC)/graph_diff/*.cpp
	g++-8 -o olda *.o -lstdc++fs
	

parser_test: $(SRC)/test/*.cpp 
	g++-8 $(BOOST_PATH) -c -std=c++17 -lstdc++fs $(SRC)/graph/parser_util.cpp
	g++-8 $(BOOST_PATH) -c -std=c++17 -lstdc++fs $(SRC)/graph/method_entry.cpp
	g++-8 $(BOOST_PATH) -c -std=c++17 -lstdc++fs $(SRC)/omni_graph/graph_util.cpp
	g++-8 $(BOOST_PATH) -c -std=c++17 -lstdc++fs $(SRC)/test/test_method_entry.cpp 
	g++-8 -o target/test_method_entry *.o -lstdc++fs
	rm -rf  test_method_entry.o
	g++-8 $(BOOST_PATH) -c -std=c++17 -lstdc++fs $(SRC)/test/test_method_param.cpp 
	g++-8 -o target/test_method_param *.o -lstdc++fs
	rm -rf test_method_param.o
	g++-8 $(BOOST_PATH) -c -std=c++17 -lstdc++fs $(SRC)/test/test_method_exit.cpp 
	g++-8 -o target/test_method_exit *.o -lstdc++fs
	rm -rf test_method_exit.o
	
clean:
	rm -rf *.o
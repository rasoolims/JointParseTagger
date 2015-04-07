CXX=g++
CFLAGS=-O3 -I/usr/lib/jvm/java-8-oracle/include/ -I/usr/lib/jvm/java-8-oracle/include/linux/ -L/usr/bin/java -L/usr/lib/jvm/java-8-oracle/jre/lib/amd64/server/ -ljvm -std=c++11 -Wl,-rpath,/usr/lib/jvm/java-8-oracle/jre/lib/amd64/server/

JC=javac

all: java parse_test

parse_test: parse_test.cpp parse.o
	$(CXX) $^ -o parse_test $(CFLAGS)  

parse.o: parse.cpp parse.h
	$(CXX) -c parse.cpp $(CFLAGS)

java: ../src/TagAndParse.java
	$(JC) -cp ../jar/JointParseTagger.jar ../src/TagAndParse.java 

clean:
	rm -f *~ *.o parse_test

CXX=g++
CFLAGS=-std=c++11 
DEPS=buffer.hpp
LIBS=-lrt -lpthread

all: publisher subscriber

%: %.cpp $(DEPS)
	$(CXX) -o $@ $< $(CFLAGS) $(LIBS)

clean:
	rm -f publisher subscriber delay*.txt

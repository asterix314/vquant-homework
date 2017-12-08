CXX=clang++
CFLAGS=-std=c++11 -O2
DEPS=buffer.hpp
LIBS=-lrt -lpthread -latomic

all: publisher subscriber

%: %.cpp $(DEPS)
	$(CXX) -o $@ $< $(CFLAGS) $(LIBS)

clean:
	rm -f publisher subscriber delay*.txt

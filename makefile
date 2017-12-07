all: publisher subscriber

publisher: publisher.cpp buffer.hpp
	clang++ -std=c++11 -lpthread -lrt publisher.cpp -o publisher

subscriber: subscriber.cpp buffer.hpp
	clang++ -std=c++11 -lpthread -lrt subscriber.cpp -o subscriber

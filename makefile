all: publisher subscriber

publisher: publisher.cpp buffer.hpp
	clang++ -std=c++11 -O2 publisher.cpp -o publisher

subscriber: subscriber.cpp buffer.hpp
	clang++ -std=c++11 -O2 subscriber.cpp -o subscriber

all: publisher subscriber

publisher: publisher.cpp buffer.hpp
	clang++ -std=c++11 publisher.cpp -o publisher

subscriber: subscriber.cpp buffer.hpp
	clang++ -std=c++11 subscriber.cpp -o subscriber

all: publisher subscriber

publisher: publisher.cpp buffer.hpp
	clang++ -std=c++14 publisher.cpp -o publisher

subscriber: subscriber.cpp buffer.hpp
	clang++ -std=c++14 subscriber.cpp -o subscriber

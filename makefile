all: publisher subscriber

publisher: publisher.cpp homework.hpp
	clang++ -std=c++14 publisher.cpp -o publisher

subscriber: subscriber.cpp homework.hpp
	clang++ -std=c++14 subscriber.cpp -o subscriber

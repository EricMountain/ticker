all: ticker

ticker: ticker.cpp
	g++ -static -Wall -Werror -O3 -o ticker ticker.cpp

.PHONY: clean
clean:
	rm ticker

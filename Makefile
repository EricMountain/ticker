all: ticker

ticker: ticker.cpp
	g++ -Wall -Werror -O3 -o ticker ticker.cpp

.PHONY: clean
clean:
	rm ticker

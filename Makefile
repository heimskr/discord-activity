COMPILER ?= g++
CXXFLAGS := -g -Og -Wall -Wextra -std=c++23 -Iinclude -Isrc/discord
LDFLAGS  := -Llib/$(shell uname -m) -ldiscord_game_sdk
SOURCES  := $(shell find src -name '*.cpp')
OBJECTS  := $(SOURCES:.cpp=.o)

main: $(OBJECTS)
	$(COMPILER) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(COMPILER) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(shell find src -name '*.o') ./main

test: main
	./main
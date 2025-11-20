all:test_runner_exe

clean:
	rm testrunner.exe *.o ../source/*.o

run:
	./testrunner.exe


CC=clang++

WARNINGS=-Wall

INCLUDES=-I. -I../source

# NOTE using draft C++23 for now
CFLAGS=-c $(WARNINGS) $(INCLUDES) -std=c++2b -g -O0

LDFLAGS=

SOURCES=$(filter-out ../source/main.cpp, $(wildcard ../source/*.cpp) $(wildcard *.cpp))

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=testrunner.exe

test_runner_exe: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


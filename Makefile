CC=g++
CXXFLAGS=-std=c++17
CFLAGS=-fpermissive
target=go2mazing

SRCS=$(wildcard *.cpp)
SRCS+=$(wildcard *.c)
SRCS+=$(wildcard ./lib/ugui/*.c)
SRCS+=$(wildcard ./wnd/*.cpp)
OBJS_cpp= $(patsubst %cpp,%o,$(SRCS))
OBJS= $(patsubst %c,%o,$(OBJS_cpp))

INCLUDE = -I/usr/local/include/
LIB = -L/usr/local/lib/ -lgo2 -lopenal -lpng

all:$(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(target) $(LIB)

%.o :%.c %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@  $(INCLUDE)

clean:
	rm $(OBJS) $(target) -f

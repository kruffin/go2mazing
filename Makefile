CC=g++
CXXFLAGS=-std=c++17
CFLAGS=-fpermissive
target=go2mazing

SRCS=$(wildcard *.cpp)
SRCS+=$(wildcard *.c)
SRCS+=$(wildcard ./lib/ugui/*.c)
SRCS+=$(wildcard ./wnd/*.cpp)
SRCS+=$(wildcard ./lib/*.c)
OBJS_cpp= $(patsubst %cpp,%o,$(SRCS))
OBJS= $(patsubst %c,%o,$(OBJS_cpp))

INCLUDE = -I/usr/local/include/
LIB = -L/usr/local/lib/ -lopenal -lpng

go2: LIB+= -lgo2
glfw: LIB+= -lglfw -lrt -lm -ldl -lX11 -lpthread -lXrandr -lGL
glfw: CPPFLAGS=-DUSE_GLFW

go2: all
glfw: all

all:$(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(target) $(LIB)

%.o :%.c %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@  $(INCLUDE)

clean:
	rm $(OBJS) $(target) -f

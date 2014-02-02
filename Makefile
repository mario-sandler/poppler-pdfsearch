CXXFLAGS = `pkg-config --cflags poppler-cpp poppler`
LDFLAGS = `pkg-config --libs poppler-cpp poppler`
CXX = clang++
CC = clang++

OBJS = pdfsearch.o parseargs.o

all: pdfsearch

pdfsearch: $(OBJS)

clean:
	rm -vf $(OBJS) pdfsearch

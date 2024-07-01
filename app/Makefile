srcfiles := $(shell find . -name "*.cpp")
OBJS  := $(patsubst %.cpp, %.o, $(srcfiles))
$(info SRC: $(srcfiles))
$(info OBJ: $(OBJS))
CXX = g++
CXXFLAGS = -I. -Wall -O3 -ggdb -pthread  -lpthread

all: clustering
%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

clustering:	$(OBJS)
		$(CXX) -pthread  -lpthread -o $@ $(OBJS)

clean:
	rm -f *.o
	rm -f clustering


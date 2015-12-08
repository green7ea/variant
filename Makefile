CXX = clang++
CXXFLAGS = -MD -pipe -Wall -Wextra -std=c++11 -g
LDFLAGS =
SRCS = test.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
APP = test

.PHONY: all clean

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(APP) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(DEPS) $(APP)

-include $(DEPS)

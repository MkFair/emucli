SOURSES = $(wildcard *.cpp)
OBJFILES = $(patsubst %.cpp,%.o,$(SOURSES))

%.o: %.cpp
	g++ --std=c++17 -c $< -o $(<:%.cpp=%.o)
all: $(OBJFILES)
	g++ -pthread --std=c++17 *.o 

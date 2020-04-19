CXX=g++
CXXFLAGS=-Wall -Werror -std=c++11 -DNDEBUG -O3

all: tiles

tiles: *.cc *.hpp
	$(CXX) $(CXXFLAGS) *.cc *.hpp -o tiles

clean:
	rm -fr *.o tiles tiles_solver

CXX	:= g++

p3 : main.cpp
	$(CXX) -x -o $@ $<

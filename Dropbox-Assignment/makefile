CXXFLAGS = -std=c++17

all: mfs filesystem
	$(CXX) $(CXXFLAGS) mfs.o filesystem.o
	./a.out
mfs: mfs.cpp filesystem.h
	$(CXX) $(CXXFLAGS) -c mfs.cpp
filesystem: filesystem.h filesystem.cpp
	$(CXX) $(CXXFLAGS) -c filesystem.cpp

clean:
	rm -f *.o a.out

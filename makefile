CXXFLAGS=-g -std=c++14 -Wall -Wno-missing-braces -D_DEBUG -I ./boost_1_64_0
LDFLAGS=

objects=main.o countertrie.o compress.o

shrt: $(objects)
	$(CXX) -o shrt $(objects) $(LDFLAGS)

main.o: main.cpp FRC.h countertrie.h compress.h

countertrie.o: countertrie.h

compress.o: compress.h FRC.h

clean:
	rm $(objects) shrt

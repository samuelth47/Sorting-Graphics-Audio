
CC	= g++
CFLAGS	= -Wall -ansi -pedantic -std=c++11 -I/usr/include/freetype2/
LFLAGS	= -L/usr/lib -fopenmp -ltsal -ltsgl -lfreetype -lGLEW -lglfw -lGLU  -lGL
PROGS	= ../bubble_sort ../insertion_sort ../merge_sort ../quick_sort

all: $(PROGS)

../merge_sort: merge_sort.cpp sortUtilities.h
	$(CC) $(CFLAGS) merge_sort.cpp -o ../merge_sort $(LFLAGS) 

../quick_sort: quick_sort.cpp sortUtilities.h
	$(CC) $(CFLAGS) quick_sort.cpp -o ../quick_sort $(LFLAGS) 

../insertion_sort: insertion_sort.cpp sortUtilities.h
	$(CC) $(CFLAGS) insertion_sort.cpp -o ../insertion_sort $(LFLAGS) 

../bubble_sort: bubble_sort.cpp sortUtilities.h
	$(CC) $(CFLAGS) bubble_sort.cpp -o ../bubble_sort $(LFLAGS) 

clean:
	rm -f $(PROGS) 

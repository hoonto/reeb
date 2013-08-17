all:
	g++ -O3 -I ../include -I ../deps/lemon/include -fpic -c pydenali.cpp ../src/*.cpp
	g++ -O3 -shared *.o -L ../deps/lemon/lib -lemon -o pydenali.so

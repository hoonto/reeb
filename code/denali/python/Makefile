all:
	g++ -I ../include -I ../deps/lemon/include -fpic -c pydenali.cpp ../src/*.cpp
	g++ -shared *.o -L ../deps/lemon/lib -lemon -o pydenali.so

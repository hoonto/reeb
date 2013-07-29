all:
	g++ pytourtre.cpp -fpic -c -I /home/eldridge/workbench/reeb/lib/libtourtre/include/
	g++ -shared pytourtre.o /home/eldridge/workbench/reeb/lib/libtourtre/libtourtre.a -o pytourtre.so

clean:
	rm *.so *.o

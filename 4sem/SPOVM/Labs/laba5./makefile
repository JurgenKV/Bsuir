Lab: laba5wr.c laba5.c
	gcc -fpic -c laba5wr.c
	gcc -shared laba5wr.o -lrt -o laba5wra.so
	gcc laba5.c -c
	gcc laba5.o -ldl -lpthread -L. -o laba5
	clear
	./laba5

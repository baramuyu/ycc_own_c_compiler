ycc: ycc.c

test: ycc
	./ycc -test
	./test.sh

clean:
	rm -f ycc *.o *~ tmp*
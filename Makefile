all:
	make -C lib
	make -C cat

clean:
	make -C lib clean
	make -C cat clean

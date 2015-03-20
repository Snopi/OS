all:
	make -C lib
	make -C cat
	make -C revwords
	make -C delwords

clean:
	make -C lib clean
	make -C cat clean
	make -C revwords clean
	make -C delwords clean

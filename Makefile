all:
	make -C lib
	make -C cat
	make -C revwords
	make -C filter

clean:
	make -C lib clean
	make -C cat clean
	make -C revwords clean    
	make -C filter clean  

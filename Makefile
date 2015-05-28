all:
	make -C lib
	make -C cat
	make -C revwords
	make -C filter
	make -C bufcat
	make -C simplesh
clean:
	make -C lib clean
	make -C cat clean
	make -C revwords clean    
	make -C filter clean  
	make -C bufcat clean
	make -C simplesh clean

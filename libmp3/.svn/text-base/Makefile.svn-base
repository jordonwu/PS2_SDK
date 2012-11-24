test: example
	ps2client execee host:bin/mp3play.elf example/test.mp3 
	
example: libmp3
	mkdir -p bin
	cd example ; make

libmp3: libmad libid3tag
	mkdir -p ee/lib
	cd ee/src ; make

libmad: 
	cd libmad ; make

libid3tag: 
	cd libid3tag ; make

hex: main.c
	sdcc main.c
	
all: hex
	packihx main.ihx > main.hex
	
clean:
	git clean -dfx
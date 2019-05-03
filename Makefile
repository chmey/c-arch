all: carch
carch: carch.c
	gcc -o carch carch.c -Wall
test: carch
	rm archive1
	./carch pack archive1 archiveme.txt
	ls -l
	hexdump archive1
clean:
	rm carch

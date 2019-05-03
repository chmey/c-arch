all: carch
carch: carch.c
	gcc -o carch carch.c -Wall
test: carch
	-rm archive1
	echo "Hello, World" > archiveme.txt
	echo "Czesz" > archiveme2.txt
	./carch pack archive1 archiveme.txt archiveme2.txt
	./carch list archive1
	hexdump archive1
	./carch pack archive1 archiveme.txt
	./carch list archive1
	hexdump archive1
clean:
	rm carch

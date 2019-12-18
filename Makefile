# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all:
	platformio -f -c vim run

upload:
	platformio -f -c vim run --target upload

clean:
	platformio -f -c vim run --target clean

program:
	platformio -f -c vim run --target program

uploadfs:
	platformio -f -c vim run --target uploadfs

update:
	platformio -f -c vim update

proto:
	cd .. && ./tools/nanopb-0.3.9-macosx-x86/generator-bin/protoc --nanopb_out=. ./proto/*.proto
	cp ../proto/*.c src/
	cp ../proto/*.h src/

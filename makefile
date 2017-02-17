all: bin shell ls rm touch

shell: shell.c
	gcc shell.c -o shell

bin:
	mkdir -p bin/

ls: src/ls.c
	gcc src/ls.c -o bin/ls -std=c99

rm: src/mkdir.c
	gcc src/mkdir.c -o bin/mkdir -std=c99

touch: src/touch.c
	gcc src/touch.c -o bin/touch -std=c99
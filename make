shell: shell.c
  gcc shell.c -o shell -std=c99

ls: src/ls.c
  gcc src/ls.c -o bin/ls -std=c99

rm: src/rm
  gcc src/rm.c -o bin/rm -std=c99

touch: src/touch
  gcc src/touch.c -o bin/touch -std=c99
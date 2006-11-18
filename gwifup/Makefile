clean:
	-rm *~

wifup:
	gcc -o ../wifup wifup.c
	chmod 4755 ../wifup

gwifup:
	gcc -o ../gwifup gwifup.c eggtrayicon.c `pkg-config --cflags --libs gtk+-2.0`
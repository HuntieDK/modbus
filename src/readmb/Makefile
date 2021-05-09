total: readmb

readmb: readmb.o
	gcc -o readmb readmb.o -lmodbus

install: readmb
	cp readmb /usr/local/bin
	chown root.root /usr/local/bin/readmb
	chmod 4755 /usr/local/bin/readmb

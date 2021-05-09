total: readmb

readmb: readmb.o
	gcc -o readmb readmb.o -lmodbus

install: readmb
	sudo cp readmb /usr/local/bin
	sudo chown root.root /usr/local/bin/readmb
	sudo chmod 4755 /usr/local/bin/readmb

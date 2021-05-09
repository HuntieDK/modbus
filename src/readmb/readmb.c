/*
 * modbus.c - a very small MODBUS register query application
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <modbus/modbus.h>

static void pr_div(int32_t value, int div)
{
	char str[32];
	int dec = 0;
	int div10 = div;
	int fac10 = 1;
	char *p;
	while (div10 >= 10) {
		++dec;
		div10 /= 10;
		fac10 *= 10;
	}
	sprintf(str, "%20lld", (int64_t)value * fac10 / div);
	if (dec > 0 && dec <= 16) {
		memmove(str, str+1, 19-dec);
		p = str+19-dec;
		p[0] = '.';
		if (p[-1] == ' ') p[-1] = '0';
		while (p[1] == ' ') *++p = '0';
	}
	p=str;
	while (*p == ' ') ++p;
	printf("%s ", p);
}

static void pr_reg(modbus_t *ctx, char type, const char* addr_str)
{
	int16_t reg[2];
	int addr, cnt;
	char* end;
	int div = 1;
	addr = strtol(addr_str, &end, 0);
	if (end != NULL && *end == '/') {
		div = strtol(end+1, NULL, 10);
		if (div <= 0) {
			div = 1;
		}
	}
	cnt = (type == 's') ? 1 : 2;
	if (modbus_read_registers(ctx, addr, cnt, reg) < 0) {
		fprintf(stderr,"Read register %d failed: %s\n", addr, modbus_strerror(errno));
		fputs("? ", stdout);
		return;
	}
	switch (type) {
	case 's':
		pr_div(reg[0], div);
		break;
	case 'i':
		pr_div(reg[0]+(((int32_t)reg[1])<<16), div);
		break;
	case 'f':
		printf("%f ", *(float*)&reg[0]);
		break;
	}
}

static void usage()
{
	fprintf(stderr, "usage: mb-read [-a addr] reg ...\n");
	exit(1);
}

int main(int argc, char* argv[])
{
	modbus_t *ctx;
	int i;
	int addr = 1;
	int baud = 9600;
	int bits = 8;
	int stop = 1;
	char par = 'N';

	int opt;
	while ((opt = getopt(argc, argv, "a:")) != -1) {
		switch (opt) {
		case 'a':	addr = atoi(optarg);	break;
		default:	usage();
		}
	}

	// connect to server
	ctx = modbus_new_rtu("/dev/ttyS0", baud, par, bits, stop); 
	if (modbus_connect(ctx) == -1) {
		fprintf(stderr,"Connection failed: %s\n",modbus_strerror(errno));
		modbus_free(ctx);
		exit(101);
	}
	// modbus_set_debug(ctx, TRUE);
	modbus_set_byte_timeout(ctx, 1, 0);
	modbus_set_response_timeout(ctx, 4, 0);
	modbus_set_slave(ctx, addr);

	int reg;
	int16_t val[2];
	for (i=optind; i<argc; i++) {
		switch (argv[i][0]) {
		case 's':	/* short (16) */
		case 'i':	/* int (32) */
		case 'f':	/* float (32) */
			pr_reg(ctx, argv[i][0], argv[i]+1);
			break;
		default:
			fprintf(stderr, "Register type %c not supported\n", argv[i][0]);
			break;
		}
	}
	puts("");

	modbus_close(ctx);
	modbus_free(ctx);

	exit(0);
}

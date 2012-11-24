#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


int main(int argc, char *argv[]) {
	struct stat buf;
	char filename[256];
	FILE *src;
	FILE *dst;
	unsigned char d;

	if (argc < 3) {
		printf("usage: %s <source file> <name>\n", argv[0]);
		return 1;
	}

	src = fopen(argv[1], "rb");
	if (src == NULL) {
		printf("error opening %s\n", argv[1]);
		return 1;
	}
	sprintf(filename, "%s.c", argv[2]);
	dst = fopen(filename, "wb");
	if (dst == NULL || stat(argv[1], &buf) == -1) {
		printf("error opening %s\n", filename);
		return 1;
	}

	fprintf(dst, "\n");
	fprintf(dst, "unsigned char %s[%d] = {\n", argv[2], buf.st_size);
	for (;;) {
		fread(&d, 1, 1, src);
		if (feof(src)) break;
		fprintf(dst, "\t0x%x,\n", d);
	}
	fprintf(dst, "};\n");

	fclose(dst);

	sprintf(filename, "%s.h", argv[2]);
	dst = fopen(filename, "wb");
	if (dst == NULL) {
		printf("error opening %s\n", filename);
		return 1;
	}

	fprintf(dst, "\n");
	fprintf(dst, "unsigned char %s[%d];\n", argv[2], buf.st_size);

	fclose(dst);
	fclose(src);

	return 0;
}


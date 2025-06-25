#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct capacity {
	size_t version;
	char EC_level[8];
	size_t alphanum_capacity;
	size_t byte_capacity;
} capacity;

capacity* readCapacityTable(char* filename);

int main(void) {
	// how do you find the minm version of qrcode needed to encode the given data?
	// we'll be only using alphanumeric for now
	// ./qrcode --encode "hello123" -Ec L
	// here we are using error correction level - low
	capacity* cap = readCapacityTable("qr_capacity_table.csv");
	if (cap == NULL) return 1;
	for (int i = 0; cap[i].version != 0; i++) {
		printf("Version: %ld\n", cap[i].version);
		printf("EC Level: %s\n", cap[i].EC_level);
		printf("Alphanum capacity: %ld\n", cap[i].alphanum_capacity);
		printf("Byte capacity: %ld\n", cap[i].byte_capacity);
	}
	free(cap);
	return 0;
}

capacity* readCapacityTable(char* filename) {
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error while opening the file\n");
		return NULL;
	}
	char header[64]; // long enough for storing the header.
	fgets(header, sizeof(header), fp);
	// count the number of fields from header
	int count_comma = 0;
	for (int i = 0; header[i] != '\0'; i++) {
		if (header[i] == ',') count_comma++;
	}
	int fields = count_comma + 1;
	char line[20]; // long enough for one line in our csv file
	capacity* cap = malloc(170 * sizeof(capacity));
	char csv_data[fields][8]; // array of fields for each line
	int j, k;
	int c = 0;
	while (fgets(line, sizeof(line), fp) != NULL) {
		j = 0, k = 0;
		for (int i = 0; line[i] != '\0'; i++) {
			if (line[i] == ',' || line[i+1] == '\0') {
				csv_data[j][k] = '\0';
				j++;
				k = 0;
			}	else {
				csv_data[j][k] = line[i];
				k++;
			}
		}
		// for (int i = 0; i < 4; i++) printf("%s\n", csv_data[i]);
		cap[c].version = atoi(csv_data[0]);
		strcpy(cap[c].EC_level, csv_data[1]);
		cap[c].alphanum_capacity = atoi(csv_data[2]);
		cap[c].byte_capacity = atoi(csv_data[3]);
		c++;
	}
	fclose(fp);
	return cap;
}
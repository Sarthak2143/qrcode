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
int textType(char* text);
int validEC(char* text);

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Usage: %s text EC_level\n", argv[0]);
		return 1;
	}
	// how do you find the minm version of qrcode needed to encode the given data?
	// we'll be only using alphanumeric for now
	// ./qrcode --encode "hello123" -Ec L
	// here we are using error correction level - low
	char* text = argv[1];
	size_t length = strlen(text);
	char* ec_level = argv[2];
	int char_type = textType(text);
	if (validEC(ec_level) == 0) return 0;
	printf("Text type: %s\n", (char_type == 0) ? "Alphanumeric": "Byte");
	printf("Text: %s\n", text);
	printf("EC_level: %s\n", ec_level);
	capacity* cap = readCapacityTable("qr_capacity_table.csv");
	if (cap == NULL) return 1;

	size_t version;
	for (int i = 0; cap[i].version != 0; i++) {
		if (strcmp(cap[i].EC_level, ec_level) == 0) {
			// we need to find the first occurence of a number greater than the strlen
			if (char_type == 0) {
				if (cap[i].alphanum_capacity > length) {
					version = cap[i].version;
					break;
				}
			} else {
				if (cap[i].byte_capacity > length) {
					version = cap[i].version;
					break;
				}
			}
		}
	}
	printf("Minimum version needed: %zu\n", version);
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

int textType(char* text) {
	// return 1 for byte else 0 for alphanum
	for (int i = 0; text[i] != '\0'; i++) {
		if (!isalnum(text[i])) return 1;
	}
	return 0;
}

int validEC(char* text) {
	if (strlen(text) != 1) return 0;
	// check if the text is valid ec level and capitalize too
	text[0] = toupper(text[0]);
	if (strcmp(text, "L") != 0 && strcmp(text, "M") != 0 && 
			strcmp(text, "Q") != 0 && strcmp(text, "H") != 0) {
		printf("Invalid EC_level\n");
		return 0;
	}
	else return 1;
}
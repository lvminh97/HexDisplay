#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#define HEX_OK					0
#define HEX_ERROR_OPEN_FILE 	1
#define HEX_ERROR_WRONG_FORMAT 	2

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

uint8_t buffer[65536];

void initBuffer();
uint8_t readFromFile(char *filename);
uint8_t hexToDec(char hex);
uint8_t checkValidRecord(uint8_t *record, uint8_t len);
void writeToBuffer(uint8_t *record, uint8_t len);
void printBuffer(uint16_t page);
uint16_t getKeyPressed(void);

int main(int argc, char* argv[]){
	uint16_t page;
	initBuffer();
	readFromFile(argv[1]);
	page = 0;
	printBuffer(page);
	while(1){
		uint16_t key = getKeyPressed();
		if(key == 0xe04b && page > 0) {
			page--;
			printBuffer(page);
		}
		else if(key == 0xe04d && page < 163) {
			page++;
			printBuffer(page);
		}
		else if(key == 0x1b00){
			system("cls");
			break;
		}
	}
	return 0;
}

void initBuffer(){
	uint32_t i;
	for(i = 0; i < 65536; i++) buffer[i] = 0xff;
}

uint8_t readFromFile(char *filename){
	FILE *file = fopen(filename, "r");
	uint8_t c, record[50];
	int recordPos, buffPos, start, i;
	if(file == NULL){
		return HEX_ERROR_OPEN_FILE;
	}
	start = 0;
	recordPos = 0;
	buffPos = 0;
	while((c = fgetc(file)) != EOF){
		if(start == 0){
			if(c != ':') return HEX_ERROR_WRONG_FORMAT;
			start = 1; // begin of a record
		}
		else{
			if(c == '\n'){
				if(start == 0) return HEX_ERROR_WRONG_FORMAT;
				int valid = checkValidRecord(record, recordPos);
				if(valid != 0) return valid;
				else writeToBuffer(record, recordPos);
				recordPos = 0;
				start = 0;
			}
			else {
				if(hexToDec(c) == -1) return HEX_ERROR_WRONG_FORMAT;
				record[recordPos] = hexToDec(c);
				c = fgetc(file);
				if(hexToDec(c) == -1) return HEX_ERROR_WRONG_FORMAT;
				record[recordPos] = record[recordPos] * 16 + hexToDec(c);
				recordPos++;	
			}
		}
	}
	fclose(file);
	return HEX_OK;
}

uint8_t hexToDec(char hex){
	if('0' <= hex && hex <= '9') return hex - '0';
	else if('A' <= hex && hex <= 'F') return hex - 'A' + 10;
	else if('a' <= hex && hex <= 'f') return hex - 'a' + 10;
	else return -1; //invalid hex symbol
}

uint8_t checkValidRecord(uint8_t *record, uint8_t len){
	if(len < 5) return 2;
	uint8_t numBytes = record[0];
	
	if(numBytes > 16 || numBytes + 5 != len) return 2;

	uint16_t address  = (record[1] << 8) | record[2];
	uint8_t type = record[3];
	if(type != 0x00 && type != 0x01 && type != 0x02 && type != 0x04) return 2;

	if(type != 0x00 && address != 0x0000) return 2;

	uint8_t checksum = record[len - 1];
	uint8_t i, sum = 0;
	for(i = 0; i < len - 1; i++) sum += record[i];
	sum = ~sum;
	if(sum + 0x01 != checksum) return 2;
	
	return 0;		
}

void writeToBuffer(uint8_t *record, uint8_t len){
	uint8_t numBytes = record[0];
	uint16_t address = (record[1] << 8) | record[2];
	if(record[3] != 0x00) return;
	uint32_t i;
	for(i = 0; i < numBytes; i++){
		buffer[address + i] = record[i + 4];
	}
}

void printBuffer(uint16_t page){
	system("cls");
	uint16_t address = page * 400;
	uint8_t i, j;
	printf("Address  | Memory                                          | ASCII \n");
	printf("---------|-------------------------------------------------|-----------------------\n");
	for(i = 0; i < 25; i++){
		printf("%08X | ", address);
		for(j = 0; j < 16; j++){
			printf("%02X ", buffer[address + j]);
		}
		printf("| \n");
		if(address == 0xfff0) break;
		address += 16;
	}
}

uint16_t getKeyPressed(void){
	uint16_t c;
	c = getch() << 8;
	if(c == 0xe000) c |= getch();
	return c;
}

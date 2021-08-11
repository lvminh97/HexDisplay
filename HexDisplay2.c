#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

unsigned char buff[65536];
unsigned int extendAddr = 0, maxAddr= 0xFFFF;
int end = 0;

void process(int line, unsigned char *row, int length){
    int address, sum;
    if(length < 5){
        printf("Dong %d sai dinh dang cua Intel HEX.", line);
        exit(1);
    }
    if(row[0] > 16 || row[0] != length - 5){
        printf("Dong %d sai dinh dang cua Intel HEX. %d %d", line, row[0], length);
        exit(1);
    }
    // checksum
    sum = 0;
    for(int i = 0; i < length - 1; i++)
        sum = sum + row[i];
    sum = ~sum;
    if(((sum + 1) & 0xFF) != row[length - 1]){
        printf("Dong %d sai dinh dang cua Intel HEX.", line);
        exit(1);
    }
    address = (row[1] << 8) + row[2];
    // record type
    if(row[3] == 0){
        address = address + extendAddr;
        if(address <= 0xFFFF){
            for(int i = 0; i < row[0]; i++)
                buff[address + i] = row[i + 4];
            maxAddr = address;
        }
    }
    else if(row[3] == 1){
        if(address != 0 || row[0] > 0){
            printf("Dong %d sai dinh dang cua Intel HEX.", line);
            exit(1);
        }
        else
            end = 1;
    }
    else if(row[3] == 2){
        if(address != 0 || row[0] != 2){
            printf("Dong %d sai dinh dang cua Intel HEX.", line);
            exit(1);
        }
        else
            extendAddr = ((row[4] << 4) + row[5]) << 4;
    }
    else if(row[3] == 4){
        if(address != 0 || row[0] != 2){
            printf("Dong %d sai dinh dang cua Intel HEX.", line);
            exit(1);
        }
        else
            extendAddr = ((row[4] << 4) + row[5]) << 16;
    }
    else{
        printf("Dong %d sai dinh dang cua Intel HEX.", line);
        exit(1);
    }
}

void readFromFile(char *file){
    FILE *inp = fopen(file, "r");
    unsigned char row[50];
    int line = 1, begin = 0, rowId = 0, buffId = 0, byteCplt = 0;
    char tmp;
    if(inp == NULL){
        printf("Khong the mo file %s", file);
        exit(1);
    }
    while(EOF != (tmp = fgetc(inp))){
        if(end){
            printf("Dong %d sai dinh dang cua Intel HEX.", line);
            exit(1);
        }
        if(!begin){
            if(tmp == ':')
                begin = 1;
            else{
                printf("Dong %d sai dinh dang cua Intel HEX.", line);
                exit(1);
            }
        }
        else{
            if(tmp == '\n'){
                if(!begin){
                    printf("Dong %d sai dinh dang cua Intel HEX.", line);
                    exit(1);
                }
                process(line, row, rowId);
                rowId = begin = 0;
                line = line + 1;
            }
            else{
                int value;
                if(tmp >= '0' && tmp <= '9')
                    value = tmp - 48;
                else if(tmp >= 'a' && tmp <= 'f')
                    value = tmp - 87;
                else if(tmp >= 'A' && tmp <= 'F')
                    value = tmp - 55;
                else
                    value = -1;
                if(value == -1){
                    printf("Dong %d sai dinh dang cua Intel HEX.", line);
                    exit(1);
                }
                else{
                    if(!byteCplt){
                        row[rowId] = value;
                        byteCplt = 1;
                    }
                    else{
                        row[rowId] = (row[rowId] << 4) + value;
                        byteCplt = 0;
                        rowId = rowId + 1;
                    }
                }
            }
        }
    }
    fclose(inp);
}

void print(int part){
    system("cls");
    int addr = 400 * part;
    unsigned char tmp;
    printf("Dia chi    | Du lieu                                         | Ky tu ASCII \n");
	printf("-----------|-------------------------------------------------|-------------------\n");
    for(int i = 0; i < 25; i++){
        if(addr < maxAddr){
            printf("%08X   | ", addr);
            for(int j = 0; j < 16; j++){
                printf("%02X ", buff[addr + j]);
            }
            printf("| ");
            for(int j = 0; j < 16; j++){
                tmp = buff[addr + j];
                if(tmp >= 32)
                    printf("%c", tmp);
                else
                    printf(".");
            }
            addr = addr + 16;
        }
        printf("\n");
    }
    printf("Ban dang xem part %d/%d\n", part + 1, (maxAddr / 400) + 1);
    printf("Nhan 'a' de lui, nhan 's' de tien, nhan 'c' de thoat");
}

int main(int argc, char* argv[]){
	for(int i = 0; i < 65536; i++){
        buff[i] = 0xFF;
    }
	readFromFile(argv[1]);
	int part = 0;
	print(part);
	while(1){
		char keycode = getch();
		if(part > 0 && (keycode == 'a' || keycode == 'A')) {
			part = part - 1;
			print(part);
		}
		else if(part < maxAddr / 400 && (keycode == 's' || keycode == 'S')) {
			part = part + 1;
			print(part);
		}
        else if(keycode == 'c' || keycode == 'C'){
            exit(1);
        }
	}
	return 0;
}
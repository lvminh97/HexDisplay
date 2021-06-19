#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#define HEX_OK					0
#define HEX_ERROR_OPEN_FILE 	1
#define HEX_ERROR_WRONG_FORMAT 	2

// Mã phím
#define LEFT_ARROW_KEY		0xe04b // phím mũi tên trái
#define RIGHT_ARROW_KEY		0xe04d // phím mũi tên phải
#define ESC_KEY				0x1b00 // phím Esc

// Định nghĩa kiểu dữ liệu không dấu
typedef unsigned char		uint8_t;	// 8 bit
typedef unsigned short		uint16_t;	// 16 bit
typedef unsigned int		uint32_t;	// 32 bit

uint8_t buffer[65536];
uint32_t line, extendAddress = 0x0000, maxAddress = 0xffff;
uint8_t isEnd = 0;

// Nguyên mẫu hàm
void initBuffer();
uint8_t readFromFile(char *filename);
int hexToDec(char hex);
uint8_t processRecord(uint8_t *record, uint8_t len);
void printBuffer(uint16_t page);
uint16_t getKeyPressed(void);

int main(int argc, char* argv[]){
	initBuffer();
	uint8_t check = readFromFile(argv[1]);
	if(check == HEX_ERROR_OPEN_FILE){
		printf("%s could not be opened.", argv[1]);
		return 1;
	}
	else if(check == HEX_ERROR_WRONG_FORMAT){
		printf("Line %d of file %s is wrong format.", line, argv[1]);
		return 1;
	}
	uint16_t page = 0;
	printBuffer(page);
	while(1){
		uint16_t key = getKeyPressed();
		if(key == LEFT_ARROW_KEY && page > 0) {
			page--;
			printBuffer(page);
		}
		else if(key == RIGHT_ARROW_KEY && page < (maxAddress / 400)) {
			page++;
			printBuffer(page);
		}
		else if(key == ESC_KEY){
			system("cls");
			break;
		}
	}
	return 0;
}

// Khởi tạo buffer với tất cả các ô có giá trị 0xFF
void initBuffer(){
	for(uint32_t i = 0; i < 65536; i++) {
		buffer[i] = 0xff;
	}
}

// Đọc dữ liệu từ file hex và ghi vào buffer
// Input: 
// filename: tên file hex
// Output:
// + HEX_ERROR_OPEN_FILE: có lỗi khi mở file
// + HEX_ERROR_WRONG_FORMAT: file hex sai định dạng
// + HEX_OK: đọc dữ liệu thành công
uint8_t readFromFile(char *filename){
	uint8_t record[50];
	char c;
	int recordPos, buffPos, start;
	FILE *file = fopen(filename, "r"); 
	if(file == NULL)
		return HEX_ERROR_OPEN_FILE;
	line = 1;
	start = 0;
	recordPos = 0;
	buffPos = 0;
	while((c = fgetc(file)) != EOF){ // khi nào chưa đọc hết file thì vẫn tiếp tục lấy dữ liệu (lấy từng byte một)
		if(isEnd == 1)	// nếu đã đọc được record end-of-file (dánh dấu kết thúc file hex) mà sau đó vẫn còn dữ liệu => sai format
			return HEX_ERROR_WRONG_FORMAT;
		if(start == 0){ // chuẩn bị đọc 1 record mới
			if(c != ':')  // nếu bắt đầu 1 record mà không phải là ký tự : => sai format
				return HEX_ERROR_WRONG_FORMAT;
			start = 1;
		}
		else{
			if(c == '\n'){ // ký tự xuống record (kết thúc record hiện tại)
				if(start == 0) 
					return HEX_ERROR_WRONG_FORMAT;
				uint8_t checkValid = processRecord(record, recordPos); // xử lý record dữ liệu vừa đọc và kiểm tra tính hợp lệ
				if(checkValid != HEX_OK) // nếu không hợp lệ thì kết thúc việc đọc
					return checkValid;
				recordPos = 0;
				start = 0;
				line++;
			}
			else { // vẫn đang trong record dữ liệu hiện tại
				if(hexToDec(c) == -1) // ký tự đọc được không phải hexa
					return HEX_ERROR_WRONG_FORMAT;
				record[recordPos] = hexToDec(c); // lấy 4 bit cao của byte
				c = fgetc(file); // lấy ký tự hexa tiếp theo, chú ý 1 byte gồm 2 ký tự hexa
				if(hexToDec(c) == -1)  // ký tự đọc được không phải hexa
					return HEX_ERROR_WRONG_FORMAT;
				record[recordPos] = record[recordPos] * 16 + hexToDec(c); // dịch trái 4 bit cao đi 4 vị trí sau đó đưa 4 bit thấp vào => hoàn thành 1 byte đưa vào record
				recordPos++;	
			}
		}
	}
	fclose(file); // sau khi đọc xong thì đóng file
	return HEX_OK; // việc đọc thành công
}

// Hàm chuyển 1 ký tự hexa sang giá trị thập phân
// Input
// hex: ký tự hexa
// Output:
// + số nguyên
int hexToDec(char hex){
	if('0' <= hex && hex <= '9') 
		return hex - '0';
	else if('A' <= hex && hex <= 'F') 
		return hex - 'A' + 10;
	else if('a' <= hex && hex <= 'f') 
		return hex - 'a' + 10;
	else 
		return -1; // ký tự hexa không hợp lệ
}

// Hàm xử lý 1 record dữ liệu
// Input:
// record: mảng chứa các byte trong record dữ liệu
// len: chiều dài mảng
// Output:
// + HEX_ERROR_WRONG_FORMAT: file hex sai định dạng
// + HEX_OK: đọc dữ liệu thành công
uint8_t processRecord(uint8_t *record, uint8_t len){
	if(len < 5) // 1 record tối thiểu phải gồm 5 byte (bằng 5 trong trường hợp record ko mang dữ liệu)
		return HEX_ERROR_WRONG_FORMAT;
	uint8_t nBytes = record[0]; // số byte mang dữ liệu nằm ở byte đầu tiên của record
	if(nBytes > 16 || nBytes + 5 != len) // mỗi record chỉ chứa tối đa 16 byte mang dữ liệu, và số byte mang dữ liệu + 5 byte tối thiểu phải khớp với tham số len
		return HEX_ERROR_WRONG_FORMAT;
	uint32_t address = (record[1] << 8) + record[2]; // 4 byte địa chỉ được lấy từ byte thứ 2 và thứ 3 trong record
	// kiểm tra checksum
	uint8_t sum = 0, checksum = record[len - 1];
	for(uint8_t i = 0; i < len - 1; i++) 
		sum += record[i];
	sum = ~sum;
	if(((sum + 0x01) & 0xff) != checksum) // nếu checksum không khớp => sai định dạng
		return HEX_ERROR_WRONG_FORMAT;
	switch (record[3]){ // xử lý record dữ liệu dựa vào byte quy định kiểu của record (byte số 4)
	case 0x00: // 0x00 => record chứa dữ liệu ghi vào các ô nhớ
		address += extendAddress; // tính toán ra địa chỉ tuyệt đối
		if(address <= 0xffff){  // yêu cầu đề bài chỉ xét 64KBytes đầu tiên (tương ứng 2 byte địa chỉ - max là 0xFFFF)
			for(uint8_t i = 0; i < nBytes; i++){ // lấy các byte mang dữ liệu và ghi vào buffer tương ứng với địa chỉ của chúng
				buffer[address + i] = record[i + 4];
			}
			maxAddress = address;
		}
		break;
	case 0x01: // record xác định kết thúc file hex
		if(address != 0x0000 || nBytes != 0) // địa chỉ bắt buộc phải là 0x0000 và không có byte mang dữ liệu
			return HEX_ERROR_WRONG_FORMAT;
		isEnd = 1; // cờ báo đã kết thúc file, nếu sau khi cờ này lên 1 mà trong file vẫn còn data => sai format 
		break;
	case 0x02: // record chứa địa chỉ đoạn mở rộng => địa chỉ vật lý = (địa chỉ đoạn << 4) + địa chỉ đọc được trong record
		if(address != 0x0000 || nBytes != 2) // địa chỉ bắt buộc phải là 0x0000 và phải có 2 byte mang dữ liệu (dữ liệu này chính là địa chỉ đoạn mở rộng)
			return HEX_ERROR_WRONG_FORMAT;
		extendAddress = ((record[4] << 4) + record[5]) << 4;
		break;
	case 0x04: // record chứa địa chỉ tuyến tính mở rộng => địa chỉ vật lý = (địa chỉ tuyến tính << 16) + địa chỉ đọc được trong record
		if(address != 0x0000 || nBytes != 2) // địa chỉ bắt buộc là 0x0000 và phải có 2 byte mang dữ liệu (dữ liệu này chính là địa chỉ đoạn tuyến tính)
			return HEX_ERROR_WRONG_FORMAT;
		extendAddress = ((record[4] << 4) + record[5]) << 16;
		break;
	case 0x05: // record thường gặp trong hex file của vi xử lý dòng MDK-ARM, thường không mang dữ liệu quan trọng liên quan đến các ô nhớ
		if(address != 0x0000 || nBytes != 4) // địa chỉ bắt buộc là 0x0000 và có 4 byte mang dữ liệu
			return HEX_ERROR_WRONG_FORMAT;
		break;
	default: // nếu khác 5 giá trị trên => sai format
		return HEX_ERROR_WRONG_FORMAT;
	}
	return HEX_OK;
}

// Hàm in ra nội dung ô nhớ, in tối đa 25 dòng trên 1 trang
// Input:
// page: số thứ tự trang
void printBuffer(uint16_t page){
	system("cls");
	uint32_t address = page * 400;
	uint8_t i, j;
	printf("Address  | Memory                                          | ASCII \n");
	printf("---------|-------------------------------------------------|-------------------\n");
	for(i = 0; i < 25; i++){
		if(address < maxAddress){
			printf("%08X | ", address);
			for(j = 0; j < 16; j++){
				printf("%02X ", buffer[address + j]);
			}
			printf("| ");
			for(j = 0; j < 16; j++){
				uint8_t c = buffer[address + j];
				if(c < 0x20) c = '.';
				printf("%c", c);
			}
			address += 16;
		}
		printf("\n");
	}
	printf("Page %d of %d\n", page + 1, (maxAddress / 400) + 1);
	printf("Press <- / -> for prev/next page. Press Esc for exit");
}

// Hàm lấy mã phím nhấn
uint16_t getKeyPressed(void){
	uint16_t c;
	c = getch() << 8;
	if(c == 0xe000 || c == 0x0000) c |= getch();
	return c;
}
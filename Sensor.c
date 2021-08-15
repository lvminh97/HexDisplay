#include <stdio.h>
#include <string.h>

int interval, sample_time; 
char interval_unit[10], sample_time_unit[10];

void input(){
    // default value
    interval = 7;
    strcpy(interval_unit, "ngay");
    sample_time = 5;
    strcpy(sample_time_unit, "phut");
    // input
    printf("Nhap khoang thoi gian thuc hien phep do: ");
    scanf("%d %s", &interval, &interval_unit);
    printf("Nhap chu ky lay mau: ");
    scanf("%d %s", &sample_time, &sample_time_unit);
}

void generateData(){
    
}

int main(int argc, char* argv[]){
    
}
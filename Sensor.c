#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct Time{
    unsigned int year, month, day, hour, minute, second;
} Time;

typedef struct Record{
    Time time;
    float temp;
} Record;

int topOfMonth(int year, int month){
    switch(month){
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            if((year % 100 != 0 && year % 4 == 0) || year % 400 == 0)
                return 29;
            else
                return 28;
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        default:
            return 0;
    }
}

int isValidTime(Time ts){
    // check year is valid
    if(ts.year < 0)
        return 0;
    // check month is valid
    if(ts.month < 1 || ts.month > 12)
        return 0;
    // check day is valid
    if(ts.day < 1 || ts.day > topOfMonth(ts.year, ts.month))
        return 0;
    // check hour
    if(ts.hour < 0 || ts.hour > 23)
        return 0;
    // check minute
    if(ts.minute < 0 || ts.minute > 59)
        return 0;
    // check second
    if(ts.second < 0 || ts.second > 59)
        return 0;
    // valid
    return 1;
}

Time addTime(Time t, unsigned int minute){
    Time rs = {0, 0, 0, 0, 0, 0};
    if(!isValidTime(t))
        return rs;
    rs.year = t.year;
    rs.month = t.month;
    rs.day = t.day;
    rs.hour = t.hour;
    rs.minute = t.minute + minute;
    rs.second = t.second;
    if(rs.minute > 59){
        rs.hour += rs.minute / 60;
        rs.minute %= 60;
    }
    if(rs.hour > 23){
        rs.day += rs.hour / 24;
        rs.hour %= 24;
    }
    while(rs.day > topOfMonth(rs.year, rs.month)){
        rs.day -= topOfMonth(rs.year, rs.month);
        rs.month++;
        if(rs.month > 12){
            rs.month = 1;
            rs.year++;
        }
    }
    return rs;
}

void printTime(Time t){
    printf("%04d-%02d-%02d %02d:%02d:%02d", t.year, t.month, t.day, t.hour, t.minute, t.second);
}

unsigned int interval, sample_time; 
unsigned int dataLen;
Record data[30000];

void task1(char *name){
    printf("Nhap khoang thoi gian thuc hien phep do (ngay): ");
    scanf("%d", &interval);
    printf("Nhap chu ky lay mau (phut): ");
    scanf("%d", &sample_time);
    char filename[50];
    sprintf(filename, "%s_temperature_data.csv", name);
    FILE *file = fopen(filename, "w");
    Time t = {2021, 8, 16, 0, 0, 0};
    interval *= 1440;
    fprintf(file, "time,values\n");
    while(interval > sample_time){
        interval -= sample_time;
        t = addTime(t, sample_time);
        fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d,%.1f\n", t.year, t.month, t.day, t.hour, t.minute, t.second, (rand() % 751 - 125) * 0.2);
    }
    fclose(file);
}

void task21(){

}

void task2(char *name){
    char filename[50];
    sprintf(filename, "%s_temperature_data.csv", name);
    FILE *file = fopen(filename, "r");
    char tmp;
    while((tmp = fgetc(file)) != '\n');
    int id = 0;
    // while()
    // fscanf(file, "%04d-%02d-%02d %02d:%02d:%02d,%.2f", &data[id].time.year, 
    //                                                     &data[id].time.month, 
    //                                                     &data[id].time.day, 
    //                                                     &data[id].time.hour,
    //                                                     &data[id].time.minute,
    //                                                     &data[id].time.second,
    //                                                     &data[id].temp);

    fclose(file);
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    task1(argv[1]);
    task2(argv[1]);
}
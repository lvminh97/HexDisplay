#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct Time{
    int day, month, year, hour, minute, second;
} Time;

int isValidTime(Time ts){
    // check year is valid
    if(ts.year < 0)
        return 0;
    // check month is valid
    if(ts.month < 1 || ts.month > 12)
        return 0;
    // check day is valid
    if(ts.day < 1)
        return 0;
    else{
        if(ts.month == 4 || ts.month == 6 || ts.month == 9 || ts.month == 11){
            if(ts.day > 30)
                return 0;
        }
        else if(ts.month == 2){
            if((ts.year % 100 != 0 && ts.year % 4 == 0) || ts.year % 400 == 0){
                if(ts.day > 29)
                    return 0;
            }
            else if(ts.day > 28)
                return 0;
        } 
        else if(ts.day > 31)
            return 0;
    }
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

Time addTime(Time t, int minute){
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
    if((rs.month))
    return rs;
}

unsigned int interval, sample_time; 

void input(){
    // default value
    interval = 7;
    sample_time = 5;
    // input
    printf("Nhap khoang thoi gian thuc hien phep do (ngay): ");
    scanf("%d", &interval);
    printf("Nhap chu ky lay mau (phut): ");
    scanf("%d", &sample_time);
}

void generateData(char *name){
    char filename[50];
    strcpy(filename, name);
    strcat(filename, "_temperature_data.csv");
    FILE *file = fopen(filename, "w");
    Time t = {2021, 8, 16, 0, 0, 0};
    interval *= 1440;
    while(interval > sample_time){
        interval -= sample_time;
        t = addTime(t, sample_time);

    }
    fclose(file);
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    input();

}
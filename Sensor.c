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

int topOfMonth(int year, int month);
int isValidTime(Time ts);
Time addTime(Time t, unsigned int minute);
int dateCompare(Time t1, Time t2);

int partitionSort(int L, int R, int id);
void quickSort(int L, int R);

void task1(char *name, char *stuId);
void task2(char *name);
void task21(char *name);
void task22(char *name);
void task23(char *name);
void task24(char *name);

unsigned int interval, sample_time; 
unsigned int dataLen;
Record data[30000];

int main(int argc, char* argv[]){
    srand(time(NULL));
    task1(argv[1], argv[2]);
    task2(argv[1]);
}

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

int dateCompare(Time t1, Time t2){
    if(!isValidTime(t1) || !isValidTime(t2))
        return -2; // invalid time format
    // compare year
    if(t1.year < t2.year)
        return -1;
    else if(t1.year > t2.year)
        return 1;
    // compare month
    if(t1.month < t2.month)
        return -1;
    else if(t1.month > t2.month)
        return 1;
    // compare day
    if(t1.day < t2.day)
        return -1;
    else if(t1.day > t2.day)
        return 1;
    // equal
    return 0;
}

void task1(char *name, char *stuId){
    printf("Nhap khoang thoi gian thuc hien phep do (ngay): ");
    scanf("%d", &interval);
    printf("Nhap chu ky lay mau (phut): ");
    scanf("%d", &sample_time);
    int minRecord;
    sscanf(stuId, "%d", &minRecord);
    minRecord = 1000 + (minRecord % 1000);
    if(1440 * interval / sample_time < minRecord){
        printf("So diem do khong thoa man. So diem do toi thieu phai bang 1000 + 3 so cuoi cua ma sinh vien");
        exit(1);
    }
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

void task2(char *name){
    char filename[50];
    sprintf(filename, "%s_temperature_data.csv", name);
    FILE *file = fopen(filename, "r");
    char tmp[50];
    fgets(tmp, 50, file);   // get the title line
    // get data from file
    int id = 0;
    while(fgets(tmp, 50, file) != NULL){
        // fgets(tmp, 50, file);
        if(tmp[0] == 0)
            break;
        sscanf(tmp, "%04d-%02d-%02d %02d:%02d:%02d,%f", &data[id].time.year, &data[id].time.month, &data[id].time.day,
                                                            &data[id].time.hour, &data[id].time.minute, &data[id].time.second, &data[id].temp);
        id++;
    }
    dataLen = id;
    fclose(file);
    task21(name);
    task22(name);
    task23(name);
    task24(name);
}

void task21(char *name){
    char filename[50];
    sprintf(filename, "%s_outlier_data.csv", name);
    FILE *file = fopen(filename, "w");
    int invalidCnt = 0;
    for(int i = 0; i < dataLen; i++){
        if(data[i].temp < 15.0 || data[i].temp > 42.0)
            invalidCnt++;
    }
    fprintf(file, "invalid_data_points,%d\n", invalidCnt);
    fprintf(file, "time,values\n");
    for(int i = 0; i < dataLen; i++){
        if(data[i].temp < 15.0 || data[i].temp > 42.0)
            fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d,%.1f\n", data[i].time.year, data[i].time.month, 
                                                data[i].time.day, data[i].time.hour, data[i].time.minute, 
                                                data[i].time.second, data[i].temp);
    }
    fclose(file);
}

void task22(char *name){
    char filename[50];
    sprintf(filename, "%s_data_summary.csv", name);
    FILE *file = fopen(filename, "w");
    fprintf(file, "parameters,time,values\n");
    double sum = 0, min = 125.0, max = -25.0;
    int cnt = 0;
    Time t;
    t = data[0].time;
    for(int i = 0; i < dataLen; i++){
        if(dateCompare(data[i].time, t) == 0){
            if(data[i].temp >= 15.0 && data[i].temp <= 42.0){
                sum += data[i].temp;
                min = min > data[i].temp ? data[i].temp : min;
                max = max < data[i].temp ? data[i].temp : max;
                cnt++;
            }
        }
        else{
            fprintf(file, "temperature_max,%04d-%02d-%02d,%.1f\n", data[i - 1].time.year, data[i - 1].time.month, data[i - 1].time.day, max);
            fprintf(file, "temperature_min,%04d-%02d-%02d,%.1f\n", data[i - 1].time.year, data[i - 1].time.month, data[i - 1].time.day, min);
            fprintf(file, "mean,%04d-%02d-%02d,%.1f\n", data[i - 1].time.year, data[i - 1].time.month, data[i - 1].time.day, sum / cnt);
            sum = 0;
            min = 125.0;
            max = -25.0;
            cnt = 0;
            t = data[i].time;
        }
    }
    if(cnt > 0){
        fprintf(file, "temperature_max,%04d-%02d-%02d,%.1f\n", data[dataLen - 1].time.year, data[dataLen - 1].time.month, data[dataLen - 1].time.day, max);
        fprintf(file, "temperature_min,%04d-%02d-%02d,%.1f\n", data[dataLen - 1].time.year, data[dataLen - 1].time.month, data[dataLen - 1].time.day, min);
        fprintf(file, "mean,%04d-%02d-%02d,%.1f\n", data[dataLen - 1].time.year, data[dataLen - 1].time.month, data[dataLen - 1].time.day, sum / cnt);
    }
    fclose(file);
}

void task23(char *name){
    char filename[50];
    sprintf(filename, "%s_data_statistics.csv", name);
    FILE *file = fopen(filename, "w");
    fprintf(file, "range,quantity\n");
    int cnt_15_18 = 0, cnt_18_23 = 0, cnt_23_28 = 0, cnt_28_32 = 0, cnt_32_35 = 0, cnt_35_38 = 0, cnt_38_42 = 0;
    for(int i = 0; i < dataLen; i++){
        float value = data[i].temp;
        if(value >= 15.0 && value < 18.0)
            cnt_15_18++;
        else if(value >= 18.0 && value < 23.0)
            cnt_18_23++;
        else if(value >= 23.0 && value < 28.0)
            cnt_23_28++;
        else if(value >= 28.0 && value < 32.0)
            cnt_28_32++;
        else if(value >= 32.0 && value < 35.0)
            cnt_32_35++;
        else if(value >= 35.0 && value < 38.0)
            cnt_35_38++;
        else if(value >= 38.0 && value <= 42.0)
            cnt_38_42++;
    }
    fprintf(file, "15 to 18,%d\n", cnt_15_18);
    fprintf(file, "18 to 23,%d\n", cnt_18_23);
    fprintf(file, "23 to 28,%d\n", cnt_23_28);
    fprintf(file, "28 to 32,%d\n", cnt_28_32);
    fprintf(file, "32 to 35,%d\n", cnt_32_35);
    fprintf(file, "35 to 38,%d\n", cnt_35_38);
    fprintf(file, "38 to 42,%d\n", cnt_38_42);
    fclose(file);
}

void task24(char *name){
    char filename[50];
    sprintf(filename, "%s_temperature_data_sorted.csv", name);
    FILE *file = fopen(filename, "w");
    fprintf(file, "time,values\n");
    quickSort(0, dataLen - 1);
    for(int i = 0; i < dataLen; i++){
        fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d,%.1f\n", data[i].time.year, data[i].time.month, data[i].time.day, 
                    data[i].time.hour, data[i].time.minute, data[i].time.second, data[i].temp);
    }
    fclose(file);
}

int partitionSort(int L, int R, int id){
    float pivot = data[id].temp;
    int cnt = 0;
    Record tmp;
    // swap id <-> R
    tmp = data[id];
    data[id] = data[R];
    data[R] = tmp;
    for(int i = L; i < R; i++){
        if(data[i].temp > pivot){
            tmp = data[i];
            data[i] = data[L + cnt];
            data[L + cnt] = tmp;
            cnt++;
        }
    }
    tmp = data[R];
    data[R] = data[L + cnt];
    data[L + cnt] = tmp;
    return L + cnt;
}

void quickSort(int L, int R){
    if(L < R){
        int id = (L + R) / 2;
        id = partitionSort(L, R, id);
        if(id > L) quickSort(L, id - 1);
        if(id < R) quickSort(id + 1, R);
    }
}
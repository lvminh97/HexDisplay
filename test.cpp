#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

int main(){
nhaplai:
	int a, b, i = 0, Min, Max;
	do{
		printf("\nNhap mot so: ");
		scanf("%d", &a);
		if(a < 0) printf("\nBan nhap sai! Nhap lai!");
	}	
	while(a < 0);
	
	printf("\nYes/No? ");
	char m;
	m = getch();
	if(m == 'Y' || m == 'y'){
		system("cls");
		goto nhaplai;
	}
}

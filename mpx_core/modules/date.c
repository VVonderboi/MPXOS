#include "mpx_supt.h"
#include <core/serial.h>
#include <string.h>
#include <core/io.h>
#include "comhand.h"

void setDate(char* date);
void getDate();

/** 
* Name = setDate\n
* Query = /setDate = (parameter);\n
* Parameters: month/day/year (xx/yy/zzzz)\n
* Description: Sets the date based on the inputted string.
*/
void setDate(char* date){
	cli();
	
	char dateCopy[strlen(date)];
	strcpy(dateCopy,date);
	
	outb(0x70,0x08);
	
	char monthArray[3];
	memset(monthArray, '\0', 3);
	monthArray[0]=dateCopy[0];
	monthArray[1]=dateCopy[1];
	monthArray[2]='\0';
	
	int month = atoi(monthArray);
	if (month > 12 || month < 1){
		klogv("Not a valid month, please enter between 1 and 12 inclusive.");
		return;
	}
	unsigned char temp10= iToBcd(month);
	outb(0x71, temp10);
	
	
	outb(0x70,0x07);
	char dayArray[3];
	memset(dayArray, '\0', 3);
	dayArray[0]=dateCopy[3];
	dayArray[1]=dateCopy[4];
	dayArray[2]='\0';
	
	int day = atoi(dayArray);
	if (day > 31 || day < 1){
		klogv("Not a valid day, please enter between 1 and 31 inclusive.");
		return;
	}
	if((monthArray[1] == '2' || monthArray[1] == '4' || monthArray[1] == '6' || monthArray[1] == '8' || monthArray[1] == '9' || month == 11) && day == 31){
		klogv("Not a valid day for this month, please enter a new date.");
		return;
	}
	char year[5];
	memset(year,'\0',5);
	year[0]=dateCopy[6];
	year[1]=dateCopy[7];
	year[2]=dateCopy[8];
	year[3]=dateCopy[9];
	year[4]='\0';
	int yearTemp = atoi(year);
	if(yearTemp < 2021 || yearTemp > 2100){
		klogv("Not a supported year, please choose a new year.");
		return;
	}
	if(month==2 && day > 28){
		if((yearTemp%4)==0){
			if(day==29){
			}
			else{
				klogv("Not a valid day for this month, please enter a new date.");
				return;
			}
		}
		else{
			klogv("Not valid, please choose a new date.");
			return;
		}
		
	}
	unsigned char temp11= iToBcd(day);
	outb(0x71,temp11);
	
	outb(0x70,0x09);
	char half1Array[3];
	memset(half1Array, '\0', 3);
	char half2Array[3];
	memset(half2Array, '\0', 3);
	half1Array[0]=dateCopy[6];
	half1Array[1]=dateCopy[7];
	half1Array[2]='\0';
	half2Array[0]=dateCopy[8];
	half2Array[1]=dateCopy[9];
	half2Array[2]='\0';
	
	int half1 = atoi(half1Array);
	if (strlen(dateCopy)>10){
		klogv("Not a valid date, please enter a new date.");
		return;
	}
	unsigned char temp12 = iToBcd(half1);
	outb(0x71,temp12);
	
	outb(0x70,0x32);
	int half2 = atoi(half2Array);
	unsigned char temp13 = iToBcd(half2);
	outb(0x71,temp13);
				
	sti();
}

/** 
* Name = getDate\n
* Query = /getDate;\n
* Parameters: N/A\n
* Description: Returns the date in  string form.
*/
void getDate(){
	outb(0x70, 0x08);
	unsigned char monthPrint =inb(0x71); 	
	printString("The date is ");
	bcdToI(monthPrint);
	printString("/");
	//printString("/");
	
	outb(0x070, 0x07);
	unsigned char dayPrint =inb(0x71);
	bcdToI(dayPrint);
	printString("/");
	//printString("/");
	
	outb(0x70, 0x09);
	unsigned char half1Print =inb(0x71);
	bcdToI(half1Print);
	printString("");
	//printString("/");
	
	outb(0x70, 0x32);
	unsigned char half2Print =inb(0x71);
	bcdToI(half2Print);
	printStringln("");
	//printStringln("");
}


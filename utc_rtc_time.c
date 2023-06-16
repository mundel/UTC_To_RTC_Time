// Online C compiler to run C program online
//You need to modify code as per your requirement 
//This is smaple code only

#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {
    unsigned char Second;
    unsigned char Minute;
    unsigned char Hour;
    unsigned char Day;
    unsigned char Month;
    unsigned int  Year;
    unsigned char DayOfWeek;
}RtcDataTypeDef;

//country specific time zone 
//Below one is for IST(+5:30) time zone

#define IST_OFFSET_VALUE 		19800u //((5.5)*60*60)	5:30 for (IST) 
#define UTC_OFFSET_YEAR	 		1970u
#define DAYS_IN_A_LEAP_YEAR		366u
#define DAYS_IN_AN_YEAR			365u
#define HOUR_IN_A_DAY			24u
#define SECONDS_IN_A_MINUTE		(60u)
#define SECONDS_IN_AN_HOUR		(60u*60u)
#define SECONDS_IN_DAY			((HOUR_IN_A_DAY) * (SECONDS_IN_AN_HOUR))
#define SECONDS_IN_AN_YEAR		((DAYS_IN_AN_YEAR) * (HOUR_IN_A_DAY) * (SECONDS_IN_AN_HOUR))
#define SECONDS_IN_A_LEAP_YEAR	((DAYS_IN_A_LEAP_YEAR) * (HOUR_IN_A_DAY) * (SECONDS_IN_AN_HOUR))

#define IS_LEAP_YEAR(year) 		((!(year % 4) && (year % 100)) || (!(year % 400)))
const int fg_UTCDay[] 		= {0,31,59,90,120,151,181,212,243,273,304,334,365};
const int fg_UTCLeapDay[] 	= {0,31,60,91,121,152,182,213,244,274,305,335,366};


const char  fg_UTCMonth[]  	 = {31,28,31,30,31,30,31,31,30,31,30,31};
const char  fg_UTCLeapMonth[] = {31,29,31,30,31,30,31,31,30,31,30,31};

unsigned char bcdToDec(unsigned char value)
{
  return ((value / 16) * 10 + value % 16);
}

unsigned char decToBcd(unsigned char value){
  return (value / 10 * 16 + value % 10);
}

//===============================================
//passing utctime and it conver to RTC (IST time) where year is xxxx(like 2023)
void ConvertUTCtoRTC(unsigned int a_UTCinSeconds, RtcDataTypeDef *a_pRtcStuct)
{
	unsigned int l_UTCTime = 0;
	unsigned int YearSec, MonthSec, RemainSec;
	unsigned int day, month, year;
	unsigned char hour, minute;

	l_UTCTime = a_UTCinSeconds;
	l_UTCTime += IST_OFFSET_VALUE; //Add 5:30 for Indian time

	year= 0;
	YearSec = 0;

	for(int l_Count = UTC_OFFSET_YEAR; ; l_Count++)
	{
		year = l_Count;
		if(IS_LEAP_YEAR(l_Count))
		{
			YearSec += SECONDS_IN_A_LEAP_YEAR;

			if(YearSec > l_UTCTime)
			{
				YearSec -= SECONDS_IN_A_LEAP_YEAR;
				break;
			}
		}
		else
		{
			YearSec += SECONDS_IN_AN_YEAR;
			if(YearSec > l_UTCTime)
			{
				YearSec -= SECONDS_IN_AN_YEAR;
				break;
			}
		}
	}

	RemainSec = l_UTCTime - YearSec;	//(l_UTCTime % 31536000);	//(365*24*3600) //calculate remaining seconds after year calculate
	month = 0 ;
	MonthSec = 0;

	if(IS_LEAP_YEAR((year)))
	{
		for(unsigned int l_Count1 = 0; ; l_Count1++)
		{
			month++;
			MonthSec +=(fg_UTCLeapMonth[l_Count1] * SECONDS_IN_DAY);

			if((MonthSec > RemainSec) || (month > 12))
			{
				MonthSec -=(fg_UTCLeapMonth[l_Count1] * SECONDS_IN_DAY);
				break;
			}
		}
	}
	else
	{
		for(unsigned int l_Count2 = 0; ; l_Count2++)
		{
			month++;
			MonthSec +=(fg_UTCMonth[l_Count2] * SECONDS_IN_DAY);

			if((MonthSec > RemainSec) || (month > 12))
			{
				MonthSec -=(fg_UTCMonth[l_Count2] * SECONDS_IN_DAY);
				break;
			}
		}
	}

	RemainSec = RemainSec - MonthSec; //Remaining seconds after month calculate
	day = (unsigned char) (RemainSec / SECONDS_IN_DAY);
	day += 1;
	RemainSec = (RemainSec % SECONDS_IN_DAY);
	hour = (unsigned char) (RemainSec / SECONDS_IN_AN_HOUR);
	RemainSec = (RemainSec % SECONDS_IN_AN_HOUR);
	minute = (RemainSec / SECONDS_IN_A_MINUTE);
	RemainSec = (RemainSec % SECONDS_IN_A_MINUTE);

	a_pRtcStuct->Day = day;
	a_pRtcStuct->Month = month;
	a_pRtcStuct->Year = year;
	a_pRtcStuct->Hour = hour;
	a_pRtcStuct->Minute = minute;
	a_pRtcStuct->Second = RemainSec;
}

//==============================================================================
//Returns time in UTC Format
//Passing RTC time(IST time where year in 4 digit xxxx (like.. 2023) and return UTC time
unsigned int UTL_GetRTC_to_UTCTime(RtcDataTypeDef* a_pRTC)
{
	unsigned int l_year = 0;
	unsigned char l_month = 0;
	unsigned char l_day = 0;
	unsigned char l_leapDays = 0;
	unsigned int l_TimeInSeconds = 0;
	unsigned int l_TempVal = 0;
	unsigned int l_Count = 0;

	l_year = a_pRTC->Year;
	//if(l_iYY >= 100)
	//l_iYY -= 100;   //Reference year 2000 but in linux 2012 read as 112 so we write 100
	//We are assuming arument sending by user from external RTC 
	//RTC year value always coming in 2 digit only, for example if 98 value received from RTC 
	// it may be 1998 or 2098, So we can support our range in between 1970 to 2069
	if((l_year >= 70) && (l_year  <= 99))
	{
		l_year += 1900;
	}
	else	//We are assuming rage is 2000 to 2069
	{
		l_year += 2000;
	}

	if(l_year >= UTC_OFFSET_YEAR)
	{
		l_year -= UTC_OFFSET_YEAR; 
	}

	//Calculate leap day from 1970 to l_year passed in argument
	for(l_Count = UTC_OFFSET_YEAR; l_Count < (l_year + UTC_OFFSET_YEAR); l_Count++)  //calculate leap day
	{
		if(IS_LEAP_YEAR(l_Count))
		{
			l_leapDays++;
		}
	}

	if(a_pRTC->Month)
	l_month = a_pRTC->Month - 1;  //discard because january 0 in linux
	if(a_pRTC->Day)
	l_day = a_pRTC->Day - 1; //date 

	//years seconds
	l_TimeInSeconds  = (l_year * 365 * 24 * 3600);

	////months seconds
	if(IS_LEAP_YEAR(l_Count)) //if((l_Count % 4) == 0x00) //current year If Leap Year
	{
		l_TempVal = fg_UTCLeapDay[l_month];
	}
	else
	{
		l_TempVal = fg_UTCDay[l_month];
	}

	l_TimeInSeconds += (l_TempVal*24*3600);
	l_TimeInSeconds += (l_day*24*3600);				//Days seconds
	l_TimeInSeconds += (a_pRTC->Hour*3600);			//Hours seconds
	l_TimeInSeconds += (a_pRTC->Minute*60);			//Minute seconds
	l_TimeInSeconds += (l_leapDays*24*3600);			//leap days seconds
	l_TimeInSeconds += a_pRTC->Second;				//seconds

	l_TimeInSeconds -= IST_OFFSET_VALUE; //Subtracted +5.30 Hrs - India Time is ahead of 5 hrs 30 mins

	return(l_TimeInSeconds);
}
int main() 
{
    // Write C code here
	unsigned int l_utc = 0;
	RtcDataTypeDef l_rtc;
	time_t  seconds;
    seconds = time(NULL);
	
    printf("Utc time = %ld", seconds);
	//test code here working as expected or not
	//read system data and time
	ConvertUTCtoRTC(seconds, &l_rtc);
	printf("\nTime = %02d:%02d:%02d", l_rtc.Hour, l_rtc.Minute, l_rtc.Second);
	printf("\nDate = %02d:%02d:%02d \n", l_rtc.Day, l_rtc.Month, l_rtc.Year);
	
	l_rtc.Year -= 2000;
	
	l_utc = UTL_GetRTC_to_UTCTime(&l_rtc);
	
	if(l_utc ==  seconds)
	{
		printf("Utc time match");
	}
	else{
		printf("Utc time not match (l_utc=%ld, seconds=%ld", l_utc, seconds);
	}
	

    return 0;
}

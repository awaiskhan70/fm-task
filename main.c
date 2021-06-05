#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define MAXTIMINGS	85
#define DHTPIN		7
#define LED_BULE	1
#define LED_RED		26
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
 
float read_dht11_dat()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f,current_temperature; 
 
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
 
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHTPIN, INPUT );
 
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) )
		{
			dht11_dat[j / 8] <<= 1;
			if ( counter > 16 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	}
 
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{
		//f = dht11_dat[2] * 9. / 5. + 32;
		//printf( "Humidity = %d.%d %% Temperature = %d.%d C (%.1f F)\n",dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
		current_temperature= dht11_dat[3] * 1. / 10. +  dht11_dat[2];
		printf("current temperature = %f \n ",current_temperature);
		
	}else  {
		//printf( "Data not good, skip\n" );
	}
	return current_temperature;
}
 
int main( void )
{
	float temperature_threashold = 25.5;
	float current_temperature;
	wiringPiSetup();
	pinMode(LED_BULE , OUTPUT );
	pinMode(LED_RED , OUTPUT );
	
	if ( wiringPiSetup() == -1 )
		exit( 1 );
	FILE *f;
	f=fopen("temperature-sensor-config.txt","r");
	if(f!=NULL)
	{
		fscanf(f,"%f",&temperature_threashold);
		printf("Temperature Threshold = %f \n",temperature_threashold);
		rewind(f);
		fclose(f);
	}
	else
	{
		f=fopen("temperature-sensor-config.txt","w+");
		fputs("25.5",f);
		rewind(f);
		fclose(f);		
	}
	while ( 1 )
	{
		current_temperature=read_dht11_dat();
		if(current_temperature > temperature_threashold)
		{
		digitalWrite( LED_RED , HIGH );
			printf("warning temperature is HIGH \n");
		}
		else
		{
		digitalWrite( LED_RED , LOW );
		}
		delay( 2000 ); 
	}
 
	return(0);
}

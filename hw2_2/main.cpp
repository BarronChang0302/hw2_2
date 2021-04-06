#include "mbed.h"
#include "uLCD_4DGL.h"
#include "mbed_events.h"
using namespace std::chrono;

DigitalIn up(D3);
DigitalIn down(D4);
DigitalIn sel(D6);
uLCD_4DGL uLCD(D1, D0, D2);
AnalogOut aout(D7);
AnalogIn ain(A0);


Thread ADCt;
Thread DACt;
EventQueue ADCQ;
EventQueue DACQ;

float ADCdata[200];

void ADC()
{
    for(int i = 0; i < 200; i++){
        ADCdata[i] = ain;
        wait_us(100 - 12);
    }
    for(int i = 0; i < 200; i++){
        printf("%f\r\n", ADCdata[i]);
    }
}

void DAC()
{
    while(1) {
       for(float i = 0; i <= 3.0/3.3; i += 3.0/3.3/40) {
           aout = i;
           wait_us(100 - 12);
       }
       for(float i = 3.0/3.3 - 3.0/3.3/60; i >= 3.0/3.3/60; i -= 3.0/3.3/60) {
           aout = i;
           wait_us(100 - 12);
       }
    }
}

int main()
{
    int f = 1, freq;
    float T;

    // Plot uLCD
    for(int i = 0; i <= 5; i++) {
        uLCD.locate(2 + 2 * i, 5);
        uLCD.printf("%d", i);
    } 
    uLCD.rectangle(20, 50, 90, 60, 0xFFFFFF);
    uLCD.filled_rectangle(20, 50, 34, 60, 0xFFFFFF);
    uLCD.locate(5, 8);
    uLCD.printf("Unit:100(Hz)");

    // Select frequency
    while(1) {
        if(f != 5 && up) {
            f++;
            uLCD.filled_rectangle(20, 50, 20 + 14 * f, 60, 0xFFFFFF);
            uLCD.filled_rectangle(20 + 14 * f, 50, 90, 60, 0x000000);
        }
        if(f != 1 && down) {
            f--;
            uLCD.filled_rectangle(20, 50, 20 + 14 * f, 60, 0xFFFFFF);
            uLCD.filled_rectangle(20 + 14 * f, 50, 90, 60, 0x000000);
        }
        uLCD.rectangle(20, 50, 90, 60, 0xFFFFFF);
        if(sel) break;
        ThisThread::sleep_for(100ms);
    }

    freq = f * 100;
    T = 1.0 / freq;
    float time1 = 1000000 * T / 100;
    float total_time = 200 * time1;

    printf("%f\r\n", total_time);
    printf("%f\r\n", total_time);

    DACt.start(callback(&DACQ, &EventQueue::dispatch_forever));
    ADCt.start(callback(&ADCQ, &EventQueue::dispatch_forever));
    DACQ.call(DAC);
    ADCQ.call(ADC);
}
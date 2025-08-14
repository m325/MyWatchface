#include "Watchy_7_SEG.h"

//TODO Somewhere in settings?
//Time settings
#define NIGHT_START 22 //switch from Dark Mode to Light Mode when the hour is 22
#define NIGHT_END 6 //switch from Light Mode to Dark Mode when the hour is 6

const uint8_t BATTERY_SEGMENT_WIDTH = 7;
const uint8_t BATTERY_SEGMENT_HEIGHT = 11;
const uint8_t BATTERY_SEGMENT_SPACING = 9;
const uint8_t WEATHER_ICON_WIDTH = 48;
const uint8_t WEATHER_ICON_HEIGHT = 32;

void Watchy7SEG::drawWatchFace(){
    updateDarkmode();
    display.fillScreen(darkmode ? GxEPD_BLACK : GxEPD_WHITE);
    display.setTextColor(darkmode ? GxEPD_WHITE : GxEPD_BLACK);
    drawTime();
    drawDate();
    updateWeather();
    drawBattery();
    display.drawBitmap(116, 75, WIFI_CONFIGURED ? wifi : wifioff, 26, 18, darkmode ? GxEPD_WHITE : GxEPD_BLACK);
    if(BLE_CONFIGURED){
        display.drawBitmap(100, 73, bluetooth, 13, 21, darkmode ? GxEPD_WHITE : GxEPD_BLACK);
    }
    #ifdef ARDUINO_ESP32S3_DEV
    if(USB_PLUGGED_IN){
      display.drawBitmap(140, 75, charge, 16, 18, darkmode ? GxEPD_WHITE : GxEPD_BLACK);
    }
    #endif
}

void Watchy7SEG::updateDarkmode(){
    //TODO: Adapt timeframe based on settings
    if(currentTime.Hour <= NIGHT_START && currentTime.Hour >= NIGHT_END)
    {
      darkmode = true;
    }
    else
    {
      darkmode = false;
    }
}

void Watchy7SEG::drawTime(){
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setCursor(5, 53+5);
    int displayHour;
    if(HOUR_12_24==12){
      displayHour = ((currentTime.Hour+11)%12)+1;
    } else {
      displayHour = currentTime.Hour;
    }
    if(displayHour < 10){
        display.print("0");
    }
    display.print(displayHour);
    display.print(":");
    if(currentTime.Minute < 10){
        display.print("0");
    }
    display.println(currentTime.Minute);
}

void Watchy7SEG::drawDate(){
    display.setFont(&Seven_Segment10pt7b);

    int16_t  x1, y1;
    uint16_t w, h;

    String dayOfWeek = dayStr(currentTime.Wday);
    display.getTextBounds(dayOfWeek, 5, 85, &x1, &y1, &w, &h);
    if(currentTime.Wday == 4){
        w = w - 5;
    }
    display.setCursor(85 - w, 85);
    display.println(dayOfWeek);

    String month = monthShortStr(currentTime.Month);
    display.getTextBounds(month, 60, 110, &x1, &y1, &w, &h);
    display.setCursor(85 - w, 110);
    display.println(month);

    display.setFont(&DSEG7_Classic_Bold_25);
    display.setCursor(5, 120);
    if(currentTime.Day < 10){
    display.print("0");
    }
    display.println(currentTime.Day);
    display.setCursor(5, 150);
    display.println(tmYearToCalendar(currentTime.Year));// offset from 1970, since year is stored in uint8_t
}
void Watchy7SEG::drawSteps(){
    // reset step counter at midnight
    if (currentTime.Hour == 0 && currentTime.Minute == 0){
      sensor.resetStepCounter();
    }
    uint32_t stepCount = sensor.getCounter();
    display.drawBitmap(10, 165, steps, 19, 23, darkmode ? GxEPD_WHITE : GxEPD_BLACK);
    display.setCursor(35, 190);
    display.println(stepCount);
}
void Watchy7SEG::drawBattery(){
    display.drawBitmap(158, 73, battery, 37, 21, darkmode ? GxEPD_WHITE : GxEPD_BLACK);
    display.fillRect(163, 78, 27, BATTERY_SEGMENT_HEIGHT, darkmode ? GxEPD_BLACK : GxEPD_WHITE);//clear battery segments
    int8_t batteryLevel = 0;
    float VBAT = getBatteryVoltage();
    if(VBAT > 4.0){
        batteryLevel = 3;
    }
    else if(VBAT > 3.6 && VBAT <= 4.0){
        batteryLevel = 2;
    }
    else if(VBAT > 3.20 && VBAT <= 3.6){
        batteryLevel = 1;
    }
    else if(VBAT <= 3.20){
        batteryLevel = 0;
    }

    for(int8_t batterySegments = 0; batterySegments < batteryLevel; batterySegments++){
        display.fillRect(163 + (batterySegments * BATTERY_SEGMENT_SPACING), 78, BATTERY_SEGMENT_WIDTH, BATTERY_SEGMENT_HEIGHT, darkmode ? GxEPD_WHITE : GxEPD_BLACK);
    }
}

void Watchy7SEG::updateWeather(){
    weatherData currentWeather = getWeatherData();
}
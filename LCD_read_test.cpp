#include "jhd1313m1.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

#define RGB_ADDRESS 0x62
#define LCD_ADDRESS 0x3E

int main(int argc, char **argv)
{
    FILE* fp = fopen("sensor.in", "r");
    char *buffer = NULL;
    size_t n;
//! [Interesting]
    // 0x62 RGB_ADDRESS, 0x3E LCD_ADDRESS
    upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0, 0x3E, 0x62);
    // mraa::Result x = lcd->createChar((uint8_t) 0, (uint8_t*) lenface1);
    lcd->displayOn();
    lcd->cursorOn();
    lcd->cursorBlinkOn();
    
    lcd->setCursor(0,0);
    lcd->write("Row #|     Value");

    int row_num = 1;
    while(getline(&buffer, &n, fp) >= 0)
    {
        lcd->setCursor(1,0);
        char row_line[17] = {};
        sprintf(row_line, "%5d|%10s", row_num, buffer);
        row_num++;
        sleep(1);

        lcd->write(std::string(row_line));
    }

    lcd->displayOff();
    printf("Off for 3 seconds\n");
    sleep(3);
    delete lcd;
//! [Interesting]
    return 0;
}
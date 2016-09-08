#include "jhd1313m1.h"
#include <sys/poll.h>

#define RGB_ADDRESS 0x62
#define LCD_ADDRESS 0x3E

int main(int argc, char **argv)
{
//! [Interesting]
    // 0x62 RGB_ADDRESS, 0x3E LCD_ADDRESS
    upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0, 0x3E, 0x62);
    // mraa::Result x = lcd->createChar((uint8_t) 0, (uint8_t*) lenface1);
    lcd->displayOn();
    lcd->cursorOn();
    lcd->cursorBlinkOn();
    lcd->setCursor(0,0);
    sleep(2);
    // lcd->displayOff();
    lcd->write("The lonely night");
    lcd->setCursor(1,0);
    lcd->write("is still young.");
    sleep(2);
    lcd->autoscrollOn();
    printf("Sleeping for 5 seconds\n");
    sleep(5);
    lcd->displayOn();
    // printf("Off for 5 seconds\n");
    sleep(5);
    delete lcd;
//! [Interesting]
    return 0;
}
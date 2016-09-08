#include <unistd.h>

#include "mraa.hpp"

static volatile int counter = 0;
static volatile int oldcounter = 0;

void
interrupt(void* args)
{
    if(((mraa::Gpio*) args)->read() == 0)
        return;

    ++counter;
    printf("INTERRUPT\n");
}

int
main()
{
    mraa::Gpio* x = new mraa::Gpio(6);

    x->dir(mraa::DIR_IN);

    x->isr(mraa::EDGE_BOTH, &interrupt, x);

    for (;counter < 10;) {
        if (counter != oldcounter) {
            fprintf(stdout, "timeout counter == %d\n", counter);
            oldcounter = counter;
        }
        // got to relieve our poor CPU!
        sleep(1);
    }

    return EXIT_SUCCESS;
}
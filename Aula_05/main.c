#include "loop.h"
#include "setup.h"

int main(void)
{
    setup();
    loop();
    while(1);
}

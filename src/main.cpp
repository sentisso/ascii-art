#include <iostream>
#include "classes/Controller/Controller.h"

using namespace std;

int main(void)
{
    // support unicode
    setlocale(LC_ALL, "");

    // https://man7.org/linux/man-pages/man3/curs_variables.3x.html
    ESCDELAY = 10;

    Controller controller = Controller();

    return EXIT_SUCCESS;
}
//
// Created by asd on 8.5.22.
//

#include "Windows.h"

size_t window::mid_x()
{
    return round(x / 2);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t window::mid_y()
{
    return round(y / 2);
}

// ---------------------------------------------------------------------------------------------------------------------

window& window::operator=(WINDOW* new_win)
{
    win = new_win;
    return *this;
}
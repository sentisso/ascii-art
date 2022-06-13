//
// Created by asd on 8.5.22.
//

#ifndef ASCII_WINDOWS_H
#define ASCII_WINDOWS_H

#include <string>
#include <ncurses.h>
#include <math.h>

/**
 * Wrapper for an ncurses window storing useful information about the window, which can be later used for rendering. 
 */
struct window
{
    WINDOW * win = nullptr;
    /**
     * Current width of this window - getmaxx(win).
     */
    size_t x = 0;
    /**
     * Current height of this window - getmaxy(win).
     */
    size_t y = 0;
    
    /**
     * If this window is "active" and should be created.
     */
    bool active = true;

    /**
     * Useful for horizontally centering stuff.
     * @returns Rounded middle value of X.
     */
    size_t mid_x();

    /**
     * Useful for vertically centering stuff.
     * @returns Rounded middle value of Y.
     */
    size_t mid_y();

    window& operator = (WINDOW * new_win);
};

/**
 * All ncurses windows used.
 */
struct windows
{
    /**
     * Top 2-line window used for printing the images' path, animation speed etc.
     */
    window top;
    
    /**
     * This is the largest "main" window where the active image is usually rendered.
     */
    window main;

    /**
     * This window is used primarly for rendering image thumbnails and image adjustments.
     */
    window bottom;

    /**
     * Only the keymap gets rendered to this window. Which is located at the very bottom of the terminal.
     */
    window keymap;

    windows() {}
};


#endif //ASCII_WINDOWS_H

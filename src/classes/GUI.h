//
// Created by asd on 7.5.22.
//

#ifndef ASCII_GUI_H
#define ASCII_GUI_H

#define ERROR_BG    8
#define GREEN_BG    9
#define GREEN_FG    10

#define ctrl(x) ((x) & 0x1f)

#include <string>
#include <ncurses.h>
#include <math.h>
#include "Windows.h"
#include "Image/Image.h"
#include <unistd.h>
#include <vector>

/**
 * Class with some helper methods that help with rendering a unified GUI across the whole app.
 * Such as borders, menu, keymap etc.
 */
class GUI
{
    /**
     * All current ncurses windows.
     */
    windows _win;
    
    /**
     * What is the index of the last keymap that couldn't be rendered because it didn't fit into the screen?
     * This can be later used as a starting index when rendering "more" keymaps.
     */
    uint8_t _last_keymap = 0;
    std::vector<std::pair<std::string, std::string>> _keymap;

public:
    GUI();
    ~GUI();
    
    /**
     * Default or initial definition of the pixel-to-ascii character gradient.\n
     * Pixel densitiy is at the start of the array, meaning [0] = most significant character, [size - 1] = transparent.
     */
    std::shared_ptr<std::string> m_ascii_gradient;

    /**
     * `_win` getter.
     * @return A reference to `this->_win`.
     */
    windows& get_win();

    /**
     * Destroys all currently initiated windows (clearing included) and re-creates them with a maximum available sizing.\n
     * Basically a fresh re-render (or a first render).
     */
    void rewin();

    /**
     * Simply clears a specified line.
     * @param y Y coordinate identifying which line to clear_gui.
     * @param cols How many columns should be cleared. If set to 0, then the window width will be used.
     */
    void clear_line(int y) const;

    void wclear_line(WINDOW* win, int y) const;

    /**
     * Clears the whole terminal window.
     * @param animate 0 = no animations, just a basic clear_gui.\n
     *                1 = top to bottom clear_gui with small delays.\n
     *                2 = top, bottom, ++top, --bottom...
     * @param ms How many milliseconds should each animation delay take.
     */
    void clear_gui(int animate = 0, int ms = 8);

    /**
     * Renders a given keymap at the bottom of the terminal window.
     * @param keymap[string] The actual keymap text (example "ENTER: load   q: quit").
     */
    void keymap(const std::vector<std::pair<std::string, std::string>> & keymap);
    
    void more_keymap();

    void less_keymap();

    /**
     * Hides/show the blinking cursor.
     * @param on
     */
    void cursor(bool on) const;

    /**
     * Determine if the terminal support colors and if yes then define color pairs.
     */
    void init_colors();
    
    void reset_colors(NCURSES_COLOR_T background);

    /**
     * Destroys a given ncurses window (clearing included).
     * @param win Pointer to the window that is supposed to be destroyed.
     */
    static void destroy_win(window & win);
    
private:
    void render_keymap(uint8_t last_keymap);
    void render_single_keymap(const std::string & key, const std::string & action, uint16_t & offset) const;
};


#endif //ASCII_GUI_H

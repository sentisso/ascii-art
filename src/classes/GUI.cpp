//
// Created by asd on 7.5.22.
//

#include <fstream>
#include "GUI.h"

// https://stackoverflow.com/questions/47686906/ncurses-init-color-has-no-effect
GUI::GUI() : m_ascii_gradient(new std::string("$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "))
{
    initscr();
    init_colors();
    keypad(stdscr, TRUE);
    nonl();
    raw();
    noecho();
    cursor(false);
    refresh();

    rewin();
}

// ---------------------------------------------------------------------------------------------------------------------

GUI::~GUI()
{
    cursor(true);

    if (_win.top.win != nullptr)
        GUI::destroy_win(_win.top);
    
    if (_win.main.win != nullptr)
        GUI::destroy_win(_win.main);

    if (_win.bottom.win != nullptr)
        GUI::destroy_win(_win.bottom);

    if (_win.keymap.win != nullptr)
        GUI::destroy_win(_win.keymap);

    endwin();
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::rewin()
{
    endwin();
    // Needs to be called after an endwin() so ncurses will initialize
    // itself with the new terminal dimensions.
    refresh();
    clear();

    if (_win.top.win != nullptr)
        GUI::destroy_win(_win.top);
    
    if (_win.main.win != nullptr)
        GUI::destroy_win(_win.main);
    
    if (_win.keymap.win != nullptr)
        GUI::destroy_win(_win.keymap);
    
    if (_win.bottom.win != nullptr)
        GUI::destroy_win(_win.bottom);

    int max_x = COLS;
    int max_y = LINES;

    _win.top.x = max_x;
    _win.top.y = 2;
    _win.main.x = max_x;
    _win.keymap.x = max_x;
    _win.keymap.y = 2;

    if (_win.bottom.active)
    {
        _win.bottom.x = max_x;

        _win.main.y = floor(max_y * 0.75) - _win.top.y - _win.keymap.y;
        _win.bottom.y = max_y - _win.top.y - _win.main.y - _win.keymap.y - 1;

        _win.bottom = newwin(_win.bottom.y + 1, _win.bottom.x, _win.top.y + _win.main.y, 0);
    }
    else
    {
        _win.main.y = max_y - _win.top.y - _win.keymap.y - 1;
    }

    // newwin(int height, int width, int starty, int startx)
    _win.top = newwin(_win.top.y, _win.top.x, 0, 0);
    _win.main = newwin(_win.main.y, _win.main.x, _win.top.y, 0);
    _win.keymap = newwin(_win.keymap.y, _win.keymap.x, _win.top.y + _win.main.y + _win.bottom.y + 1, 0);
    
    refresh();
}

// ---------------------------------------------------------------------------------------------------------------------

windows& GUI::get_win()
{
    return _win;
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::clear_line(int y) const
{
    move(y, 0);
    clrtoeol();
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::wclear_line(WINDOW* win, int y) const
{
    wmove(win, y, 0);
    wclrtoeol(win);
    wrefresh(win);
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::clear_gui(int animate, int ms)
{
    cursor(false);

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000L * ms; // 8ms

    int bottom = _win.main.y + _win.bottom.y + _win.keymap.y;
    int top = 0;
    int row = 0;
    int i = 0;

    switch (animate)
    {
        case 1:
            for (i = 0; i < bottom; ++i)
            {
                clear_line(i);
                nanosleep(&ts, NULL);
                refresh();
            }
            break;

        case 2:
            while (bottom != top)
            {
                clear_line(row);
                nanosleep(&ts, NULL);
                refresh();

                i++;
                if (i % 2 == 1) row = ++top;
                else row = bottom--;
            }
            break;

        default:
            clear_gui();
            refresh();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::keymap(const std::vector<std::pair<std::string, std::string>> & keymap)
{
    _keymap = keymap;

    render_keymap(0);
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::more_keymap()
{
    render_keymap(_last_keymap);
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::less_keymap()
{
    render_keymap(0);
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::render_keymap(uint8_t last_keymap)
{
    if (last_keymap >= _keymap.size())
        return;

    wclear(_win.keymap.win);
    
    // print the "=" delimiter
    std::string line;
    for (size_t i = 0; i < _win.main.x; ++i)
        line.push_back('=');

    mvwaddstr(_win.keymap.win, 0, 0, line.c_str());

    std::pair<std::string, std::string> more({"F3", "->"});
    uint16_t key_more_len = more.first.length() + 1 + more.second.length() + 2;

    uint16_t offset = 0;
    
    // if more keymaps are shown, then render the "F2 less"
    if (last_keymap > 0)
        render_single_keymap("F2", "<-", offset);
    
    for (size_t i = last_keymap; i < _keymap.size(); ++i)
    {
        // if this keymap doesn't fit into the screen...
        if (offset + _keymap[i].first.length() + 1 + _keymap[i].second.length() > _win.main.x)
        {
            render_single_keymap(more.first, more.second, offset);
            _last_keymap = i;
            break;
        }

        render_single_keymap(_keymap[i].first, _keymap[i].second, offset);
        
        // if the next next keymap + the "F2 more" don't fit into the screen...
        if (
            i + 2 < _keymap.size() && 
            offset + _keymap[i + 1].first.length() + 1 + _keymap[i + 1].second.length() + 3 + key_more_len > _win.main.x
        )
        {
            // ...then print the "F2 more" a break it here
            render_single_keymap(more.first, more.second, offset);
            _last_keymap = i + 1;
            break;
        }
    }

    wrefresh(_win.keymap.win);
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::render_single_keymap(const std::string& key, const std::string& action, uint16_t & offset) const
{
    mvwaddstr(_win.keymap.win, 1, offset, key.c_str());
    offset += key.length() + 1;

    wattron(_win.keymap.win, COLOR_PAIR(GREEN_BG));
    mvwaddstr(_win.keymap.win, 1, offset, action.c_str());
    wattroff(_win.keymap.win, COLOR_PAIR(GREEN_BG));
    offset += action.length() + 3;
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::cursor(bool on) const
{
    if (on) curs_set(1);
    else curs_set(0);
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::init_colors()
{
    if (!has_colors()) return;

    start_color();
    use_default_colors();
    clear();

    reset_colors(-1);
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::reset_colors(NCURSES_COLOR_T background)
{
    // https://www.ditig.com/256-colors-cheat-sheet
    if (COLORS == 256)
        for (NCURSES_COLOR_T i = 16; i < COLORS; ++i)
            init_pair(i, i, background);
    
    // init_pair(index, foreground, background);
    if (COLORS == 256)
    {
        init_pair(GREEN_BG, COLOR_BLACK, 14);
        init_pair(GREEN_FG, 14, -1);
    }
    else
    {
        init_pair(GREEN_BG, COLOR_BLACK, COLOR_GREEN);
        init_pair(GREEN_FG, COLOR_GREEN, COLOR_BLACK);
    }
    
    init_pair(ERROR_BG, COLOR_BLACK, COLOR_RED);
    refresh();
}

// ---------------------------------------------------------------------------------------------------------------------

void GUI::destroy_win(window & win)
{
    wclear(win.win);
    wrefresh(win.win);
    delwin(win.win);
    
    win.x = 0;
    win.y = 0;
    win.win = nullptr;
}
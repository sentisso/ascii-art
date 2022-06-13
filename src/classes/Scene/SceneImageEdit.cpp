//
// Created by asd on 6.5.22.
//

#include "SceneImageEdit.h"
#include "../Controller/Controller.h"

void SceneImageEdit::render()
{
    _input_mode = false;

    _bottom_win_active_state = _win.bottom.active;

    // if the bottom window was hidden, then show it again, cause this scene renders the settings into it
    if (!_win.bottom.active)
    {
        _win.bottom.active = true;
        _gui.rewin();
    }

    _gui.cursor(false);

    render_keymap();

    _controller.m_scenes.gallery.get_active_image()->render(_win.main);

    render_settings();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::render_keymap()
{
    std::vector<std::pair<std::string, std::string>> keymap{
            {"s",             "save"},
            {"</>",           "change values"},
            {"\u2BC5/\u2BC6", "switch settings"}
    };

    if (COLORS == 256)
    {
        Image* img = _controller.m_scenes.gallery.get_active_image();

        if (img->m_adjustments.colors_enabled)
        {
            keymap.push_back({"c", "disable colors"});
            
            if (!img->m_adjustments.hdr)
                keymap.push_back({"h", "HDR on"});
            else
                keymap.push_back({"h", "HDR off"});
            
            keymap.push_back({"b", "change background"});
        }
        else
            keymap.push_back({"c", "enable colors"});
    }

    if (_saved)
        keymap.push_back({"ESC", "exit"});
    else
        keymap.push_back({"ESC", "exit without saving"});

    _gui.keymap(keymap);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::render_settings()
{
    Image* img = _controller.m_scenes.gallery.get_active_image();
    wclear(_win.bottom.win);
    
    uint8_t offset = 0;
    for (uint8_t i = 0; i < _settings_total; ++i)
    {
        if (i == _active_settings) wattron(_win.bottom.win, COLOR_PAIR(GREEN_BG));
        
        std::string row;
        std::string bar;
        switch (_settings[i])
        {
            case brightness:
                row = " brightness ";
                bar = generate_bar_adjust(-256, 256, img->m_adjustments.brightness);

                if (bar.length() + row.length() <= _win.main.x)
                    mvwaddstr(_win.bottom.win, i + 1 + offset, 0, (row + bar).c_str());
                else
                {
                    mvwaddstr(_win.bottom.win, i + 1 + offset++, 0, row.c_str());
                    mvwaddstr(_win.bottom.win, i + 1 + offset, 0, (" " + bar).c_str());
                }
                break;

            case contrast:
                row = " contrast   ";
                bar = generate_bar_adjust(-256, 256, img->m_adjustments.contrast);

                if (bar.length() + row.length() <= _win.main.x)
                    mvwaddstr(_win.bottom.win, i + 1 + offset, 0, (row + bar).c_str());
                else
                {
                    mvwaddstr(_win.bottom.win, i + 1 + offset++, 0, row.c_str());
                    mvwaddstr(_win.bottom.win, i + 1 + offset, 0, (" " + bar).c_str());
                }
                break;

            case inverted:
                row = " inverted   ";
                if (img->m_adjustments.inverted) row += "Y ";
                else row += "N ";

                mvwaddstr(_win.bottom.win, i + 1 + offset, 0, row.c_str());
                break;
                
            case stretch:
                row = " vertically_stretch    " + std::to_string(img->m_adjustments.stretch) + "% ";
                mvwaddstr(_win.bottom.win, i + 1 + offset, 0, row.c_str());
                break;
        }

        if (i == _active_settings) wattroff(_win.bottom.win, COLOR_PAIR(GREEN_BG));
    }

    wrefresh(_win.bottom.win);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string SceneImageEdit::generate_bar_adjust(int16_t min, int16_t max, int16_t value)
{
    std::string bar;
    std::string str_value = " " + std::to_string(value) + " ";

    // how long that bar will be
    size_t len = abs(min - max) / _adjust_unit + 1;

    // if the bar overflows, then make it smaller 2 times
    if (len + str_value.length() > _win.main.x)
        _adjust_unit *= 2;
    
    // if a bar two times the current size fits, then make it bigger 2 times
    else if (len * 2 + str_value.length() <= _win.main.x && _adjust_unit / 2 >= _min_adjust_unit)
        _adjust_unit /= 2;

    int16_t mid = abs(min - max) / 2 + min;
    
    for (int16_t i = min; i <= max; i += _adjust_unit)
    {
        if (i == value) bar += '#';
        // a subtle highlight of the middle value
        else if (i == mid) bar += "+";
        else bar += '-';
    }
    bar += str_value;
    
    return bar;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::adjust_image(int ch)
{
    Image* img = _controller.m_scenes.gallery.get_active_image();

    _saved = false;

    switch (_settings[_active_settings])
    {
        case brightness:
            if (ch == KEY_LEFT)
                img->m_adjustments.change_brightness(img->m_adjustments.brightness - _adjust_unit);
            else
                img->m_adjustments.change_brightness(img->m_adjustments.brightness + _adjust_unit);
            break;

        case contrast:
            if (ch == KEY_LEFT)
                img->m_adjustments.change_contrast(img->m_adjustments.contrast - _adjust_unit);
            else
                img->m_adjustments.change_contrast(img->m_adjustments.contrast + _adjust_unit);
            break;

        case inverted:
            img->m_adjustments.change_invert(!img->m_adjustments.inverted);
            break;
            
        case stretch:
            if (ch == KEY_LEFT)
                img->m_adjustments.change_stretch(img->m_adjustments.stretch - 1);
            else
                img->m_adjustments.change_stretch(img->m_adjustments.stretch + 1);
            break;
    }

    render_settings();
    img->render(_win.main);
    render_keymap();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::save_adjustments()
{
    if (!_saved)
    {
        Image* img = _controller.m_scenes.gallery.get_active_image();
        _last_saved = img->m_adjustments;
    }

    _saved = true;
    render_keymap();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::exit()
{
    Image* img = _controller.m_scenes.gallery.get_active_image();

    if (!_saved)
    {
        img->m_adjustments = _last_saved;
    }

    // if the bottom window was hidden when this scene got first rendered, then hide it again
    if (!_bottom_win_active_state)
    {
        _win.bottom.active = false;
        _gui.rewin();
    }

    _controller.change_scene(&_controller.m_scenes.gallery);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::input(int ch)
{
    switch (ch)
    {
        case 's':
            save_adjustments();
            break;

        case KEY_UP:
        case KEY_DOWN:
            change_settings(ch);
            break;

        case KEY_LEFT:
        case KEY_RIGHT:
            adjust_image(ch);
            break;

        case 'c':
            toggle_colors();
            break;

        case 'h':
            toggle_hdr();
            break;

        case 'b':
            toggle_background();
            break;

        case 27:
            exit();
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::toggle_colors()
{
    Image* img = _controller.m_scenes.gallery.get_active_image();
    if (COLORS == 256 && img != nullptr)
    {
        img->m_adjustments.colors_enabled = !img->m_adjustments.colors_enabled;
        img->render(_win.main);
        render_keymap();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::toggle_hdr()
{
    Image* img = _controller.m_scenes.gallery.get_active_image();
    if (COLORS == 256 && img != nullptr)
    {
        img->m_adjustments.hdr = !img->m_adjustments.hdr;
        img->render(_win.main);
        render_keymap();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::toggle_background()
{
    if (_controller.m_scenes.gallery.change_background())
    {
        _controller.m_scenes.gallery.get_active_image()->render(_win.main);
        render_settings();
        render_keymap();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageEdit::change_settings(int ch)
{
    if (ch == KEY_UP)
    {
        if (_active_settings == 0)
            _active_settings = _settings_total - 1;
        else
            _active_settings--;
    }
    else
    {
        if (_active_settings == _settings_total - 1)
            _active_settings = 0;
        else
            _active_settings++;
    }

    render_settings();
}
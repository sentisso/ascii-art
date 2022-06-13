//
// Created by asd on 6.5.22.
//

#include "SceneGallery.h"
#include "../Controller/Controller.h"

void SceneGallery::render()
{
    _input_mode = false;

    _gui.cursor(false);
    
    render_keymap();

    render_thumbnails();
    
    render_top();

    Image * img = get_active_image();
    if (img != nullptr)
    {
        img->render(_win.main);
    }
    else
    {
        wclear(_win.main.win);
        wrefresh(_win.main.win);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::render_keymap()
{
    std::vector<std::pair<std::string, std::string>> keymap{
            {"o", "open"},
    };
    
    Image * img = get_active_image();
    if (img != nullptr)
    {
        if (!_animation_running)
        {
            keymap.push_back({"e", "edit"});
            keymap.push_back({"</>", "prev/next"});
            keymap.push_back({"j/l", "move left/right"});
        }

        if (_win.bottom.active)
            keymap.push_back({"t", "hide thumbnails"});
        else
            keymap.push_back({"t", "show thumbnails"});
    
        if (COLORS == 256 && !_animation_running)
        {
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

        // no reason for an animation if there's only one image
        if (_controller.m_images.size() > 1)
        {
            if (_animation_running)
            {
                keymap.push_back({"s", "stop animation"});
                keymap.push_back({"\u2BC5/\u2BC6", "+/- speed"});
            }
            else
                keymap.push_back({"s", "start animation"});
        }
    }
    
    keymap.push_back({"q", "quit"});

    _gui.keymap(keymap);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::render_thumbnails()
{
    if (!_win.bottom.active) return;
    
    // TODO: fix responsivness lol

    wclear(_win.bottom.win);

    uint16_t i = 0;
    uint32_t offset = 0;
    for (auto& img: _controller.m_images)
    {
        img->to_ascii(_win.bottom.x + 1, _win.bottom.y + 1, img->m_bmp_thumbnail);

        for (uint32_t y = 0; y < img->m_bmp_thumbnail.height; ++y)
            for (uint32_t x = 0; x < img->m_bmp_thumbnail.width; ++x)
                mvwaddch(_win.bottom.win, y, x + offset, img->m_bmp_thumbnail.amp[y][x].first);

        // if this image is the active one, then highlight it with a border
        if (i == m_active_image)
        {
            wattron(_win.bottom.win, COLOR_PAIR(GREEN_FG));
            // top border
            mvwhline(_win.bottom.win, 0, offset, ACS_HLINE, img->m_bmp_thumbnail.width);
            // bottom border
            mvwhline(_win.bottom.win, img->m_bmp_thumbnail.height - 1, offset, ACS_HLINE, img->m_bmp_thumbnail.width);
            // left border
            mvwvline(_win.bottom.win, 1, offset, ACS_VLINE, img->m_bmp_thumbnail.height - 2);
            // right border
            mvwvline(_win.bottom.win, 1, offset + img->m_bmp_thumbnail.width, ACS_VLINE,
                     img->m_bmp_thumbnail.height - 2);
            // upper left corder
            mvwaddch(_win.bottom.win, 0, offset, ACS_ULCORNER);
            // upper right corner
            mvwaddch(_win.bottom.win, 0, offset + img->m_bmp_thumbnail.width, ACS_URCORNER);
            // lower left corner
            mvwaddch(_win.bottom.win, img->m_bmp_thumbnail.height - 1, offset, ACS_LLCORNER);
            // lower right corner
            mvwaddch(_win.bottom.win, img->m_bmp_thumbnail.height - 1, offset + img->m_bmp_thumbnail.width,
                     ACS_LRCORNER);
            wattroff(_win.bottom.win, COLOR_PAIR(GREEN_FG));
        }

        offset += img->m_bmp_thumbnail.width + 2;
        i++;
    }

    wrefresh(_win.bottom.win);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::render_top()
{
    wclear(_win.top.win);

    // render image order "2/5"
    std::string order = std::to_string(m_active_image + (_controller.m_images.size() > 0 ? 1 : 0)) + "/" +
                      std::to_string(_controller.m_images.size()) + " ";
    mvwaddstr(_win.top.win, 0, 0, order.c_str());

    std::string time;
    if (_animation_running)
    {
        time = " " + std::to_string(_animation_timeout / 1000) + "s";
        mvwaddstr(_win.top.win, 0, _win.top.x - time.length(), time.c_str());
    }
    
    // render image path
    Image * img = get_active_image();
    if (img != nullptr)
    {
        // if the path overflows when it's centered
        if (img->m_path.length() / 2 + order.length() > _win.top.mid_x() || img->m_path.length() / 2 + time.length() > _win.top.mid_x())
        {
            if (_animation_running)
            {
                size_t ov = img->m_path.length() / 2 - _win.top.mid_x();
                if (order.length() > time.length())
                    ov += order.length();
                else
                    ov += time.length();
                
                if (ov < img->m_path.length())
                {
                    std::string path(img->m_path.begin() + ov, img->m_path.end());
        
                    for (int i = 0; i < 3; ++i)
                        path[i] = '.';
                    
                    mvwaddstr(_win.top.win, 0, _win.top.mid_x() - path.length() / 2, path.c_str());
                }
            }
            else
            {
                // allign the path to the right
                // if it overflows the whole screen
                if (img->m_path.length() + order.length() > _win.top.x)
                {
                    size_t ov = img->m_path.length() + order.length() - _win.top.x;

                    std::string path(img->m_path.begin() + ov, img->m_path.end());

                    for (int i = 0; i < 3; ++i)
                        path[i] = '.';

                    mvwaddstr(_win.top.win, 0, _win.top.x - path.length(), path.c_str());
                }
                else
                    mvwaddstr(_win.top.win, 0, _win.top.x - img->m_path.length(), img->m_path.c_str());
            }
        }
        else
            mvwaddstr(_win.top.win, 0, _win.top.mid_x() - img->m_path.length() / 2, img->m_path.c_str());
        
    }
    
    wrefresh(_win.top.win);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::input(int ch)
{
    // these controls are available to the user independent of the animation's running state
    switch (ch)
    {
        case 's':
            start_stop_animation();
            break;

        case 't':
            // show/hide the thumbnails
            _win.bottom.active = !_win.bottom.active;
            _gui.rewin();
            render();
            break;

        case 'q':
            _controller.quit();
            break;

        case ERR:
            input_timeout();
            break;
    }
    
    if (_animation_running) return;
    
    // these controls are available to the user only while the animation is NOT running
    switch (ch)
    {
        case 'o':
            _controller.change_scene(&_controller.m_scenes.image_upload);
            break;

        case 'e':
            if (_controller.m_images.size() > 0)
                _controller.change_scene(&_controller.m_scenes.image_edit);
            break;

        case KEY_LEFT:
            change_image(m_active_image - 1);
            break;

        case KEY_RIGHT:
            change_image(m_active_image + 1);
            break;

        case 'j':
            move_active_image(true);
            render_thumbnails();
            break;

        case 'l':
            move_active_image(false);
            render_thumbnails();
            break;

        case 'c':
            toggle_colors();
            break;
            
        case 'h':
            toggle_hdr();
            break;

        case 'b':
            if (change_background())
                render();
            break;
            
        case 'd':
            if (_controller.m_images.size() == 0) return;
            _controller.change_scene(&_controller.m_scenes.image_delete);
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::change_image(uint16_t index)
{
    if (_controller.m_images.size() > index)
    {
        m_active_image = index;
        render_thumbnails();
        render_top();
        get_active_image()->render(_win.main);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::start_stop_animation()
{
    if (_controller.m_images.size() <= 1) return;

    // if the timeout was set, then stop the animation
    if (_animation_running)
    {
        _animation_running = false;
        _animation_timeout = -1;
        _controller.set_timeout(_animation_timeout);
    }
    else
    {
        // else start the animation
        change_image(0);

        _animation_running = true;
        
        _animation_timeout = 2000;
        _controller.set_timeout(_animation_timeout);
    }

    render_keymap();
    render_top();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::input_timeout()
{
    // if the last image is shown rn
    if (m_active_image + 1 == _controller.m_images.size())
    {
        start_stop_animation();
        return;
    }

    change_image(m_active_image + 1);
    render_thumbnails();
}

// ---------------------------------------------------------------------------------------------------------------------

bool SceneGallery::change_background()
{
    Image* img = get_active_image();
    
    if (COLORS == 256 && img != nullptr && img->m_adjustments.colors_enabled)
    {
        switch (_background)
        {
            case -1:
                _background = 232;
                break;
            case 232:
                _background = 231;
                break;
            case 231:
                _background = -1;
                break;
        }

        _gui.reset_colors(_background);

        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::toggle_colors()
{
    Image* img = get_active_image();
    
    if (COLORS == 256 && img != nullptr)
    {
        img->m_adjustments.colors_enabled = !img->m_adjustments.colors_enabled;

        img->render(_win.main);
        render_keymap();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::toggle_hdr()
{
    Image* img = get_active_image();
    if (COLORS == 256 && img != nullptr)
    {
        img->m_adjustments.hdr = !img->m_adjustments.hdr;
        
        img->render(_win.main);
        render_keymap();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneGallery::move_active_image(bool left)
{
    if (_controller.m_images.size() == 0) return;
    
    if (left)
    {
        if (m_active_image == 0) return;
        std::swap(_controller.m_images[m_active_image - 1], _controller.m_images[m_active_image]);
        m_active_image--;
    }
    else
    {
        if (m_active_image == _controller.m_images.size() - 1) return;
        std::swap(_controller.m_images[m_active_image + 1], _controller.m_images[m_active_image]);
        m_active_image++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Image* SceneGallery::get_active_image()
{
    if (_controller.m_images.size() > 0 && m_active_image < _controller.m_images.size())
        return _controller.m_images[m_active_image].get();
    
    return nullptr;
}
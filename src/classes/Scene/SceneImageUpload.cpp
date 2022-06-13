//
// Created by asd on 6.5.22.
//

#include "SceneImageUpload.h"
#include "../Controller/Controller.h"

void SceneImageUpload::render()
{
    _gui.keymap({
        {"ENTER", "load"},
        {"ESC", "cancel"}
    });

    int mid_y = _win.main.mid_y();
    int mid_x = _win.main.mid_x();

    // clear the space around the input
    _gui.clear_line(mid_y - 1); // padding
    _gui.clear_line(mid_y); // input label
    _gui.clear_line(mid_y + 1);
    _gui.clear_line(mid_y + 2); // input
    _gui.clear_line(mid_y + 3); // padding

    // render the input
    mvaddstr(mid_y, mid_x - 22, "Enter a new path to your image (.jpg, .png)");
    move(mid_y + 2, mid_x);
    _gui.cursor(true);
    _input_mode = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageUpload::input(int ch)
{
    if (!_input_mode) return;

    int mid_x = _win.main.mid_x();
    int mid_y = _win.main.mid_y() + 2; // line where the path will be printed

    switch (ch)
    {
        case KEY_BACKSPACE:
            if (_path.size() > 0)
            {
                _path.pop_back();
                _gui.clear_line(mid_y);
            }
            break;

        case '\r':
            // on regular enter
            if (try_load_image()) return;
            break;

        case 27:
            // reset the path, so the next image upload doesn't contain it
            _path = "";
            _controller.change_scene(&_controller.m_scenes.gallery);
            return;

        default:
            _path.push_back(ch);
    }

    // print the path in the middle of the screen
    mvaddstr(mid_y, mid_x - round(_path.size() / 2), _path.c_str());
}

// ---------------------------------------------------------------------------------------------------------------------

bool SceneImageUpload::try_load_image()
{
    // pause input
    _input_mode = false;

    std::shared_ptr<Image> img;

    // try to load the provided image
    try
    {
        img = Image::get_image(_path, _gui.m_ascii_gradient);

    } catch (std::invalid_argument& e)
    {
        // if there was an error, print it on the screen
        int mid_x = _win.main.mid_x();
        int mid_y = _win.main.mid_y() + 2;

        _gui.clear_line(mid_y + 2);
        attron(COLOR_PAIR(ERROR_BG));
        mvaddstr(mid_y + 2, mid_x - round(strlen(e.what()) / 2), e.what());
        attroff(COLOR_PAIR(ERROR_BG));

        // unpause input
        _input_mode = true;
        return false;
    }

    // push the successfully loaded image into the "gallery"
    // set the active image as the currently last one
    _controller.m_images.push_back(img);
    // reset the path, so the next image upload doesn't contain it
    _path = "";
    _controller.m_scenes.gallery.change_image(_controller.m_images.size() - 1);
    // load the next scene, which is a SceneGallery
    _controller.change_scene(&_controller.m_scenes.gallery);
    return true;
}
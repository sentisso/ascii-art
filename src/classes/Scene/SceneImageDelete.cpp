//
// Created by asd on 4.6.22.
//

#include "SceneImageDelete.h"
#include "../Controller/Controller.h"

void SceneImageDelete::render()
{
    _gui.keymap({
        {"y", "yes, delete"},
        {"n", "no, do not delete"}
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
    mvaddstr(mid_y, mid_x - 24, "Are you sure you want to delete this image (Y/N)?");
    move(mid_y + 2, mid_x);
    _gui.cursor(true);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageDelete::input(int ch)
{
    switch (ch)
    {
        case 'y':
            delete_image();
            break;
        
        case 'n':
            _controller.change_scene(&_controller.m_scenes.gallery);
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneImageDelete::delete_image()
{
    size_t active_image = _controller.m_scenes.gallery.m_active_image;

    _controller.m_images[active_image].reset();
    _controller.m_images.erase(_controller.m_images.begin() + active_image);

    // if the last image was just deleted, then set the previously second last image as active
    if (active_image > 0 && active_image == _controller.m_images.size())
        _controller.m_scenes.gallery.m_active_image--;

    _controller.change_scene(&_controller.m_scenes.gallery);
}
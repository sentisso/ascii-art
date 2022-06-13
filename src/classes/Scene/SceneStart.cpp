//
// Created by asd on 6.5.22.
//

#include <iostream>
#include "SceneStart.h"
#include "../../enums.h"
#include "../Image/Image.h"
#include "../Controller/Controller.h"

void SceneStart::render()
{
    int mid_y = _win.main.mid_y() + _win.top.y;
    int mid_x = _win.main.mid_x();

    // ASCII art. logo
    mvwaddstr(_win.main.win, 1, mid_x - 47, R"(_____/\\\\\\\\\________/\\\\\\\\\\\__________/\\\\\\\\\__/\\\\\\\\\\\__/\\\\\\\\\\\_)");
    mvwaddstr(_win.main.win, 2, mid_x - 47, R"( ___/\\\\\\\\\\\\\____/\\\/////////\\\_____/\\\////////__\/////\\\///__\/////\\\///__)");
    mvwaddstr(_win.main.win, 3, mid_x - 47,
             R"(  __/\\\/////////\\\__\//\\\______\///____/\\\/_______________\/\\\_________\/\\\_____)");
    mvwaddstr(_win.main.win, 4, mid_x - 47,
             R"(   _\/\\\_______\/\\\___\////\\\__________/\\\_________________\/\\\_________\/\\\_____)");
    mvwaddstr(_win.main.win, 5, mid_x - 47,
             R"(    _\/\\\\\\\\\\\\\\\______\////\\\______\/\\\_________________\/\\\_________\/\\\_____)");
    mvwaddstr(_win.main.win, 6, mid_x - 47,
             R"(     _\/\\\/////////\\\_________\////\\\___\//\\\________________\/\\\_________\/\\\_____)");
    mvwaddstr(_win.main.win, 7, mid_x - 47,
             R"(      _\/\\\_______\/\\\__/\\\______\//\\\___\///\\\______________\/\\\_________\/\\\_____)");
    mvwaddstr(_win.main.win, 8, mid_x - 47,
             R"(       _\/\\\_______\/\\\_\///\\\\\\\\\\\/______\////\\\\\\\\\__/\\\\\\\\\\\__/\\\\\\\\\\\_)");
    mvwaddstr(_win.main.win, 9, mid_x - 47,
             R"(        _\///________\///____\///////////___________\/////////__\///////////__\///////////__)");
    wattron(_win.main.win, COLOR_PAIR(GREEN_BG));
    mvwaddstr(_win.main.win, 11, mid_x - 1, " art. ");
    wattroff(_win.main.win, COLOR_PAIR(GREEN_BG));

    // render the input and move the cursor there
    std::string input_text = "Enter the path to your definition of the pixel-to-ASCII char gradient";
    
    if (_is_ascii_gradient_set)
    {
        // clear the past ascii gradient input (mid_y + 2), placeholder (mid_y) and hint (mid_y + 4)
        _gui.wclear_line(_win.main.win, mid_y);
        _gui.wclear_line(_win.main.win, mid_y + 2);
        _gui.wclear_line(_win.main.win, mid_y + 4);
        _gui.wclear_line(_win.main.win, mid_y + 6);
        _gui.wclear_line(_win.main.win, mid_y + 7);
        _gui.wclear_line(_win.main.win, mid_y + 9); // and the error line
        _path = "";

        input_text = "Enter the path to your first image (.jpg, .png)";
    }
    else
    {
        mvwaddstr(_win.main.win, mid_y + 4, mid_x - 38, "Where the most left character represents the brightest pixel (max 256 chars)");
        mvwaddstr(_win.main.win, mid_y + 6, mid_x - 31, "If the path is left blank, this default gradient will be used:");
        mvwaddstr(_win.main.win, mid_y + 7, mid_x - 35, _gui.m_ascii_gradient->c_str());
    }
    
    mvwaddstr(_win.main.win, mid_y, mid_x - round(input_text.size() / 2), input_text.c_str());
    _input_mode = true;

    move(mid_y + 2, mid_x);
    _gui.cursor(true);
    wrefresh(_win.main.win);
    
    _gui.keymap({
        {"ENTER", "load"},
        {"ESC", "quit"}
    });
    
    refresh();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneStart::input(int ch)
{
    if (!_input_mode) return;

    int mid_x = _win.main.mid_x();
    int mid_y = _win.main.mid_y() + _win.top.y + 2; // line where the path will be printed

    switch (ch)
    {
        case KEY_BACKSPACE:
            if (_path.size() > 0)
            {
                _path.pop_back();
            }
            break;

        // on ESC
        case 27:
            _controller.quit();
            break;

        case '\n':
        case '\r':
            // on regular enter
            if (!_is_ascii_gradient_set)
            {
                if (try_load_ascii_gradient()) render();
                else _input_mode = true;
            }
            else if (_controller.m_images.size() == 0)
            {
                if (try_load_image()) return;
                else _input_mode = true;
            }
            break;

        default:
            _path.push_back(ch);
    }

    // print the path in the middle of the screen
    _gui.wclear_line(_win.main.win, mid_y);
    mvwaddstr(_win.main.win, mid_y, mid_x - round(_path.size() / 2), _path.c_str());

    wrefresh(_win.main.win);
}

// ---------------------------------------------------------------------------------------------------------------------

bool SceneStart::try_load_image()
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
        print_error(e.what());
        return false;
    }

    // push the successfully loaded image into the "gallery"
    _controller.m_images.push_back(img);

    // load the next scene, which is a SceneGallery
    _controller.change_scene(&_controller.m_scenes.gallery);

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SceneStart::try_load_ascii_gradient()
{
    // pause input
    _input_mode = false;
    
    // the default ascii gradient defined in the GUI will be used
    if (_path.size() == 0) {
        _is_ascii_gradient_set = true;
        return true;
    }
    
    std::ifstream ifs(_path, std::ios::in);
    
    if (!ifs.is_open())
    {
        print_error("File cannot be opened or it does not exist.", 5);
        return false;
    }
    
    std::shared_ptr<std::string> buffer(new std::string);

    // try to read the first 256 characters, more aren't needed, because grayscale is only from 0-256.
    char ch;
    for (int i = 0; i < 257; ++i)
    {
        ifs.get(ch);
        if (ifs.eof()) break;
        if (ifs.bad())
        {
            print_error("An error occured while reading the file.", 5);
            return false;
        }
        
        if (i == 256)
        {
            print_error("ASCII gradient cannot exceed 256 characters.", 5);
            return false;
        }
        
        buffer->push_back(ch);
    }

    _is_ascii_gradient_set = true;
    
    // copy the gradient into the global one
    _gui.m_ascii_gradient = buffer;
    
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneStart::print_error(const std::string & err, int offset)
{
    int mid_x = _win.main.mid_x();
    int mid_y = _win.main.mid_y() + 4 + _win.top.y + offset;  // the error line

    _gui.wclear_line(_win.main.win, mid_y);
    wattron(_win.main.win, COLOR_PAIR(ERROR_BG));
    mvwaddstr(_win.main.win, mid_y, mid_x - round(err.size() / 2), err.c_str());
    wattroff(_win.main.win, COLOR_PAIR(ERROR_BG));

    wrefresh(_win.main.win);
}
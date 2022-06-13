//
// Created by asd on 6.5.22.
//

#ifndef ASCII_SCENE_H
#define ASCII_SCENE_H

#include <vector>
#include <cstring>
#include "../GUI.h"

// forward declaration bullshitery
class Controller;

/**
 * Abstract class for different scenes which get polymorphically rendered.
 */
class Scene
{
protected:
    /**
     * Reference to the global GUI.
     */
    GUI & _gui;
    windows & _win;

    Controller & _controller;

    /**
     * Some scenes include text inputs, this indicator can be useful when capturing inputs.
     * Eg. when a user is prompted to type, we don't want the program to quit when pressing "q" in the middle of the input.
     */
    bool _input_mode = false;

public:
    Scene(GUI & gui, Controller & controller);

    /**
     * Renders this specific scene to the terminal window via ncurses.
     */
    virtual void render() = 0;

    /**
     * Handles user input in a custom manner based on the scene and it's keymap.
     * @param ch Code for a key that was pressed.
     */
    virtual void input(int ch) = 0;
};


#endif //ASCII_SCENE_H

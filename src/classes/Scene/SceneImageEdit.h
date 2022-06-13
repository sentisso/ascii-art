//
// Created by asd on 6.5.22.
//

#ifndef ASCII_SCENEIMAGEEDIT_H
#define ASCII_SCENEIMAGEEDIT_H


#include "Scene.h"
#include "../../enums.h"
#include "../Image/ImageAdjustments.h"

/**
 * This is where the user can edit a currently active image.
 * This scene can be accessed by pressing "e" in the gallery.
 */
class SceneImageEdit : public Scene
{
    /**
     * What is the unit that is used for adjusting the settings.
     * @example if the adjust unit is 4, then the brightness can be adjusted by with an accuracy of 4.\n
     *          -4...0...4...8...12...16
     */
    int16_t _adjust_unit = 4;
    /**
     * What is the minimal allowed value of the adjust unit.
     */
    int16_t _min_adjust_unit = 2;
    
    /**
     * Which settings is currently being modified so the program knows which settings to change.
     */
    uint8_t _active_settings = 0;
    /**
     * All settings that are available to the user.
     * And this also defines the settings' order in which they are rendered in the GUI.
     */
    image_adjustments _settings[4] = {
        brightness, contrast, inverted, stretch
    };
    /**
     * How many settings are there in total (corresponds to _settings).
     */
    uint8_t _settings_total = 4;
    
    ImageAdjustments _last_saved;
    
    /**
     * Are the current changes saved?
     */
    bool _saved = true;
    
    /**
     * What was the state of the bottom window's active attribute when image edit 
     */
    bool _bottom_win_active_state = false;

public:
    SceneImageEdit(GUI & gui, Controller & controller) : Scene(gui, controller) {}

    void render() override;
    void input(int ch) override;
    
private:
    /**
     * Render this scene's keymap.\n
     * If 256 colors are supported then the enable/disable colors and change background options will be showed.
     */
    void render_keymap();
    
    /**
     * Render all of the available settings and their current values.
     * Such as sliding bar of brightness and contrast etc.
     */
    void render_settings();
    
    /**
     * Change the active settings based on the given arrow key (up/down).
     * This can later affect the behaviour of the adjust_image method.
     * @param ch int arrow key up or down.
     */
    void change_settings(int ch);
    
    /**
     * Adjust current image's settings based on the pressed arrow key and the active settings. 
     * @param ch int arrow key left or right. If it's left then the settings value will be lowered.
     */
    void adjust_image(int ch);
    
    void toggle_colors();
    
    void toggle_hdr();
    
    void toggle_background();
    
    /**
     * "save" adjustments by copying the image's current state to _last_saved.\n
     * This works because after the user exits, _last_saved gets copied into the images adjustments.
     */
    void save_adjustments();
    
    /**
     * Just copy the current _last_saved into the images adjustments and change the scene to gallery.
     */
    void exit();
    
    /**
     * Render an "adjustment bar" based on the min and max values and the current value.\n
     * Automatically the adjust unit gets multiplied by 2, if the bar wouldn't fit into the screen,
     * respectively if a bar 2 times the current size would fit into the screen the adjust unit gets divided by 2.
     * 
     * @note works best with values that are the power of 2.
     * @param min lowest value of the bar 
     * @param max greatest value of the bar
     * @param value current value
     */
    std::string generate_bar_adjust(int16_t min, int16_t max, int16_t value);
};


#endif //ASCII_SCENEIMAGEEDIT_H

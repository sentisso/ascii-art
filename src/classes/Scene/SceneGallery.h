//
// Created by asd on 6.5.22.
//

#ifndef ASCII_SCENEGALLERY_H
#define ASCII_SCENEGALLERY_H


#include "Scene.h"
#include "../Image/Image.h"

/**
 * This is where the user can browse the uploaded images.
 */
class SceneGallery : public Scene
{
    /**
     * Current background behind the main image.\n
     * -1 default background color (no background),\n
     * 0 black,\n
     * 1 white\n
     */
    int16_t _background = -1;
    
    bool _animation_running = false;
    int16_t _animation_timeout = -1;
    
public:
    SceneGallery(GUI & gui, Controller & controller) : Scene(gui, controller) {}
    
    /**
     * Index of a currently active image (the image that is shown on the main screen).
     */
    size_t m_active_image = 0;

    void render() override;
    void input(int ch) override;
    
    /**
     * Render the thumbnails in the bottom window, where the currently active image is highlighted
     */
    void render_thumbnails();

    /**
     * Render this scene's keymap.\n
     * If 256 colors are supported then the enable/disable colors and change background options will be showed.
     */
    void render_keymap();
    
    /**
     * Render the top window where the active image's path will be shown.
     */
    void render_top();

    /**
     * Change the _active_image index.
     * @param index
     */
    void change_image(uint16_t index);
    
    /**
     * Event handler for the background change ('b').
     * Cycle between the default color (-1), black and white.
     */
    bool change_background();
    
    /**
     * Toggle colorful rendering of the current ascii art.
     */
    void toggle_colors();
    
    /**
     * Turn the HDR on/off of the active image and rerender the image.
     */
    void toggle_hdr();
    
    void start_stop_animation();
    
    /**
     * Handle the ERR input. Which occurs when ncurses' timeout is set and when the timeout's delay passes.
     * @see https://linux.die.net/man/3/timeout
     */
    void input_timeout();
    
    /**
     * Returns a pointer to the currently active image.
     * @return If there are currently no Images in the m_images vector of the Controller, then nullptr is returned.
     */
    Image* get_active_image();
    
private:
    /**
     * Move the currently active image left or right in the controllers m_images.
     * @param left Should the image be moved left?
     */
    void move_active_image(bool left);
};


#endif //ASCII_SCENEGALLERY_H

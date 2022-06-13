//
// Created by asd on 6.5.22.
//

#ifndef ASCII_SCENESTART_H
#define ASCII_SCENESTART_H

#include <signal.h>
#include <memory>
#include <fstream>
#include "Scene.h"

/**
 * This is an initial scene where the user will be prompted to define the default ascii gradient and to upload the first image.
 */
class SceneStart : public Scene
{
    std::string _path;
    /**
     * A flag indicating whether or not the ascii gradient was defined and if the image path should be requested.
     */
    bool _is_ascii_gradient_set = false;

public:
    SceneStart(GUI & gui, Controller & controller) : Scene(gui, controller) {}

    void render() override;
    /**
     * Parse user's input as the image path.\n
     * On regular enter -> `try_load_image` is called, as in submitting the image path.
     * @param ch
     */
    void input(int ch) override;

private:

    /**
     * Try to load an Image object from the current `_path` and insert it into the global gallery.\n
     * In case of an error while parsing/reading/loading, the error will printed on the screen.
     */
    bool try_load_image();
    
    /**
     * Try to load the pixel-to-ascii character gradient from the current `_path`.\n
     * In case of an error while parsing/reading/loading, the error will printed on the screen.
     * @return true if the specified gradient is valid.
     */
    bool try_load_ascii_gradient();
    
    /**
     * TODO: probably virtualize?
     * Print a given error string onto the screen.
     * @param err Textual error :)
     * @param offset Sometimes the error needs to be printed a bit lower/higher.
     */
    void print_error(const std::string & err, int offset = 0);
};


#endif //ASCII_SCENESTART_H

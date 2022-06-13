//
// Created by asd on 6.5.22.
//

#ifndef ASCII_SCENEIMAGEUPLOAD_H
#define ASCII_SCENEIMAGEUPLOAD_H


#include "Scene.h"

/**
 * This scene is used for uploading a new image.
 * It has minimal UI - only the input gets rendered over the active image.
 */
class SceneImageUpload : public Scene
{
    std::string _path;

public:
    SceneImageUpload(GUI & gui, Controller & controller) : Scene(gui, controller) {}

    void render() override;
    void input(int ch) override;

private:

    /**
     * Try to load an Image object from the current `_path`.\n
     * In case of an error while parsing/reading/loading, the error will printed on the screen.
     */
    bool try_load_image();
};


#endif //ASCII_SCENEIMAGEUPLOAD_H

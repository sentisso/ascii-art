//
// Created by asd on 4.6.22.
//

#ifndef ASCII_SCENEIMAGEDELETE_H
#define ASCII_SCENEIMAGEDELETE_H

#include "Scene.h"

class SceneImageDelete : public Scene
{
public:
    SceneImageDelete(GUI & gui, Controller & controller) : Scene(gui, controller) {}
    
    void render() override;
    void input(int ch) override;
    
private:
    void delete_image();
};


#endif //ASCII_SCENEIMAGEDELETE_H

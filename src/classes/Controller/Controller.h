//
// Created by asd on 6.5.22.
//

#ifndef ASCII_CONTROLLER_H
#define ASCII_CONTROLLER_H

#include <signal.h>
#include <chrono>

#include "../Scene/Scene.h"
#include "../Scene/SceneGallery.h"
#include "../Scene/SceneImageUpload.h"
#include "../Scene/SceneStart.h"
#include "../Scene/SceneImageEdit.h"
#include "../Scene/SceneImageDelete.h"
#include "../GUI.h"
#include "../Image/Image.h"

/**
 * All scenes of this program, where each manages their own state, data etc.
 */
struct scenes
{
    /** This is where the user can edit a currently active image. */
    SceneImageEdit image_edit;
    /** This is an initial scene where the user will be prompted to define the default ascii gradient and to upload the first image. */
    SceneStart start;
    /** This is where the user can browse the uploaded images. */
    SceneGallery gallery;
    /** This scene is used for uploading a new image. */
    SceneImageUpload image_upload;
    SceneImageDelete image_delete;

    scenes(GUI& gui, Controller& controller) : image_edit(gui, controller),
                                               start(gui, controller), gallery(gui, controller),
                                               image_upload(gui, controller), image_delete(gui, controller)
    {}
};

/**
 * Class that manages the rendering and control of different GUI Scenes - main menu, image settings etc.\n
 * It provides an API for changing active scenes and for accessing different public members and methods of each scene.\n
 * It listens for key inputs by the user and passes them to the currently active scene, which then handles it by itself.
 */
class Controller
{
    /**
     * Currently active scene that handles the terminal output.
     */
    Scene* _active_scene;

    GUI _gui;

    bool _terminated = false;
    
    /** What is the current timeout's initial value. */
    int _timeout = 0;
    /** What was the actual value of the previous timestamp reset. */
    int _last_timeout = 0;
    /** What was the timestamp of the last timeout reset. */
    std::chrono::time_point<std::chrono::steady_clock> _last_timeout_timestamp;

    /**
     * Static member which holds a pointer to the one and only instance of this SceneController class.\n
     * Used mainly in the static method `static_signal_handler` which is used for signal handling (`signal.h`).\n
     * The captured signal is then passed to the non-static method `signal_handler`, which then handles it accordingly.
     */
    static Controller* _instance;

public:
    /**
     * All scenes of this program, where each manages their own state, data etc.
     */
    scenes m_scenes;

    /**
     * All currently loaded images.
     */
    std::vector<std::shared_ptr<Image>> m_images;

    Controller();

    /**
     * Changes the current scene to a new given one by calling the virtual method `render()` of the given scene.
     * @param new_scene The scene that should be rendered.
     */
    void change_scene(Scene* new_scene);
    
    /**
     * Set/reset current ncurses timeout.
     * @see https://linux.die.net/man/3/timeout
     * @param ms timeout delay in milliseconds.
     */
    void set_timeout(int ms);

    /**
     * Just quits the whole program in a nice way.
     * Terminates ncurses and then ends the `listen_for_input` while loop.
     */
    void quit();

private:
    /**
     * Initiates a non-blocking listener for user input.
     * Any input gets passed to the currently active scene, which handles it by itself.
     * @see https://jbwyatt.com/ncurses.html#input
     */
    void listen_for_input();
    
    /**
     * Synchronize the timeout (if set).\n
     * When an ncurses timeout is set for example to 2000ms (eg. at 00:00), then every 2s ncurses emits an ERR input
     * indicating that those 2s passed (at 00:02, 00:04, 00:06...).\n
     * But when the user presses any key in between those 2s (eg. at 00:01), then the timeout is "reset" and the ERR will
     * be emitted at 00:03, 00:05...\n
     * So when this happens the timeout needs to be set to 1000ms so the next ERR occurs at exactly 00:02 and not 00:03
     * and when it occurs at those 00:02 the timeout will be reset again to 2000ms.
     * @see https://linux.die.net/man/3/timeout
     * @param ch current input.
     */
    void synchronize_timeout(int ch);
    
    /**
     * Static method that is being passed to the signal handling function `signal()` from `<signal.h>`.
     * Which then calls the member method `singal_handler` and passes the signal to that function.
     * @param sig Captured signal flag.
     */
    static void static_signal_handler(int sig);

    /**
     * The actual signal handler which can work with member methods and attributes.
     * @param sig Captured signal flag.
     */
    void signal_handler(int sig);
};


#endif //ASCII_CONTROLLER_H

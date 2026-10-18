//##############################################################################
//## Project: ClAni ########################### Created by hury on 17.04.2020 ##
//##############################################################################
//## Author: Ryan Huth(hury) ############################# License: GNU GPLv3 ##
//##############################################################################

#include <widdershins.h>
#include <atomic>
#include <cstdio>
#include <sys/ioctl.h>

// set by a key press during the animation, or by a signal (main.cpp)
extern std::atomic<bool> quit;

namespace CliAniHury {

    Widdershins::Widdershins() {

        //printf("Created instance of Widdershins!\n");
    }

    Widdershins::~Widdershins() {

        //printf("Deleted instance of Widdershins!\n");
    }

    int Widdershins::exit() {
        _View->~CliView();
        _Engine.~GreaseMonkey();

        fflush(NULL);
        return 0;
    }

    void Widdershins::start() {
        // Sets up commandline for output
        int opmode = _View->init();

        if(opmode == 0) {
            _View->fastExit();
        }

        _View->runMainMenu();

        _Engine.setCharacters(_View->getVariety(), SYMBOL_VARIETY);

        // Scrollmode run function pointer assigned
        _RunFunc = scrollmode_run;

        // Preparing model and noise functions for operating
        _Engine.setup(opmode, _View->getSeed(), _View->getSelection());
    }

    void Widdershins::startDirect(int flavour, int effect, int seed, const char *chars) {
        // Straight into the animation, without the menu
        _View->initDirect();
        if (chars != nullptr) {
            _View->setVariety(chars);
        }

        _Engine.setCharacters(_View->getVariety(), SYMBOL_VARIETY);
        _RunFunc = scrollmode_run;

        selection select;
        select.option_s.taste = (short)flavour;
        select.option_s.effect = (short)effect;
        _Engine.setup(1, seed, select);
    }

    void performResize(CliView* view) {

        struct winsize size;
        if(ioctl(0, TIOCGWINSZ, (char *) &size) < 0)
            ; // TODO: Handle error message or such
        view->updateTermSize(size.ws_col, size.ws_row);
    }

    void Widdershins::runFrame(int time) {

        if (resizeTriggered) {
            // once per resize (upstream resized on every frame after the first)
            resizeTriggered = false;
            performResize(_View);
        }

        (*_RunFunc)(*_View, _Engine);

    }

    void scrollmode_run(CliView &view, GreaseMonkey &engine) {
        // Prints the newest line of characters at the top
        view.printAt(0, 0, engine.runIteration());
        // Waits for a specific amount of time; a key press ends the animation
        if (view.waiting(100))
            quit.store(true);
        // Removes the bottom most line on cli
        view.removeBottomLine();
    }

    void framemode_run(CliView &view, GreaseMonkey &engine) {


        view.waiting(50);
    }

    void Widdershins::scheduleResize() {
        resizeTriggered = true;
    }

} // end of namespace CliAniHury
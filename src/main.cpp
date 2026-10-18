//##############################################################################
//## Project: ClAni ########################### Created by hury on 29.01.2020 ##
//##############################################################################
//## Author: Ryan Huth(hury) ############################# License: GNU GPLv3 ##
//##############################################################################

#include <iostream>
#include <csignal>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <widdershins.h>
#include <utilz/hutime.h>

std::atomic<bool> quit(false);
CliAniHury::Widdershins* ctrl;

void got_signal(int signum) {
    if (signum == SIGWINCH)
        ctrl->scheduleResize();
    else
        quit.store(true);
}

static void usage(const char *name) {
    std::cout << "usage: " << name << " [-f flavour] [-e effect] [-s seed] [-c chars]\n"
              << "\n"
              << "Without options clani starts with its menu. Any option starts the\n"
              << "animation straight away (the rest at their defaults, the seed random).\n"
              << "\n"
              << "  -f, --flavour  perlin, voronoi (default) or cell\n"
              << "  -e, --effect   scrolling (default), raining, zigzag, zigzag-old or static\n"
              << "  -s, --seed     a number from 0 to 99999999\n"
              << "  -c, --chars    up to 10 characters from faint to dense (default .:\"=edD#B@)\n"
              << "  -h, --help     this text\n"
              << "\n"
              << "Any key, or Ctrl+C, ends the animation.\n";
}

static int lookup(const char *value, const char *const *names, int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(value, names[i]) == 0)
            return i;
    }
    return -1;
}


    int main(int argc, char* argv[]) {

        // Options that skip the menu (Asciiverse); same order as e_Flavour
        // and e_NoiseAttribute in utilz/global.h
        static const char *const flavours[] = {"perlin", "voronoi", "cell"};
        static const char *const effects[] = {"scrolling", "raining", "zigzag", "zigzag-old", "static"};
        bool direct = false;
        int flavour = FLAVOUR_VORONOI, effect = ATTRIBUTE_SCROLLING;
        int seed = (int)(time(NULL) % 100000000);
        const char *chars = nullptr;
        for (int i = 1; i < argc; i++) {
            const char *arg = argv[i];
            const char *value = i + 1 < argc ? argv[i + 1] : nullptr;
            if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
                usage(argv[0]);
                return 0;
            } else if (value && (!strcmp(arg, "-f") || !strcmp(arg, "--flavour"))) {
                flavour = lookup(value, flavours, 3);
            } else if (value && (!strcmp(arg, "-e") || !strcmp(arg, "--effect"))) {
                effect = lookup(value, effects, 5);
            } else if (value && (!strcmp(arg, "-s") || !strcmp(arg, "--seed"))) {
                char *end;
                long n = strtol(value, &end, 10);
                seed = (*end || n < 0 || n > 99999999) ? -1 : (int)n;
            } else if (value && (!strcmp(arg, "-c") || !strcmp(arg, "--chars"))) {
                chars = value;
            } else {
                usage(argv[0]);
                return 2;
            }
            if (flavour < 0 || effect < 0 || seed < 0) {
                usage(argv[0]);
                return 2;
            }
            direct = true;
            i++;
        }

        struct sigaction sigInt;
        // Binding Ctrl+C to got_signal() for normal shutdown
        sigInt.sa_handler = got_signal;
        sigemptyset(&sigInt.sa_mask);
        sigInt.sa_flags = 0;

        sigaction(SIGINT, &sigInt, NULL);
        sigaction(SIGTERM, &sigInt, NULL);
        sigaction(SIGHUP, &sigInt, NULL);
        sigaction(SIGWINCH, &sigInt, NULL);

        CliAniHury::CliView hr_view;
        ctrl = new CliAniHury::Widdershins();

        // Registers View-module at ctrl for ncurses usage
        ctrl->registerCliView(&hr_view);
        // Update the window size once in the beginning to fit the terminal
        ctrl->scheduleResize();
        // Starts program with requesting user input and preparing modules,
        // or straight into the animation when options were given
        if (direct)
            ctrl->startDirect(flavour, effect, seed, chars);
        else
            ctrl->start();

        // Timer for getting deltaTime between iterations
        CliAniHury::HuTime curTime;
        int deltaTime, lastTime = curTime.millisec();

        while (!quit) {
            int currentTime = curTime.millisec();
            deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            // runs a "frame"
            ctrl->runFrame(deltaTime);

        }
        ctrl->exit();

        // a normal end (upstream exited with status 2, the number of SIGINT)
        exit(0);
    }


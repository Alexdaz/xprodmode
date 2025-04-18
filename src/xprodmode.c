#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h> 
#include <time.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>

#include <X11/extensions/XTest.h>

#define MILLI_SEC 1000
#define MICRO_SEC 1000000

#define KEY_PRESS_INTERVAL 59
#define BUGGY_POINTER_INTERVAL 7

static Bool stopSignal = False;

struct option long_options[] = {
    { "help"   , no_argument, 0, 'h' },
    { "key"    , no_argument, 0, 'k' },
    { "buggy"  , no_argument, 0, 'b' },
    { "version", no_argument, 0, 'v' },
    { 0, 0, 0, 0 }
};

void psleep(int* ms)
{
    struct timespec req = {
        *ms / MILLI_SEC,
        (*ms % MILLI_SEC) * MICRO_SEC
    };
    nanosleep(&req, NULL);
}

void printHelp()
{
    fprintf(stderr,
    "Usage: [options]\n"
    "\n"
    "Options:\n"
    "  -h, --help    \t print this help message\n"
    "  -k, --key     \t press F15 key every 59 seconds\n"
    "  -b, --buggy   \t activate the buggy pointer\n"
    "  -v, --version \t print program version\n");
}

void signalHandler() 
{ 
    stopSignal = True;
    printf("\nBye!\n"); 
}

KeyCode getKeyCode(Display* d) 
{
    int minKeycode, maxKeycode;
    XDisplayKeycodes(d, &minKeycode, &maxKeycode);

    int fEmptykc = -1;

    for (int keycode = (minKeycode > 10 ? minKeycode : 11); keycode <= maxKeycode; keycode++)
    {
        KeySym keysym = XkbKeycodeToKeysym(d, keycode, 0, 0);

        if (keysym == NoSymbol) 
        {
            fEmptykc = keycode;
            break;
        }
    }

    if (fEmptykc != -1) 
    {
        return fEmptykc;
    } 
    else 
    {
        return NoSymbol;
    }
}

void keyPress(int msec, Display* d)
{
    KeySym noSym = NoSymbol;

    KeyCode keyCode = XKeysymToKeycode(d, XK_F15);

    if (keyCode == 0) 
    {
        KeyCode new_keycode = getKeyCode(d);

        KeySym f15Keysym = XK_F15;

        XChangeKeyboardMapping(d, new_keycode, 1, &f15Keysym, 1);
        XFlush(d);

        keyCode = new_keycode;
    }

    while (!stopSignal)
    {
        XTestFakeKeyEvent(d, keyCode, True, None);
        psleep(&msec);
        XTestFakeKeyEvent(d, keyCode, False, None);
        XFlush(d);
    }

    XTestFakeKeyEvent(d, keyCode, False, None);
    XChangeKeyboardMapping(d, keyCode, 1, &noSym, 1);

    XFlush(d);

    XCloseDisplay(d);
}

void buggyPointer(int msec, Window* w, Display* d)
{
    Screen *screen = ScreenOfDisplay(d, 0);

    while (!stopSignal)
    {
        while (!stopSignal) 
        {
            int x = rand() % screen->width - -screen->height;
            int y = rand() % -screen->width - screen->height;
 
            XWarpPointer(d, None, *w, 0, 0, 0, 0, x, y);
            psleep(&msec);
        }

        XFlush(d); 
    }

    XCloseDisplay(d);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler); 

    Display *disp = XOpenDisplay(NULL);

    Window root_window;

    if (disp == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(EXIT_FAILURE);
    }
    root_window = DefaultRootWindow(disp);

    int opt;

    while ((opt = getopt_long(argc, argv, "hkbv", long_options, 0)) != -1)
    {
        switch (opt) 
        {
            case 'h':
                printHelp();
                XCloseDisplay(disp);
                exit(EXIT_SUCCESS);
            case 'k':
                keyPress(KEY_PRESS_INTERVAL * MILLI_SEC, disp);
                exit(EXIT_SUCCESS);
                break;
            case 'b':
                buggyPointer(BUGGY_POINTER_INTERVAL, &root_window, disp);
                exit(EXIT_SUCCESS);
                break;
            case 'v':
                fprintf(stderr, "%s-%s\n", argv[0], VERSION);
                XCloseDisplay(disp);
                exit(EXIT_SUCCESS);
            case '?':
                printHelp();
                XCloseDisplay(disp);
                exit(EXIT_FAILURE);
            default:
                XCloseDisplay(disp);
                return EXIT_SUCCESS;
        }
    }

    XCloseDisplay(disp);

    return EXIT_SUCCESS;
}
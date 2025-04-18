#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h> 
#include <time.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <X11/extensions/XTest.h>

#define STOP_SIGNAL False
#define MILLI_SEC 1000
#define MICRO_SEC 1000000

#define KEY_PRESS_INTERVAL 59
#define BUGGY_POINTER_INTERVAL 7

typedef unsigned int uint;
typedef unsigned char tiny;

static Display *disp;

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
    "  -k, --key     \t press shift_l key every 59 seconds\n"
    "  -b, --buggy   \t activate the buggy pointer\n"
    "  -v, --version \t print program version\n");
}

void closeDisplay()
{
    if (disp) 
    {
        XCloseDisplay(disp);
    }
}

void signalHandler() 
{ 
    printf("\nBye!\n"); 
    XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, XK_Shift_L), False, None);
    closeDisplay();
    exit(EXIT_SUCCESS);
}

void keyPress(int msec)
{
    uint keyCode = XKeysymToKeycode(disp, XK_Shift_L);

    while (!STOP_SIGNAL)
    {
        XTestFakeKeyEvent(disp, keyCode, True, None);
        psleep(&msec);
        XTestFakeKeyEvent(disp, keyCode, False, None);
        XFlush(disp);
    }
}

void buggyPointer(int msec, Window* w)
{
    Screen *screen = ScreenOfDisplay(disp, 0);

    while (!STOP_SIGNAL)
    {
        while (!STOP_SIGNAL) 
        {
            int x = rand() % screen->width - -screen->height;
            int y = rand() % -screen->width - screen->height;
 
            XWarpPointer(disp, None, *w, 0, 0, 0, 0, x, y);
            psleep(&msec);
        }

        XFlush(disp); 
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler); 

    disp = XOpenDisplay(NULL);

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
                closeDisplay();
                exit(EXIT_SUCCESS);
            case 'k':
                keyPress(KEY_PRESS_INTERVAL * MILLI_SEC);
                break;
            case 'b':
                buggyPointer(BUGGY_POINTER_INTERVAL, &root_window);
                break;
            case 'v':
                fprintf(stderr, "%s-%s\n", argv[0], VERSION);
                closeDisplay();
                exit(EXIT_SUCCESS);
            case '?':
                printHelp();
                closeDisplay();
                exit(EXIT_FAILURE);
            default:
                closeDisplay();
                return EXIT_SUCCESS;
        }
    }

    closeDisplay();

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h> 

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <X11/extensions/XTest.h>

#define STOP_SIGNAL False
#define MILLI_SEC 1000
#define MICRO_SEC 1000000

typedef unsigned int uint;

static Display *disp;

struct option long_options[] = {
    { "help"   , no_argument, 0, 'h' },
    { "key"    , no_argument, 0, 'k' },
    { "buggy"  , no_argument, 0, 'b' },
    { "version", no_argument, 0, 'v' },
    { 0, 0, 0, 0 }
};

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
    XCloseDisplay(disp);
}

void signalHandler() 
{ 
    signal(SIGINT, signalHandler); 
    printf("\nBye!\n"); 
    fflush(stdout);
    XTestFakeKeyEvent(disp, XKeysymToKeycode(disp, XK_Shift_L), False, None);
    closeDisplay();
    exit(EXIT_SUCCESS);
}

void keyPress(int sec)
{
    uint keyCode = XKeysymToKeycode(disp, XK_Shift_L);

    while (!STOP_SIGNAL)
    {
        XTestFakeKeyEvent(disp, keyCode, True, None);
        usleep(sec * MICRO_SEC);
        XTestFakeKeyEvent(disp, keyCode, False, None);
        XFlush(disp);
    }
}

void buggyPointer(int sec, Window w)
{
    Screen *screen;
    XEvent event;

    screen = ScreenOfDisplay(disp, 0);

    while (!STOP_SIGNAL)
    {
        while (!STOP_SIGNAL) 
        {
            int x = rand() % screen->width - -screen->height;
            int y = rand() % -screen->width - screen->height;
 
            XWarpPointer(disp, None, w, 0, 0, 0, 0, x, y);
            usleep(sec * MILLI_SEC);
        }

        XFlush(disp); 
        XNextEvent(disp, &event);
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
                keyPress(59);
                break;
            case 'b':
                buggyPointer(7, root_window);
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
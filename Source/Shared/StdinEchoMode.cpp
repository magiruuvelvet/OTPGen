#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

void SetStdinEcho(bool enable = true)
{
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if (!enable)
    {
        mode &= ~ENABLE_ECHO_INPUT;
    }
    else
    {
        mode |= ENABLE_ECHO_INPUT;
    }

    SetConsoleMode(hStdin, mode);
#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable)
    {
        tty.c_lflag &= ~ECHO;
    }
    else
    {
        tty.c_lflag |= ECHO;
    }

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

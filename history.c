#include <stdio.h>
#include <time.h>
#include "history.h"

void log_activity(const char *account_num, const char *message)
{
    char filename[100];
    sprintf(filename, "logs/%s.txt", account_num);

    FILE *f = fopen(filename, "a");
    if (f)
    {
        time_t now = time(NULL);
        char *timestamp = ctime(&now);
        timestamp[24] = '\0'; // Usuwamy znak nowej linii z ctime

        fprintf(f, "[%s] %s\n", timestamp, message);
        fclose(f);
    }
}
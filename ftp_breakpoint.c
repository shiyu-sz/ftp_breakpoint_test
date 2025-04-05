#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

static void usage(void)
{
    printf("ftp_breakpoint [-h] \n");
    printf("  -h : 帮助 \n");
    printf("  -u : 上传 \n");
    printf("  -d : 下载 \n");
}

int main(int argc, char *argv[])
{
	int c;

    while ((c = getopt(argc, argv, "hu:d")) != -1) {
        switch (c) {
        case 'h':
            usage();
            break;
        case 'u':
            break;
        case 'd':
            break;
        default:
            printf("Error: invalid options. \n");
            usage();
            return -1;
        }
    }

	return 0;
}
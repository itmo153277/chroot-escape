/*
    chroot-escape
    Copyright (C) 2026  Viktor Ivanov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define USAGE "Usage: chroot-escape NEWROOT COMMAND [ARG]...\n\n" \
    "Run COMMAND with root directory set to NEWROOT that\n" \
    "can be outside of the current chroot sandbox\n\n" \
    "NEWROOT must be relative to the current root directory\n\n" \
    "Report bugs to <viktprog@gmail.com>\n"

void printUsage() {
    fputs(USAGE, stderr);
}

int execChroot(char *newRoot, char* command, char **argv) {
    if (chdir("/") != 0) {
        perror("Failed to change directory");
        return EXIT_FAILURE;
    }
    if (chroot("tmp") != 0) {
        perror("Failed to chroot to /tmp");
        return EXIT_FAILURE;
    }
    if (chdir(newRoot) != 0) {
        perror("Failed to change directory to the new root");
        return EXIT_FAILURE;
    }
    if (chroot(".") != 0) {
        perror("Failed to chroot to the new root");
        return EXIT_FAILURE;
    }
    if (execvp(command, argv) != 0) {
        fprintf(stderr, "Failed to execute command %s: %s\n", command,
            strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printUsage();
        return EXIT_FAILURE;
    }
    if (getuid() != 0) {
        fputs("This program must be run as root\n", stderr);
        return EXIT_FAILURE;
    }
    return execChroot(argv[1], argv[2], argv + 2);
}

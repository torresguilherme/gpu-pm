#include <cstdio>

int main(int argc, char** argv) {
    if(argc == 1) {
        printf("Usage: ./demo <scene file name>\n");
        return -1;
    }

    printf("%s\n", argv[1]);
}
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int M = 0, N, S = 1;
    switch (argc) {
        case 1:
            printf("help\n");
            return 0;
        case 2:
            N = atoi(argv[1]);
            break;
        case 3:
            M = atoi(argv[1]);
            N = atoi(argv[2]);
            break;
        case 4:
            M = atoi(argv[1]);
            N = atoi(argv[2]);
            S = atoi(argv[3]);
            break;
    }

    for (int i = M; i < N; i += S)
        printf("%d\n", i);
    return 0;
}

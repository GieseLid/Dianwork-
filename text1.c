#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int main(void)
{
    char A[100];
    while (1)
    {
        scanf("%s", A);

        if (strcmp(A, "Dian") == 0) {
            printf("2002\n");
        }
        else if (strcmp(A, "Quit") == 0) {
            // 可以选择 return 0; 结束程序
            return 0;
        }
        else {
            printf("Error\n");
        }
    }
    return 0;
}
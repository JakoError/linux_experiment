#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const int HEX = 16;
const int size = 4;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 0;
    }
    char *str = argv[1];
    u_int32_t length = strlen(str);
    length = length / 4 + ((length % 4) != 0);

    u_int16_t sum = 0;
    char *unit = (char *) malloc(sizeof(char) * (size + 1));
    for (int i = 0; i < length; ++i) {
        strncpy(unit, &str[(ptrdiff_t) (i * 4)], 4);
        u_int16_t data = (u_int16_t) strtoul(unit, NULL, HEX) << ((4 - strlen(unit)) * 4);
        bool carry = sum > (0xffff - data);
        sum += data + carry;
    }
    sum = ~sum;
    printf("%04x\n", sum);
    return 0;
}
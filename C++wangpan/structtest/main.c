#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct PDU {
    int a;
    int b;
    int c;
    int d[];
}PDU;

int main() {

    printf("%ld\n", sizeof(PDU));

    PDU *pdu = (PDU*)malloc(sizeof(PDU) + 100 * sizeof(int));
    pdu->a = 90;
    pdu->b = 89;
    pdu->c = 88;
    memcpy(pdu->d, "you jump i jump", 16);

    printf("a=%d,b=%d,c=%d,%s\n", pdu->a,pdu->b,pdu->c, (char*)(pdu->d));

    free(pdu);
    pdu = NULL;

    printf("Hello World!\n");
    return 0;
}

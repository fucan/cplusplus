#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BIG_INT_BIT_LEN 1024  //
#define SIGN_BIT  BIG_INT_BIT_LEN - 1 //
#define BUFFER_SIZE BIG_INT_BIT_LEN
#define POSITIVE 0
#define NEGATIVE 1

typedef struct 
{
  char value[BIG_INT_BIT_LEN];
  int len;
  int sign;
}Number;

void PrintBigInt(BigInt* a)
{
  int i;
  for(i = SIGN_BIT;i>=0;i--)
  {
    printf("%d\n", a->bit[i]);
  }
}

//
void PrintNumber(Number* n)
{
  int i;
  if(n->sign == NEGATIVE)
  {
    printf("-");
  }
  for(i=n->len-1;i>=0;i--)
  {
    if(n->value[i]>9)
    {
      printf("%c", n->value[i]-10+'a');
    }
    else
    {
      printf("%d", n->value[i]);
    }
    printf("\n");
  }
}


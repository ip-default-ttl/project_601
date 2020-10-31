#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>

int main()
{
  char input;
  system("clear");
  printf("%s\n", "[!] Warning: previous config file will be lost!");
  printf("%s\n", "Do you want to continue? [Y/N]");
  while (1)
  {
    scanf("%s", &input);
    if ((input=='Y')||(input=='y'))
    {
      break;
    }
    else
    {
      if ((input=='N')||(input=='n'))
      {
        printf("%s\n", "Recieved signal to abort!");
        return 1;
      }
      else
      {
        printf("%s\n", "Unrecognised input!");
        continue;
      }
    }
  }
  system("rm -rf .config");
  return 0;
}

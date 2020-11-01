#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>

int main()
{
  char input;
  FILE* fout;
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
  fout=fopen(".config","w");
  fprintf(fout,"%s\n","#This is default config file, please change parameters as you need");
  fprintf(fout,"%s\n","web_enable: 1");
  fprintf(fout,"%s\n","log_enable: 1");
  fprintf(fout,"%s\n","#Please remember all passwords, they will be deleted after first initialization!");
  fprintf(fout,"%s\n","username: admin");
  fprintf(fout,"%s\n","user_pass: admin");
  fprintf(fout,"%s\n","superuser: root");
  fprintf(fout,"%s\n","superuser_pass: toor");
  fprintf(fout,"%s\n","#MySQL config");
  fprintf(fout,"%s\n","SQL_ip: 127.0.0.1");
  fprintf(fout,"%s\n","SQL_user: root");
  fprintf(fout,"%s\n","SQL_pass: toor");
  return 0;
}

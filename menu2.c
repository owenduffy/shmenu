/* a program to aid simple menus in bat files */


#include <stdio.h>
#include <string.h>
#ifdef linux
#include <termios.h>
#endif
#ifdef _AIX32
#include <termio.h>
#endif

int rc,parmindx,selection=0,first_time;
char option,file_name[256]="menu.scr",options[256]="";
struct termio sioio,sioold;

/**********************************************************************/
int clear_screen()
{
printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
return 0;
}
/**********************************************************************/

display_file(char *file_name)
{
char menu[260];
FILE *fmenu;

if(fmenu=fopen(file_name,"r")){
  while(fgets(menu,sizeof(menu)-1,fmenu))
    fputs(menu,stdout);
  fclose(fmenu);
  }
else{
  if(file_name[0]){
    printf("ERROR: cant find menu file: %s\n",file_name);
  if(first_time)
    printf("Valid options are: {%s}",options);
    }
  }
return 0;
}
/**********************************************************************/

int prompt()
{
char *p;

display_file(file_name);
if(!first_time)
  printf("ERROR: option is not valid, choose one of {%s}",options);

printf("\nEnter option ===> ");
if(ioctl(1,TCGETA,&sioold)==-1){
  printf("ERROR: in call to ioctl() to save old tty setup\n");
  return -1;
  }
if(ioctl(1,TCGETA,&sioio)==-1){
  printf("ERROR: in call to ioctl() to copy tty setup\n");
  return -1;
  }
sioio.c_lflag=0;
sioio.c_cc[VMIN]=1;
sioio.c_cc[VTIME]=0;
ioctl(1,TCSETA,&sioio);
option=getchar();
ioctl(1,TCSETA,&sioold);

first_time=0;
if(option==0x1b)
  return -1;
if(p=strchr(options,option))
  return p-options+1;
else
  return 0;
}
/**********************************************************************/

int help()
{
printf("menu V1.00\n");
printf("Usage: menu options [filename]\n");
printf("COPYRIGHT: Owen Duffy and Associates - 1987\n\n");
return 0;
}
/**********************************************************************/

main(int argc,char **argv)
{
if(argc<2){
  help();
  return 0;
  }
else
{
  first_time=1;
  selection=0;
  strcpy(options,argv[1]);
  if(argc==3)
    strcpy(file_name,argv[2]);

  do{
    selection=prompt();
    if(selection==-1){
      selection=0;
      break;
      }
    }while(!selection);
    clear_screen();
    return selection;
}
return 0;
}
/**********************************************************************/


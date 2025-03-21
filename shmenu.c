/************************************************************************
shmenu: a shell script enhancer for menus
*************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <ctype.h>
#ifdef CURSES
#include <locale.h>
#include <curses.h>
#include <term.h>
#endif
#define NOOPT_NOEXIT -1
#define HELP_EXIT 125
#define ERROR_EXIT 124
char version[]=VERSION,file_name[256]="",work[256];
int debug=0,time_out=0,option,errorrow,promptrow,promptcol;
int rc,parmindx,selection=0,pass=0,upcase=0,basic=1;
char options[256]="",command='\0',promptstr[80]="Enter option ===>";
struct termio sioio,sioold;
int (*prompt)(void),(*clrscr)(void);
/************************************************************************/
#ifdef CURSES

int display_file_c(char *file_name)
{
  char menu[260];
  FILE *fmenu;
  int y=0;

  clear();
  move(0,0);
  if(fmenu=fopen(file_name,"r")){
    while(fgets(menu,sizeof(menu)-1,fmenu))
      addstr(menu);
    fclose(fmenu);
  }
  else{
    printw("\nWARNING: cannot find menu file: %s\n\n",file_name);
    printw("Valid options are: {%s}",options+1);
  }
  printw("\n%s ",promptstr);
  if(command)
    printw("(%c) ",command);
  getyx(stdscr,promptrow,promptcol);
  errorrow=promptrow-1;
  return 0;
}
/**********************************************************************/

int prompt_c()
{
  char *p;
  int i;

  if(pass>0)
    beep();
  if(pass==0)
    display_file_c(file_name);
  else if(pass==1){
    move(errorrow,0);
    printw("ERROR: option is not valid, choose one of {%s}",options+1);
    move(promptrow,promptcol);
  }
  refresh();
  do{
    option=getch();
    if(upcase)
      option=toupper(option);
    if(time_out){
      --time_out;
      if(!time_out)
        option=command;
    }
  }
  while(option<0);
  /*ioctl(1,TCSETA,&sioold);*/
  if(command&&(option=='\r'||option=='\n'))
    option=command;
  if(p=strchr(options,option)){
    clear();
    refresh();
    endwin();
    return p-options;
  }
  time_out=0;
  ++pass;
  mvaddch(promptrow,promptcol,iscntrl(option)?'.':option);
  move(promptrow,promptcol);
  return NOOPT_NOEXIT;
}
/************************************************************************/
#ifdef HALFDELAY

int halfdelay(int vtime)
{
  if(ioctl(1,TCGETA,&sioio)==-1){
    printw("ERROR: in call to ioctl() to copy tty setup\n");
    return ERROR_EXIT;
  }
  sioio.c_cc[VMIN]=time_out?0:1;
  sioio.c_cc[VTIME]=vtime;
  ioctl(1,TCSETA,&sioio);

  return 0;
}
#endif
#endif
/************************************************************************/

int clear_b()
{
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  return 0;
}
/**********************************************************************/

int display_file_b(char *file_name)
{
  char menu[260];
  FILE *fmenu;

  clear_b();
  if(fmenu=fopen(file_name,"r")){
    while(fgets(menu,sizeof(menu)-1,fmenu))
      fputs(menu,stdout);
    fclose(fmenu);
  }
  else{
    printf("ERROR: cant find menu file: %s\n",file_name);
    printf("Valid options are: {%s}\n",options+1);
  }
  if(pass>0)
    printf("\aERROR: option is not valid, choose one of {%s}",options+1);
  printf("\n%s ",promptstr);
  if(command)
    printf("(%c) ",command);
  return 0;
}
/**********************************************************************/

int prompt_b()
{
  char *p;

  display_file_b(file_name);
  if(ioctl(1,TCGETA,&sioold)==-1){
    printf("ERROR: in call to ioctl() to save old tty setup\n");
    return ERROR_EXIT;
  }
  if(ioctl(1,TCGETA,&sioio)==-1){
    printf("ERROR: in call to ioctl() to copy tty setup\n");
    return ERROR_EXIT;
  }
  sioio.c_lflag=0;
  sioio.c_cc[VMIN]=time_out?0:1;
  sioio.c_cc[VTIME]=10;
  ioctl(1,TCSETA,&sioio);
  do{
    option=getchar();
    if(upcase)
      option=toupper(option);
    if(time_out){
      --time_out;
      if(!time_out)
        option=command;
    }
  }
  while(option<0);
  ioctl(1,TCSETA,&sioold);
  if(command&&option==0x0a)
    option=command;
  if(p=strchr(options,option)){
    clear_b();
    return p-options;
  }
  time_out=0;
  ++pass;
  printf("%c \n",option);
  return NOOPT_NOEXIT;
}
/**********************************************************************/

int help()
{
  printf("\nshmenu: V%s\n\n",version);
  printf("Usage: shmenu [-b] [-c <default> [-t <timeout>]] ");
  printf("[-d <level>] [-h] [-p <prompt>] <options> <menufile>\n\n");
  printf("Copyright: Owen Duffy 1987,2009,2025.\n");
  printf("All rights reserved.\n\n");
  return HELP_EXIT;
}
/**********************************************************************/

int main(int argc,char **argv)
{
  int rc;
  extern int optind,optopt,opterr;
  extern char *optarg;
  int opt;
  char *p;

  #ifdef CURSES
    basic=0;
  #endif

  /* process command line options */
  optind=optind?optind:1;
  /*opterr=0;*/
  while((opt=getopt(argc,argv,"bc:dhp:t:u"))!=EOF){
    switch(opt){
      case 'c':
        command=optarg[0];
        break;
      case 'b':
        basic=1;
        break;
      case 'd':
        if(optarg)
          debug=atoi(optarg);
        break;
      case 'p':
        if(optarg)
          strcpy(promptstr,optarg);
        break;
      case 'h':
        return help();
      case 't':
        if(optarg)
          time_out=atoi(optarg);
        break;
      case 'u':
        upcase=1;
        break;
      case '?':
      case ':':
        help();
        return ERROR_EXIT;
    }
  }

  /* debug */
  if(debug>0){
  }
  /* process non-option arg */
  if(argc<=optind){
    printf("ERROR: No options.\n");
    help();
    return ERROR_EXIT;
  }
  options[0]=0x1b;
  strcpy(options+1,argv[optind]);
  if(upcase){
    command=toupper(command);
    for(p=options;*p;p++)
      *p=toupper(*p);
  }
  if(!strchr(options,command)){
    printf("Error: default command not valid.\n\n");
    return ERROR_EXIT;
  }
  if(time_out&&!command)
    time_out=0;
  if(argc>optind+1)
    strcpy(file_name,argv[optind+1]);
  else{
    printf("ERROR: No menu filename.\n");
    help();
    return ERROR_EXIT;
  }
  prompt=prompt_b;
  #ifdef CURSES
  if(!basic){
    /*get terminfo entry for terminal*/
    setupterm(NULL,1,&rc);
    /*set basic flag if terminal does not support cursor addressing*/
    basic=rc==1?tigetstr("cup")==0:1;
  }
  if(!basic){
    setlocale(LC_ALL,"");
    if(initscr()==0)
      basic=1;
    else{
      keypad(stdscr,TRUE);
      scrollok(stdscr,1);
      noecho();
      raw();
      if(time_out)
        halfdelay(10);
      prompt=prompt_c;
    }
  }
  #endif

  do{
      selection=prompt();
  }
  while(selection==NOOPT_NOEXIT);

  if(!basic){
    endwin();
  }
  return selection;
}
/**********************************************************************/

/*f-*/
/************************************************************************
menu: a shell script enhancer for menus

$Id$

$Log$
Revision 1.12.1.2  1996/06/17 21:33:28  owen
Update version.

 * Revision 1.12.1.1  1996/06/17  21:28:06  owen
 * Branch of non-curses menu. Minor revisions for consistency with curses
 * version.
 *
 * Revision 1.12  1996/06/13  23:08:28  owen
 * Further revision of rc, selectable prompt, upcase revised.
 *
 * Revision 1.11  1996/06/13  17:15:34  owen
 * Issue rc in range 0 - 255.
 *
 * Revision 1.10  1996/06/13  10:19:19  owen
 * Removed default menu file.
 *
 * Revision 1.9  1996/06/12  23:36:46  owen
 * Bugs fixed.
 *
 * Revision 1.8  1996/06/12  23:18:46  owen
 * Revision of return codes.
 *
 * Revision 1.7  1996/06/12  03:50:51  owen
 * Revised handling of null filename.
 *
 * Revision 1.6  1996/06/11  23:34:11  owen
 * Fixes for AIX.
 *
 * Revision 1.5  1996/06/11  21:52:00  owen
 * Reformatted.
 *
 * Revision 1.4  1996/06/11  21:47:11  owen
 * Revised default/timeout handling.
 *
 * Revision 1.3  1996/06/11  11:19:45  owen
 * Error checking of parms.
 *
 * Revision 1.2  1996/06/11  10:58:45  owen
 * Added options.
 *
 *
*************************************************************************/
/*f+*/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <ctype.h>
#define ESC_EXIT 0
#define NOOPT_NOEXIT -1
#define NOOPT_EXIT 255
#define HELP_EXIT 254
#define ERROR_EXIT 253
char version[6]="1.07",file_name[256]="";
char rcsid[]="$Id$";
int debug=0,timeout=0,option;
int rc,parmindx,selection=0,first_time=1,erase=0,upcase=0,lrc=0;
char options[256]="",command='\0',promptstr[80]="Enter option ===>";
struct termio sioio,sioold;
/************************************************************************/

int clear_screen()
{
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  return 0;
}
/**********************************************************************/

display_file(char *file_name)
{
  char menu[260];
  FILE *fmenu;

  if(file_name[0]){
    clear_screen();
    if(fmenu=fopen(file_name,"r")){
      while(fgets(menu,sizeof(menu)-1,fmenu))
        fputs(menu,stdout);
      fclose(fmenu);
    }
    else{
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
  printf("\n%s ",promptstr);
  if(command)
    printf("(%c) ",command);
  if(ioctl(1,TCGETA,&sioold)==-1){
    printf("ERROR: in call to ioctl() to save old tty setup\n");
    return ERROR_EXIT;
  }
  if(ioctl(1,TCGETA,&sioio)==-1){
    printf("ERROR: in call to ioctl() to copy tty setup\n");
    return ERROR_EXIT;
  }
  sioio.c_lflag=0;
  sioio.c_cc[VMIN]=timeout?0:1;
  sioio.c_cc[VTIME]=10;
  ioctl(1,TCSETA,&sioio);
  do{
    option=getchar();
    if(upcase)
      option=toupper(option);
    if(timeout){
      --timeout;
      if(!timeout)
        option=command;
    }
  }
  while(option<0);
  ioctl(1,TCSETA,&sioold);
  timeout=0;
  first_time=0;
  if(option==0x1b){
    putchar('\n');
    return ESC_EXIT;
  }
  if(command&&option==0x0a)
    option=command;
  printf("%c \n",option);
  if(p=strchr(options,option))
    return p-options+1;
  else
    return file_name[0]?NOOPT_NOEXIT:NOOPT_EXIT;
}
/**********************************************************************/

int help()
{
  printf("\nmenu: V%s %s\n\n",version,rcsid);
  printf("Usage: menu [-c <default> [-t <timeout>]] ");
  printf("[-d <level>] [-h] [-p <prompt>] [-r <rc>] options [<menufile>]\n\n");
  printf("Copyright: Owen Duffy & Associates Pty Ltd 1987,1996.\n");
  printf("All rights reserved.\n\n");
  return HELP_EXIT;
}
/**********************************************************************/

main(int argc,char **argv)
{
  int rc;
  extern int optind,optopt,opterr;
  extern char *optarg;
  int opt;
  char *p;

  /* process command line options */
  optind=optind?optind:1;
  /*opterr=0;*/
  while((opt=getopt(argc,argv,"c:d:hp:r:t:u"))!=EOF){
    switch(opt){
      case 'c':
        command=optarg[0];
        break;
      case 'd':
        if(optarg)
          debug=atoi(optarg);
        break;
      case 'p':
        if(optarg)
          strcpy(promptstr,optarg);
        break;
      case 'r':
        if(optarg)
          lrc=atoi(optarg);
        if(lrc==NOOPT_EXIT)
          first_time=0;
        break;
      case 'h':
        return help();
      case 't':
        if(optarg)
          timeout=atoi(optarg);
        break;
      case 'u':
        upcase=1;
        break;
      case '?':
      case ':':
        return help();
    }
  }

  /* debug */
  if(debug>0){
  }
  /* process non-option arg */
  if(argc<=optind){
    printf("Error: No options supplied.\n");
    help();
    return ERROR_EXIT;
  }
  strcpy(options,argv[optind]);
  if(upcase){
    command=toupper(command);
    for(p=options;*p;p++)
      *p=toupper(*p);
  }
  if(!strchr(options,command)){
    printf("Error: default command not valid.\n\n");
    return ERROR_EXIT;
  }
  if(timeout&&!command)
    timeout=0;
  if(argc>optind+1)
    strcpy(file_name,argv[optind+1]);
  
  do{
    selection=prompt();
  }
  while(selection==NOOPT_NOEXIT);
  if(selection!=ERROR_EXIT)
    clear_screen();
  else
    printf("\n\n");
  return selection;
}
/**********************************************************************/

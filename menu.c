/*f-*/
/************************************************************************
menu: a shell script enhancer for menus

$Id$

$Log$
Revision 1.7  1996/06/12 03:50:51  owen
Revised handling of null filename.

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
char version[6]="1.03",file_name[256]="menu.scr";
char rcsid[]="$Id$";
int debug=0,timeout=0,option;
int rc,parmindx,selection=0,first_time,erase=0;
char options[256]="",command='\0';
struct termio sioio,sioold;
/************************************************************************/

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
  if(command)
    printf("(%c) ",command);
  if(ioctl(1,TCGETA,&sioold)==-1){
    printf("ERROR: in call to ioctl() to save old tty setup\n");
    return -1;
  }
  if(ioctl(1,TCGETA,&sioio)==-1){
    printf("ERROR: in call to ioctl() to copy tty setup\n");
    return -1;
  }
  sioio.c_lflag=0;
  sioio.c_cc[VMIN]=timeout?0:1;
  sioio.c_cc[VTIME]=10;
  ioctl(1,TCSETA,&sioio);
  do{
    option=getchar();
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
    return -1;
  }
  if(command&&option==0x0a)
    option=command;
  printf("%c \n",option);
  if(p=strchr(options,option))
    return p-options+1;
  else
    return 0;
}
/**********************************************************************/

int help()
{
  printf("\nmenu: V%s %s\n\n",version,rcsid);
  printf("Usage: menu [-e] [-c <default> [-t <timeout>]] ");
  printf("[-d <level>] [-h] options [<menufile>]\n\n");
  printf("Copyright: Owen Duffy & Associates Pty Ltd 1987,1996.\n");
  printf("All rights reserved.\n\n");
  return -1;
}
/**********************************************************************/

main(int argc,char **argv)
{
  int rc;
  extern int optind,optopt,opterr;
  extern char *optarg;
  int opt;

  /* process command line options */
  optind=optind?optind:1;
  /*opterr=0;*/
  while((opt=getopt(argc,argv,"c:d:eht:"))!=EOF){
    switch(opt){
      case 'c':
        command=optarg[0];
        break;
      case 'd':
        if(optarg)
          debug=atoi(optarg);
        break;
      case 'e':
        erase=1;
        break;
      case 'h':
        return help();
      case 't':
        if(optarg)
          timeout=atoi(optarg);
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
    printf("No options.\n");
    return help();
  }
  first_time=1;
  selection=0;
  strcpy(options,argv[optind]);
  if(!strchr(options,command)){
    printf("Error: default command not valid.\n\n");
    return -1;
  }
  if(timeout&&!command)
    timeout=0;
  if(argc>optind+1)
    strcpy(file_name,argv[optind+1]);
  do{
    selection=prompt();
    if(selection==-1){
      selection=0;
      break;
    }
    if(!selection&&!file_name[0])
      selection=-2;
  }
  while(!selection);
  if(erase)
    clear_screen();
  else
    printf("\n");
  return selection;
}
/**********************************************************************/

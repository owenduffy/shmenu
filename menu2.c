
/*f-*/
/************************************************************************
menu: a shell script enhancer for menus

$Id$

$Log$
Revision 1.2  1996/06/11 10:58:45  owen
Added options.

 *
*************************************************************************/
/*f+*/
#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>

char version[6]="1.01",*optarg,command='\0';
char rcsid[]="$Id$";
int optind,optopt,debug=0,timeout=0;
int rc,parmindx,selection=0,first_time;
char option,file_name[256]="menu.scr",options[256]="";
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
  }while(option<0);
ioctl(1,TCSETA,&sioold);
timeout=0;
first_time=0;
if(option==0x1b)
  return -1;
if(option==0x0a)
  option=command;
if(p=strchr(options,option))
  return p-options+1;
else
  return 0;
}
/**********************************************************************/

int help()
{
  printf("\nmenu: V%s %s\n\n",version,rcsid);
  printf("Usage: menu [-a <accessfile>] [-d <level>] [-h] ");
  printf("[-l <logfile>] [-p] [-q <queuedir>] [-w <workdir>] \n\n");
  printf("Copyright: Owen Duffy & Associates Pty Ltd 1995,1996.\n");
  printf("All rights reserved.\n\n");
  return(0);
}
/**********************************************************************/

main(int argc,char **argv)
{
  int rc;
  char opt;

  /* process command line options */
  optind=optind?optind:1;
  opterr=0;
  while(1){
    opt=getopt(argc,argv,"c:d:ht:");
    if(opt==-1)
      break;
    switch(opt){
      case 'c':
        command=optarg[0];
        break;
      case 'd':
        if(optarg)
          debug=atoi(optarg);
        break;
      case 'h':
        help();
        return(4);
      case 't':
        if(optarg)
          timeout=atoi(optarg);
        break;
      case '?':
      case ':':
        help();
        return 4;
    }
  }

  /* debug */
  if(debug>0){
  }
  /* process non-option arg */
  if(argc<=optind){
    printf("No options.\n");
    help();
    return 4;
  }

  first_time=1;
  selection=0;
  strcpy(options,argv[optind]);
  if(argc>optind+1)
    strcpy(file_name,argv[optind+1]);

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
/**********************************************************************/


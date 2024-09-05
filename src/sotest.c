#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <dlfcn.h> 
#include <unistd.h>

#define PATH_MAX 400
#define PATH_LENGTH 300
#define MAX_LINE 400
#define MAX_TOKEN 400
#define MAX_FILENAME 50 

char *getcwd(char *buf, size_t size);
//We declare a function pointer for function to import (int myfunction(int) )
typedef void (*t_myfunc)(void);

/*
 * This function returns the next alphanumeric substring starting at index 0 until index i-1 where line[i]==' ' 
 * or line[i]=='\n' or line[i]=='\0'
 */
int extractToken(char line[], char token[], int maxlength) {
   int i=0 ; 
   while ( i<maxlength ) {
      if ( (line[i]==' ') || (line[i]=='\n') || ( line[i]=='\0' ) ) {
	      strncpy(token,line,i);
	      token[i] = '\0' ;
	      break;
      } 
      i++ ;
   }
   return 0;
}

void interactiveMode(char currentPath[]) {
   FILE * fp=stdin;
   char *charPrt = NULL ;
   char line[400];
   char commandC[400];
   char pathLibrary[MAX_FILENAME];
   char functionName[400];
   char completePath[400];
   int boolFileLoaded =0;
   void* dlh = NULL ;



   while ( 1 ) {
     if (boolFileLoaded==0) {
        printf("Command(use,quit): ");
     }
     else {
        printf("Command(use,call,quit): ");
     }
     scanf("%s", commandC);

     if ( strcmp(commandC,"quit")==0 ) {
        break;
     }


     if ( strcmp(commandC,"use")==0 ) {
        printf("Name of shared library: ");
        scanf("%s", pathLibrary);
     }
     else if ( strcmp(commandC,"call")==0 ) {
        printf("Function name: ");
        scanf("%s", pathLibrary);
     }

     if ( (commandC[0]=='#') ||  (commandC[0]==';') ) {
	continue;
     }
      
     /* vamos a extraer el comando */
     // extractToken(line, commandC , 400);
     //bo printf("command >%s<\n",commandC); 


     if ( strcmp(commandC,"use")==0 ) { 
           if ( (boolFileLoaded==1) && ( dlh!=NULL ) ) {
              dlclose(dlh);
	   }


           /* Vamos a cargar un archivo */
           // extractToken(line+4, pathLibrary, 400);
           //bo printf("pathLibrary >%s<\n",pathLibrary); 

           //We open the shared object
           sprintf(completePath,"%s%s%s",currentPath,"/",pathLibrary);

           dlh = dlopen(completePath, RTLD_LAZY );

           if ( dlh==NULL ) {
              fprintf(stderr, "File %s not found or this file is not a shared library\n", pathLibrary );
              exit(1);
           } 
	   boolFileLoaded = 1;
     }
     else if ( strcmp(commandC,"call")==0 ) { 
           /* Vamos a cargar un archivo */
           strcpy(functionName,pathLibrary);
           if ( boolFileLoaded==1 ) {
              //extractToken(line+5, functionName, 400);

              //We invoke the function functionName
              t_myfunc mf1 = dlsym(dlh, functionName);
              char *strError = dlerror();
              if ( strError!=NULL ) {
                 fprintf(stderr, "Function %s doesn't found in the shared library\n", functionName );
                 exit(1);
	      }

              (*mf1)();
	   }
	   else {
              fprintf(stdout,"warning: before calling a function you must load a library with 'use' command" ); 
	   }
     }	     
     
   }

   if (dlh!=NULL) {
      dlclose(dlh);
   }
}

void trim(char source[], char target[]) {
   int i=0;
   int iTarget=0;
   int j=strlen(source)-1;
   while ( isspace(source[i]) ) {
      i++ ;  	   
   }

   while ( (j>=i) && isspace(source[j])  ){
      j-- ;
   }
   strncpy(target, source+i,j-i+1);
   target[j-i+1] = '\0';
}

int main(int argc, char** argv) { 

  char filename[MAX_FILENAME];
  char filenameSO[MAX_FILENAME];
  char line[MAX_LINE];
  char commandC[MAX_TOKEN];
  char pathLibrary[PATH_MAX];
  char functionName[MAX_TOKEN];
  int boolFileLoaded =0;
  int boolReadingFunctionName =0;
  void* dlh = NULL ;

  char cwd[PATH_LENGTH];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
       fprintf(stderr, "getcwd() error");
       return 1;
  } 

  if (argc==1) {
     interactiveMode(cwd) ;
     return 0;
  }

  strcpy(filename,argv[1]);

  //bo printf("filename >%s<\n", filename);

  FILE *fp = fopen(filename,"r");

  if (fp==NULL) {
     /* filename doesn't exist */ 
     fprintf(stderr, "Script file %s not found\n", filename);
  }

  char *charPrt = NULL ;

  for ( charPrt = fgets(line,400,fp) ; charPrt != NULL ; charPrt = fgets(line,400,fp) ) {

     if ( (line[0]=='#') ||  (line[0]==';') ) {
	continue;
     }
      
     /* vamos a extraer el comando */
     extractToken(line, commandC , 400);
     //bo printf("command >%s<\n",commandC); 


     if ( strcmp(commandC,"use")==0 ) { 
           if ( (boolFileLoaded==1) && ( dlh!=NULL ) ) {
              dlclose(dlh);
	   }


           /* Vamos a cargar un archivo */
           extractToken(line+4, filenameSO, 400);
           sprintf(pathLibrary,"%s%s%s",cwd,"/",filenameSO);
           //bo printf("pathLibrary >%s<\n",pathLibrary); 

           //We open the shared object
           dlh = dlopen(pathLibrary, RTLD_LAZY );

           if ( dlh==NULL ) {
              fprintf(stderr, "File %s not found or this file is not a shared library\n", pathLibrary );
              exit(1);
           } 
	   boolFileLoaded = 1;
     }
     else if ( strcmp(commandC,"call")==0 ) { 
           /* Vamos a cargar un archivo */
           if ( boolFileLoaded==1 ) {
              extractToken(line+5, functionName, 400);

	      if ( functionName[0]=='\\' ) {
	         boolReadingFunctionName = 1;
		 charPrt = fgets(line,400,fp);
                 trim(line, functionName) ;
	      }

              //We invoke the function functionName
              t_myfunc mf1 = dlsym(dlh, functionName);
              char *strError = dlerror();
              if ( strError!=NULL ) {
                 fprintf(stderr, "Function %s doesn't found in the shared library\n", functionName );
                 exit(1);
	      }

              (*mf1)();
	   }
	   else {
              fprintf(stdout,"warning: before calling a function you must load a library with 'use' command" ); 
	   }
     }	     
     
  }

  return 0;
}


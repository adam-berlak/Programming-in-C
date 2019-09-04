#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FNAME_SIZE 512
#define MAX_FILES 1024

int myComparisonFunc(const void *x, const void *y);
int main( int argc, char ** argv) {
  // handle command line arguments
  char buffer1[1024];
  snprintf(buffer1, sizeof(buffer1), "find . -name '*.%s'", argv[1]);
  // open 'find...'
  FILE * fp = popen( buffer1, "r");
  if( fp == NULL) {
    perror("popen failed:");
    exit(-1);
  }
  // read in all filenames
  char buff[MAX_FNAME_SIZE];
  int nFiles = 0;
  char * files[MAX_FILES];
  while(fgets(buff,MAX_FNAME_SIZE,fp)) {
    int len = strlen(buff) - 1;
    files[nFiles] = strndup(buff,len);
    nFiles ++;
  }
  fclose(fp);

  qsort(files, nFiles, sizeof(*files), myComparisonFunc);

  // get file sizes for each file and sum them up
  long long totalSize = 0;
  struct stat st;
  for(int i = 0 ; i < atoi(argv[2]) ; i ++ ) {
    if( 0 != stat(files[i],&st)) {
      perror("stat failed:");
      exit(-1);
    }
    totalSize += st.st_size;
    printf("%s : %ld\n", files[i], st.st_size);
  }
  printf("%lld\n", totalSize);
  // clean up
  for(int i = 0; i < nFiles ; i ++ ) {
    free(files[i]);
  }
  // return success
  return 0;
}
int myComparisonFunc(const void *x, const void *y){
 char** fName1 = (char**) x;
 char** fName2 = (char**) y;
 struct stat file1;
 struct stat file2;
 int res;
 stat(*fName1, &file1);
 stat(*fName2, &file2);
 res = file1.st_size - file2.st_size;
 return(res * -1);
}

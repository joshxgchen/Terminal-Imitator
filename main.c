#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#define _GNU_SOURCE

void getAndPrintGroup(gid_t grpNum) {
  struct group *grp;

  grp = getgrgid(grpNum); 
  
  if (grp) {
    printf("The group ID %u -> %s\n", grpNum, grp->gr_name);
  } else {
    printf("No group name for %u found\n", grpNum);
  }
}

void getAndPrintUserName(uid_t uid) {

  struct passwd *tempStruct = NULL;
  tempStruct= getpwuid(uid);

  if (tempStruct) {
    printf("The user ID %u -> %s\n", uid, tempStruct->pw_name);
  } else {
    perror("Hmm not found???");
    printf("No name found for %u\n", uid);
  }
}

void msgPrint(struct stat);

void returnVal(const char *path, void (*printFile)(struct dirent*, const char*, int), int input, int R) {
    DIR *dir;
    struct dirent *tempor;
    if(!(dir = opendir(path))){
        printf("there is no such dir or file: %s\n", path);
        return;
    }
    while ((tempor = readdir(dir)) != NULL){
        if(tempor->d_type == DT_REG || tempor->d_type == DT_DIR || tempor->d_type == DT_LNK){
            if( tempor->d_name[0] == '.')
                continue;
            printFile(tempor, path, input);
        }
    }
    closedir(dir);
    if (R == 1){
        if(!(dir = opendir(path)))
            return;
        while ((tempor = readdir(dir)) != NULL){
            if (tempor->d_type == DT_DIR) {
                char nextPath[2048];
                if( tempor->d_name[0] == '.')
                    continue;
                snprintf(nextPath, sizeof(nextPath), "%s/%s", path, tempor->d_name);
                printf("\n\n%s:\n", nextPath);
                returnVal(nextPath, printFile, input, R);
            }
        }   
        closedir(dir);
    }
}
char *returner[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void retMsg(struct dirent *tempor, const char *path, int i){
    if(i==1)
        printf("%lu ", tempor->d_ino);
    printf("%s  ", tempor->d_name);
}

void retI(struct dirent *tempor, const char *path, int input){
    char temp[2000];
    char temp2[2000];
    struct stat lsValue;
    struct passwd *code = NULL;
    struct group *code2 = NULL;
    struct tm *time = NULL;
    snprintf(temp, sizeof(temp), "%s/%s", path, tempor->d_name);
    lstat(temp, &lsValue);
    code2 = getgrgid(lsValue.st_gid); 
    code = getpwuid(lsValue.st_uid);
    time = localtime(&lsValue.st_mtime);
    if(input==1)
        printf("%10lu", lsValue.st_ino);
    msgPrint(lsValue);
    printf("%4lu", lsValue.st_nlink);
    printf("%10s ", code->pw_name);
    printf("%8s", code2->gr_name);
    printf("%8lu", lsValue.st_size);
    printf("%7s %02d %04d %02d:%02d\t", returner[time->tm_mon], time->tm_mday, time->tm_year + 1900, time->tm_hour, time->tm_min);
    printf("%s", tempor->d_name);
    
    if( tempor->d_type == DT_LNK){
        readlink(tempor->d_name, temp2, sizeof(temp2));
        printf(" -> %s", temp2);
    }
    printf("\n");
}

void msgPrint(struct stat lsValue){
    printf((S_ISDIR(lsValue.st_mode)) ? "d" : "-");
    printf((lsValue.st_mode & S_IRUSR) ? "r" : "-");
    printf((lsValue.st_mode & S_IWUSR) ? "w" : "-");
    printf((lsValue.st_mode & S_IXUSR) ? "x" : "-");
    printf((lsValue.st_mode & S_IRGRP) ? "r" : "-");
    printf((lsValue.st_mode & S_IWGRP) ? "w" : "-");
    printf((lsValue.st_mode & S_IXGRP) ? "x" : "-");
    printf((lsValue.st_mode & S_IROTH) ? "r" : "-");
    printf((lsValue.st_mode & S_IWOTH) ? "w" : "-");
    printf((lsValue.st_mode & S_IXOTH) ? "x" : "-");
} 

int main( int argc, char **argv){
    int i=0,check = 0, list = 0, count=1, baseCase =0, recursive = 0;
    if(argc == 1){
        returnVal(".", retMsg, i, recursive);
        printf("\n");
        return 0;
    }
    if(argv[argc-1][0] == '-')
       count=0; 

    for(int iter=1; iter < argc-count; iter++){   
        if(argv[iter][0] == '.' && argv[iter][1] != '.')
        {
            check++;
        }
        for(int iter2=1; argv[iter][iter2]!='\0'; iter2++){
            if(argv[iter][iter2] == 'l'){
                list = 1;
            }
            else if(argv[iter][iter2] == '.'){
                baseCase++;
            }
            else if(argv[iter][iter2] == 'i'){
                i = 1;
            }
            else if(argv[iter][iter2] == 'R'){
                recursive = 1;
            }
            else {
                printf("You can only do l, i, or R, you can't do: %c\n", argv[iter][iter2]); 
                return 1;
            }
        }
    }
    while(check != 0){
      if(list == 0){
          returnVal(".", retMsg, i, recursive);
          printf("\n\n");
      }
      else{
          returnVal(".", retI, i, recursive);
          printf("\n\n");
      }
      check--;
    }
    while(baseCase != 0){
      if(list == 0){
          returnVal("..", retMsg, i, recursive);
          printf("\n\n");
      }
      else{
          returnVal("..", retI, i, recursive);
          printf("\n\n");
      }
      baseCase--;
    }
    if(count ==1 ){
        if(list == 0){
            returnVal(argv[argc-1], retMsg, i, recursive);
            printf("\n");
            return 0;
        }
        returnVal(argv[argc-1], retI, i, recursive);
        return 0;
    }
    if(list == 0){
        returnVal(".", retMsg, i, recursive);
        printf("\n");
        return 0;
    }
    returnVal(".", retI, i, recursive); 
    return 0;
}
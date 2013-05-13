/*
 * Copyright 2011-2013, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "ReadDirectory.h"

/**
 * @file ReadDirectory.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief UTILITY: A module to ease the manipulation of directories
 */

/**
 * @name Directory name manipulation
 * @{
 */

int ReadDir_isDirectory(const char* path){
    int isDir = 0;
    DIR *dir;

    dir = opendir(path);
    if(dir != NULL){
        isDir = 1;
        closedir(dir);
    }

    return isDir;
}


int ReadDir_countDirectories(const char* path){
    int nbDir = 0;
    char subDirPath[MAX_SIZE_PATH];
    /*Directory variables: */
    DIR *dir = NULL;
    struct dirent *ent = NULL;

    /*Open directory: */
    dir = opendir(path);
    if (dir != NULL) {
        /*Get subdirectories: */
        while ((ent = readdir(dir)) != NULL) {
            if(strcmp(ent->d_name, "..") && strcmp(ent->d_name, ".")){
                /*Build subpath: */
                strcpy(subDirPath, path);
                strcat(subDirPath, "/");
                strcat(subDirPath, ent->d_name);
                /*Increment: */
                if(ReadDir_isDirectory(subDirPath)){
                    nbDir++;
                }
            }
        }
        closedir (dir);
    }
    else{
        /*Invalid path: */
        nbDir = -1;
    }
    return nbDir;
}


int* ReadDir_charsPerDirectory(const char* path, const int nbDir){
    int* charsPerDir = NULL;
    int currentDir = 0;
    char subDirPath[MAX_SIZE_PATH];
    DIR *dir;
    struct dirent *ent;

    /*Open directory: */
    dir = opendir(path);
    if (dir != NULL) {
        /*Allocate memory: */
        charsPerDir = malloc(sizeof(int)*nbDir);
        #ifdef DEBUG
        if (charsPerDir == NULL) {
            printf("debug: malloc failed in ReadDir_charsPerDirectory() for \"charsPerDir\".\n");
            return NULL;
	    }
	    #endif
        /*Get subdirectories: */
        while ((ent = readdir(dir)) != NULL) {
            if(strcmp(ent->d_name, "..") && strcmp(ent->d_name, ".")){
                /*Build subpath: */
                strcpy(subDirPath, path);
                strcat(subDirPath, "/");
                strcat(subDirPath, ent->d_name);
                /*Increment: */
                if(ReadDir_isDirectory(subDirPath)){
                    charsPerDir[currentDir] = strlen(ent->d_name);
                    currentDir++;
                }
            }
        }
        closedir (dir);
    }

    return charsPerDir;
}


char** ReadDir_getDirectories(const char* path, const int nbDir, const int* charsPerDir){
    char **directories = NULL;
    char subDirPath[MAX_SIZE_PATH];
    int currentDir = 0, i = 0;
    DIR *dir;
    struct dirent *ent;

    /*Open directory: */
    dir = opendir(path);
    if (dir != NULL) {
        /*Allocate memory: */
        directories = malloc(sizeof(char*) * nbDir);
        if (directories == NULL) {
            printf("debug: malloc failed in ReadDir_getDirectories() for \"directories\"\n");
            return NULL;
	    }
	    for(i = 0; i < nbDir; i++){
	    	directories[i] = malloc(sizeof(char) * (charsPerDir[i] + 1));
	    	if (directories[i] == NULL) {
            	printf("debug: malloc failed in ReadDir_getDirectories() for \"directories[i]\"\n");
                return NULL;
            }
	    }
        /*Get subdirectories: */
        while ((ent = readdir(dir)) != NULL) {
            if(strcmp(ent->d_name, "..") && strcmp(ent->d_name, ".")){
                /*Build subpath: */
                strcpy(subDirPath, path);
                strcat(subDirPath, "/");
                strcat(subDirPath, ent->d_name);
                /*Increment: */
                if(ReadDir_isDirectory(subDirPath)){
                    strcpy(directories[currentDir],ent->d_name);
                    strcat(directories[currentDir], "\0");
                    currentDir++;
                }
            }
        }
        closedir (dir);
    }


    return directories;
}

/** @} */

/**
 * @name Filename manipulation
 * @{
 */

int ReadDir_isFile(const char* path){
    return (!ReadDir_isDirectory(path));
}

int ReadDir_countFiles(const char* path){
    int nbFiles = 0;
    char filePath[MAX_SIZE_PATH];
    /*Directory variables: */
    DIR *dir;
    struct dirent *ent;

    /*Open directory: */
    dir = opendir(path);
    if (dir != NULL) {
        /*Get subdirectories: */
        while ((ent = readdir(dir)) != NULL) {
            if(strcmp(ent->d_name, "..") && strcmp(ent->d_name, ".")){
                /*Build subpath: */
                strcpy(filePath, path);
                strcat(filePath, "/");
                strcat(filePath, ent->d_name);
                /*Increment: */
                if(!ReadDir_isDirectory(filePath)){
                    nbFiles++;
                }
            }
        }
        closedir (dir);
    }
    else{
        /*Invalid path: */
        nbFiles = -1;
    }
    return nbFiles;
}

int* ReadDir_charsPerFilename(const char* path, const int nbFiles){
    int* charsPerFilenam = NULL;
    int currentDir = 0;
    char filePath[MAX_SIZE_PATH];
    DIR *dir;
    struct dirent *ent;

    /*Open directory: */
    dir = opendir(path);
    if (dir != NULL) {
        /*Allocate memory: */
        charsPerFilenam = malloc(sizeof(int)*nbFiles);
        if (charsPerFilenam == NULL) {
            printf("debug: malloc failed in ReadDir_charsPerFilename() for \"charsPerFilenam\".\n");
            return NULL;
	    }
        /*Get subdirectories: */
        while ((ent = readdir(dir)) != NULL) {
            if(strcmp(ent->d_name, "..") && strcmp(ent->d_name, ".")){
                /*Build subpath: */
                strcpy(filePath, path);
                strcat(filePath, "/");
                strcat(filePath, ent->d_name);
                /*Increment: */
                if(!ReadDir_isDirectory(filePath)){
                    charsPerFilenam[currentDir] = strlen(ent->d_name);
                    currentDir++;
                }
            }
        }
        closedir (dir);
    }

    return charsPerFilenam;
}

char** ReadDir_getFilenames(const char* path, const int nbFiles, const int* charsPerFilenam){
    char** filenames = NULL;
    char filePath[MAX_SIZE_PATH];
    int currentDir = 0, i = 0;
    DIR *dir;
    struct dirent *ent;

    /*Open directory: */
    dir = opendir(path);
    if (dir != NULL) {
        /*Allocate memory: */
        filenames = malloc(sizeof(char*) * nbFiles);
        if (filenames == NULL) {
            printf("debug: malloc failed in ReadDir_getFilenames() for \"filenames\".\n");
            return NULL;
	    }
	    for(i = 0; i < nbFiles; i++){
	    	filenames[i] = malloc(sizeof(char) * (charsPerFilenam[i] + 1));
            if (filenames[i] == NULL) {
            	printf("debug: malloc failed in ReadDir_getFilenames() for \"filenames[i]\".\n");
                return NULL;
            }
	    }
        /*Get subdirectories: */
        while ((ent = readdir(dir)) != NULL) {
            if(strcmp(ent->d_name, "..") && strcmp(ent->d_name, ".")){
                /*Build subpath: */
                strcpy(filePath, path);
                strcat(filePath, "/");
                strcat(filePath, ent->d_name);
                /*Increment: */
                if(!ReadDir_isDirectory(filePath)){
                    
                    strcpy(filenames[currentDir],ent->d_name);
                    strcat(filenames[currentDir], "\0");
                    currentDir++;
                }
            }
        }
        closedir (dir);
    }

    return filenames;
}

/** @} */


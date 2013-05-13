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


#include "ReadFile.h"

/**
 * @file ReadFile.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief UTILITY: A module to ease file reading.
 */

int ReadFile_countLines(const char* fileName){
    FILE* f = NULL;
    int nbLines = 0;
    int currentChar = 0;

    /*Open the file: */
    f = fopen(fileName, "r");
    if(f != NULL){
        /*Get the first char: */
        currentChar = fgetc(f);
        if(currentChar == EOF){
            nbLines = -1;
        }
        /*Until the end of the file: */
        while(EOF != (currentChar = fgetc(f)))
            if(currentChar == '\n')
                nbLines++;

        /*Close the file: */
        fclose(f);
    }
    else {
    	#ifdef DEBUG
        printf("debug: Can't open the file %s correctly...\n", fileName);
        #endif
        return 0;
    }
	
    return (nbLines+1);
}


int* ReadFile_charsPerLine(const char* fileName, const int nbLines){
    FILE* f = NULL;
    int* result = NULL;
    int nbChar = 0, lineNumber = 0;
    int currentChar = 0;

    /*Open the file: */
    f = fopen(fileName, "r");
    if(f != NULL){
        /*Allocate memory: */
        result = malloc(sizeof(int)*nbLines);
        #ifdef DEBUG
        if (result == NULL) {
            printf("debug: malloc failed in ReadFile_charsPerLine() for \"result\".\n");
            return NULL;
	    }
	    #endif

        /*Get the first char: */
        currentChar = fgetc(f);
        /*Until the end of the file: */
        while(currentChar != EOF){
            if(currentChar == '\n') {
                result[lineNumber] = nbChar;
                nbChar = 0;
                lineNumber++;
            }
            else {
                nbChar++;
            }
            currentChar = fgetc(f);
            if (currentChar == EOF){
                result[lineNumber] = nbChar;
            }
        }
        /*Close the file: */
        fclose(f);
    }
    #ifdef DEBUG
    else {
        printf("debug: Can't open the file %s correctly...\n", fileName);
    }
    #endif
    return result;
}


char** ReadFile_readLines(const char* fileName, const int nbLines, const int* charsPerLine){
    FILE* f = NULL;
    char** lines = NULL;
    int i = 0;

    /*Open the file: */
    f = fopen(fileName, "r");
    if(f != NULL){
        /*Memory allocation : */
        lines = malloc(sizeof(char*)*nbLines);
        #ifdef DEBUG
        if (lines == NULL) {
            printf("debug: malloc failed in ReadFile_readLines() for \"lines\".\n");
            return NULL;
	    }
	    #endif
        /*Get the lines: */
        for(i=0; i<nbLines; i++){
            lines[i] = malloc(sizeof(char)*charsPerLine[i]+1);
            #ifdef DEBUG
            if (lines[i] == NULL) {
                printf("debug: malloc failed in ReadFile_readLines() for \"lines[i]\".\n");
                return NULL;
            }
            #endif
            fgets(lines[i], charsPerLine[i]+1, f);
            fseek(f, NB_ENDLINE_CHARS, SEEK_CUR);
        }
        /*Close the file: */
        fclose(f);
    }
    #ifdef DEBUG
    else {
        printf("debug: Can't open the file %s correctly...\n", fileName);
    }
    #endif
    return lines;
}



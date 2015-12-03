#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#include <zlib.h>

#define CHAR_BUFFER_SIZE 2048
#define N_PROPERTIES 80
#define N_SHELLS 6
#define N_FEATURES N_PROPERTIES*N_SHELLS

char **getprops(gzFile file, char buffer[]){
    char **props = malloc(sizeof(char*)*N_PROPERTIES);
    char *property;
    int i=0;
    int length=0;
    int err=0;

    // find first line with #\tPROPERTIES
    while(strstr(buffer, "#\tPROPERTIES")==NULL){
        gzgets(file, buffer, CHAR_BUFFER_SIZE);
    }

    // read in properties
    while(strstr(buffer, "#	PROPERTIES")){
        strtok(buffer, "\t");
        strtok(NULL, "\t");
        while( (property = strtok(NULL, ", \n")) ){
            length=strlen(property) + 1;
            props[i] = malloc(sizeof(char)*length);
            memcpy(props[i], property, length);
            i++;
        }
        gzgets(file, buffer, CHAR_BUFFER_SIZE);
    }

    return props;
}

void ff2arff(char** model){
    // initialize filename char arrays
    char pos[40];
    char neg[40];
    char arff[40];
    char buffer[CHAR_BUFFER_SIZE];
    int i=0;
    strcpy(pos, *model);
    strcpy(neg, *model);
    strcpy(arff, *model);
    strcat(arff, ".arff.gz");
    strcat(pos, ".pos.ff.gz");
    strcat(neg, ".neg.ff.gz");

    gzFile pIn = gzopen(pos, "rb");
    gzFile out = gzopen(arff, "wb9");

    if(pIn==NULL){
        printf("Error opening %s\n", pos);
        exit(1);
    }
    if(out==NULL){
        printf("Error opening %s\n", arff);
        exit(1);
    }

    gzbuffer(pIn, 1024*128);

    // write ARFF header
    gzputs(out, "% 1. Title: SFSU Feature File\n"
    "%\n% 2. Sources:\n%\n% 3. Author: San Francisco State University\n\n"
    "@RELATION ");
    gzputs(out, *model);
    gzputs(out, "\n\n");

    char **props = getprops(pIn, buffer);

    for(i=0;i<N_PROPERTIES;i++){
        int j;
        for(j=0; j<N_SHELLS;j++){
            char name[CHAR_BUFFER_SIZE];
            sprintf(name, "@ATTRIBUTE %s_s%d_c%d NUMERIC\n", props[i], j, i*N_SHELLS+j);
            gzputs(out, name);
        }
    }

    gzputs(out, "@ATTRIBUTE class {pos,neg}\n\n@DATA\n");

    while(buffer[0]=='#'){
        gzgets(pIn, buffer, CHAR_BUFFER_SIZE);
    }

    // read in positive data
    do{
        strtok(buffer, "\t");
        for(i=0;i<N_FEATURES;i++){
            gzputs(out, strtok(NULL, "\t"));
            gzputs(out, ",");
        }
        gzputs(out, "pos\n");
    } while( gzgets(pIn, buffer, CHAR_BUFFER_SIZE ));
    gzclose(pIn);

    gzFile nIn = gzopen(neg, "rb");
    if(nIn==NULL){
        printf("Error opening %s\n", neg);
        exit(1);
    }

    gzgets(nIn, buffer, CHAR_BUFFER_SIZE);
    while(buffer[0]=='#'){
        gzgets(nIn, buffer, CHAR_BUFFER_SIZE);
    }


    // read in negative data
    do{
        strtok(buffer, "\t");
        for(i=0;i<N_FEATURES;i++){
            gzputs(out, strtok(NULL, "\t"));
            gzputs(out, ",");
        }
        gzputs(out, "neg\n");
    } while( gzgets(nIn, buffer, CHAR_BUFFER_SIZE ));

    gzclose(nIn);
    gzflush(out,  Z_FINISH);
    gzclose(out);

}

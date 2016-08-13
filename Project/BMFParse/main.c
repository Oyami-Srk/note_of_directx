#include <stdio.h>
#include "BMFParse.h"

//My phone starts charging at 23:20;
//0%   - 23:20;
//42%  - 23:46;
//96%  - 00:28;
//100% - 00:39;
//My phone ends charging at 00:39;
//Total Time = 1:19; //^_^

int main(int argc, char **argv){
    Font fnt;
    if(argc < 3){
        printf("Wrong Arg!\nBMFParse Fontname CharID");
        return -1;
    }
    if(ParseFont(&fnt, argv[1]) != 0){
        printf("Cannot Parse Font file!\n");
        return -1;
    }
    OutPutCharByID(atoi(argv[2]), fnt);
    return 0;
}


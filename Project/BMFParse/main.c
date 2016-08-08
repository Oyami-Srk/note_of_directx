#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//My phone starts charging at 23:20;
//0%   - 23:20;
//42%  - 23:46;
//96%  - 00:28;
//100% - 00:39;
//My phone ends charging at 00:39;
//Total Time = 1:19; //^_^


typedef struct Char{
    int id;
    int Xpos, Ypos;
    int Width, Height;
}Char;

typedef struct Page{
    Char *chars;
    char name[64];
    int charNum;
}Page;

typedef struct Font{
    Page *pages;
    char name[64];
    int pageNum;
}Font;

int ReverseByteToInt(unsigned char data[], unsigned int size, int offset){
    int val  = 0;
    int i;
    for(i = size + offset - 1 ; i >= offset ; i--)
        val |= data[i] << (i - offset) * 8;
    return val;
}

long filesize(FILE*stream){
    long curpos, length;
    curpos = ftell(stream);
    fseek(stream, 0L, SEEK_END);
    length = ftell(stream);
    fseek(stream, curpos, SEEK_SET);
    return length;
}

int OutPutChar(Char chr){
    printf("========================\n");
    printf("Char ID : %d , Char : %c\n", chr.id, chr.id);
    printf("Xpos : %d , Ypos : %d\n", chr.Xpos, chr.Ypos);
    printf("Width : %d , Height : %d\n", chr.Width, chr.Height);
    printf("========================\n");
}

int ParseFont(Font *fnt, const char *Filename){
    FILE *fntfile = fopen(Filename, "rb");
    if(!fntfile){
        printf("Cannot Read fnt File!\n");
        fclose(fntfile);
        return -1;
    }
    int byteCount = filesize(fntfile);
    printf("FontFile Size : %dByte\n", byteCount);
    unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * byteCount);
    fread(data, sizeof(unsigned char) * byteCount, byteCount, fntfile);

    unsigned char FileHead[4] = { 0 };
    memcpy(FileHead, data, 3);
    if(strcmp(FileHead, "BMF") != 0){
        printf("Data Format Wrong!\n");
        return -1;
    }
    if(data[3] != 0x03){
        printf("Not Currently Version!\n");
        return -1;
    }

    fseek(fntfile, 23, SEEK_SET);
    fscanf(fntfile, "%s", fnt->name);
    printf("Font name : %s\n", fnt->name);
    int pageBlockSize = ReverseByteToInt(data, 4,
        ReverseByteToInt(data, 4, 5) + 30);
    printf("PageBlockSize : %d\n", pageBlockSize);
    //蒜香味的豆子吃进嘴里 就好像咬了一大口铁锈皮
    fseek(fntfile, ReverseByteToInt(data, 4, 5) + 34, SEEK_SET);
    char PageName[64] = { 0 };
    fscanf(fntfile, "%s", PageName);
    int pageNum = pageBlockSize / (strlen(PageName) + 1);
    fnt->pageNum = pageNum;
    printf("Page(s) Count : %d\n", pageNum);
    int i;
    if(pageNum != 1){
        fnt->pages = (Page*)malloc(sizeof(Page) * pageNum);
        fseek(fntfile, ReverseByteToInt(data, 4, 5) + 34, SEEK_SET);
        for(i = 0; i < pageNum; i++){
            fscanf(fntfile, "%s", PageName);
            strcpy(fnt->pages[i].name, PageName);
            PageName[0] = '\0';
        }
    }else{
        fnt->pages = (Page*)malloc(sizeof(Page));
        strcpy(fnt->pages[0].name, PageName);
    }
    unsigned char *data_ = (unsigned char *)malloc(
            sizeof(unsigned char) * (byteCount \
            - ReverseByteToInt(data, 4, 5)\
            - pageBlockSize - 34)
        );
    memcpy(data_,
        &data[ReverseByteToInt(data, 4, 5) + pageBlockSize + 34],
        byteCount - ReverseByteToInt(data, 4, 5) - pageBlockSize - 34
    );
    for(i = 0; i < pageNum; i++){
        printf("Page Name[%d] : %s\n", i, fnt->pages[i].name);
        if(ParsePage(&(fnt->pages[i]), i, data_) != 0){
            printf("Cannot Load Page[%d]!!\n", i);
            continue;
        }
    }
    free(data);
    free(data_);
    fclose(fntfile);
    return 0;
}

int ParsePage(Page *pag, int pagNum, unsigned char data[]){
    if(data[0] != 0x04){
        printf("Unexpected Block Symbol!\n");
        return -1;
    }
    int numChars_Total = (ReverseByteToInt(data, 4, 1) / 20);
    int numChars = 0;
    for(;numChars < numChars_Total;)
        if(data[numChars * 2 + 5 + 18 * ++numChars] != pagNum)
            break;
    if(!(pag->chars = (Char*)malloc(sizeof(Char) * numChars))){
        printf("Wrong in Memory allocation!\n");
        return -1;
    }
    printf("Num of Chars = %d\n", numChars);
    pag->charNum = numChars;
    unsigned char charData[20] = { 0 };
    int i;
    for(i = 0; i < numChars; i++){
        if(!memcpy(charData, &data[5 + i * 20], 20))
            return -1;
        if(ParseChar(&pag->chars[i], charData) != pagNum)
            i = 0;
        //OutPutChar(pag->chars[i]);
    }
    return 0;
}

int ParseChar(Char *chr, unsigned char data[20]){
    chr->id = ReverseByteToInt(data, 4, 0);
    chr->Xpos = ReverseByteToInt(data, 2, 4);
    chr->Ypos = ReverseByteToInt(data, 2, 6);
    chr->Width = ReverseByteToInt(data, 2, 8);
    chr->Height = ReverseByteToInt(data, 2, 10);
    return data[18];
}

int OutPutCharByID(int ID, Font fnt){
    int i, j;
    for(i = 0; i < fnt.pageNum; i++)
        for(j = 0; j < fnt.pages[i].charNum; j++)
            if(fnt.pages[i].chars[j].id == ID){
                OutPutChar(fnt.pages[i].chars[j]);
                return 0;
            }
    printf("Cannot find id : %d!\n", ID);
    return -1;
}

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

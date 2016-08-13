#ifndef __BMFPARSE_H_
#define __BMFPARSE_H_
#ifdef __cplusplus
extern "C" {
#endif

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

int OutPutChar(Char chr);

int OutPutCharByID(int ID, Font fnt);

int ParseFont(Font *fnt, const char *Filename);

#ifdef __cplusplus
}
#endif
#endif
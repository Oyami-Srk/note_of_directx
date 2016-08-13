#ifndef __BMFPARSE_CPP_H__
#define __BMFPARSE_CPP_H__
#include "BMFParse.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

typedef struct BMFPage{
    std::string PageName;
    std::map<char, Char> Chars;
}BMFPage;

typedef struct BMFont{
    std::string FontName;
    std::vector<BMFPage> Page;
}BMFont;

bool BMFParse(BMFont *fnt, const char *FntFileName);

bool UnloadFont(BMFont *fnt);

#endif //__BMFPARSE_CPP_H__
#include "libbmf.h"
//#include <stblib.h>

using std::map;
using std::vector;
using std::string;
using std::cout;
using std::endl;

bool BMFParse(BMFont *fnt, const char *FntFileName){
    Font font;
    if(ParseFont(&font, FntFileName) != 0){
        throw std::runtime_error("Cannot Parse Font \"" + string(FntFileName) + "\"!");
        return false;
    }
    fnt->FontName = string(font.name);
    for(int i = 0; i < font.pageNum; i++){
        BMFPage pPage;
        pPage.PageName = string(font.pages[i].name);
        for(int j = 0; j < font.pages[i].charNum; j++){
            pPage.Chars.insert(std::pair<char, Char>((char)font.pages[i].chars[j].id, font.pages[i].chars[j]));
        }
        fnt->Page.push_back(pPage);
        //free(font.pages);
    }
    return true;
}

bool UnloadFont(BMFont *fnt){
    for(vector<BMFPage>::iterator it = fnt->Page.begin(); it != fnt->Page.end(); it++){
        map<char, Char>().swap(it->Chars);
        it->Chars.clear();
    }
    vector<BMFPage>().swap(fnt->Page);
    fnt->Page.clear();
    return true;
}
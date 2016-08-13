#include "libbmf.h"
using namespace std;

int main(int argc, char **argv){
    if(argc < 2){
        cout << "Wrong Arg!" << endl;
        return 0;
    }
    BMFont Font;
    try{
        BMFParse(&Font, argv[1]);
    }catch(runtime_error e){
        cout << e.what() << endl;
    }
    cout << Font.FontName << endl;
    cout << Font.Page[0].PageName;
//    cout << Font.Page[0].Chars[argv[2][0]];
    return 0;
}
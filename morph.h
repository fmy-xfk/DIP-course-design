#ifndef __MORPH_H__
#define __MORPH_H__

#include "base.h"

typedef std::function<BYTE(int,int)> MorphFunc;

class MorphCore{
    std::vector<std::string> dat;
    int r,c,sz,mr=-1,mc=-1;
public:
    MorphCore(const char *str){
        std::string s(str);
        dat=str_split(s,';');
        r=dat.size();
        if(r==0) throw "MorphCore::bad_str";
        c=dat[0].size();
        if(c==0) throw "MorphCore::bad_str";
        sz=0;
        for(int i=0;i<r;++i){
            if(dat[i].size()!=c) throw "MorphCore::bad_str";
            for(int j=0;j<c;++j){
                char y=dat[i][j];
                if(y!='.' && y!='o' && y!='x') throw "MorphCore::bad_str";
                if(y=='x'){
                    if(mr!=-1 && mc!=-1) throw "MorphCore::bad_str";
                    mr=r;mc=c;
                }
                if(y!='.') ++sz;
            }
        }
        if(mr==-1 || mc==-1) throw "MorphCore::bad_str";
    }
    std::string& operator[](int i){
    	return dat[i];
	}
    int getRows(){return r;}
    int getCols(){return c;}
    int getSize(){return sz;}
    int getCenterRow(){return mr;}
    int getCenterCol(){return mc;}
};

#endif

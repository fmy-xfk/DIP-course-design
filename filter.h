#ifndef __FILTER_H__
#define __FILTER_H__

#include "base.h"

#define FILTER_OUTRANGE_CUT 0
#define FILTER_OUTRANGE_ABS -1

class FilterCore{
	int or_mode;
    float* dat;
    DWORD sz,area;
    void __init(DWORD _sz,BYTE md){
    	sz=_sz;or_mode=md;
        if(!(sz&1)) throw "FilterCore::bad_size";
        if(md<-1) throw "FilterCore::bad_out_range_mode";
        area=sz*sz;
        dat=new float[area];
    }
    DWORD __idx(DWORD r,DWORD c){
        if(r<0 || r>=sz) throw "FilterCore::bad_index_r";
        if(c<0 || c>=sz) throw "FilterCore::bad_index_c";
        return r*sz+c;
    }
public:
    void setAll(const char* str,float div){
        std::string s(str);
        auto ret=str_split(s,';');
        if(ret.size()!=sz) throw "FilterCore::bad_data_str";
        int j=0;
        for(auto x:ret){
            auto vals=str_split(x,' ');
            int len=vals.size();
            if(len!=sz) throw "FilterCore::bad_data_str";
            for(int i=0;i<len;++i){
                float f;
                sscanf(vals[i].c_str(),"%f",&f);
                dat[j*sz+i]=f/div;
            }
            ++j;
        }
    }
    FilterCore() = delete;
    FilterCore(FilterCore &cc){
        __init(cc.sz,or_mode);
        memcpy(dat,cc.dat,sizeof(float)*area);
    }
    FilterCore(DWORD sz,int mode=FILTER_OUTRANGE_CUT) {
        __init(sz,mode);
    }
    FilterCore(DWORD sz,float* src_dat,int mode=FILTER_OUTRANGE_CUT) {
        __init(sz,mode);
        memcpy(dat,src_dat,sizeof(float)*area);
    }
    FilterCore(DWORD sz,float* src_dat,float div,int mode=FILTER_OUTRANGE_CUT) {
        __init(sz,mode);
        for(int i=0;i<area;++i) dat[i]=src_dat[i]/div;
    }
    FilterCore(DWORD sz,const char* src,float div,int mode=FILTER_OUTRANGE_CUT){
        __init(sz,mode);
        setAll(src,div);
    }
    FilterCore(const char* str,float div,int mode=FILTER_OUTRANGE_CUT){
    	std::string s(str);
	    __init(str_split(s,';').size(),mode);
	    setAll(str,div);
	}
    DWORD size(){return sz;}
    float get(DWORD r,DWORD c){return dat[__idx(r,c)];}
    void set(DWORD r,DWORD c,float val){dat[__idx(r,c)]=val;}
    float* operator[](DWORD r){return dat+r*sz;}
    int getOutRangeMode(){return or_mode;}
    ~FilterCore(){delete[] dat;}
};

typedef std::function<int(std::vector<BYTE>&)> FilterSFunc;

class FilterS{
    DWORD sz;
    FilterSFunc func;
public:
    FilterS() = delete;
    FilterS(DWORD size,FilterSFunc f):sz(size),func(f){
        if(!(sz&1)) throw "FilterS::bad_size";
    }
    DWORD getSize(){return sz;}
    int proc(std::vector<BYTE> &dat) {return func(dat);}
};

//给定距离，输出通过量
typedef std::function<double(double)> FilterF;

#endif

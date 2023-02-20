#ifndef __MAT_H__
#define __MAT_H__

#include "base.h"
#include "bmp.h"
#include "filter.h"
#include "morph.h"

static std::mt19937 __rnd(time(0));

class Mat{
    friend class FilterCore;
    friend class Bmp;
    DWORD R,C,sz;
    BYTE* dat;
    void __check_rc(DWORD r,DWORD c){
        if(r<0 || r>=R) throw "Mat::bad_index_r";
        if(c<0 || c>=C) throw "Mat::bad_index_c";
    }
    DWORD __idx(DWORD r,DWORD c){
        if(r<0 || r>=R) throw "Mat::bad_index_r";
        if(c<0 || c>=C) throw "Mat::bad_index_c";
        return r*C+c;
    }
    DWORD __i(DWORD r,DWORD c){return r*C+c;}
    void __fromBmp(Bmp &bmp){
        R=bmp.getHeight();C=bmp.getWidth();
        dat=new BYTE[sz=R*C];
        BYTE* p2=dat;
        DWORD lw1=bmp.getLineSize();
        for(int i=R-1;i>=0;i--){
            BYTE* p1=bmp.getDatptr()+i*lw1;
            for(int j=0;j<C;j++){
                *(p2++)=(BYTE)((*(p1)*114+*(p1+1)*587+*(p1+2)*299)/1000);
                p1+=3;
            }
        }
    }
public:
    //内部操作
    Mat() = delete;
    Mat(const Mat &mat){
        R=mat.R;C=mat.C;
        dat=new BYTE[sz=R*C];
        memcpy(dat,mat.dat,sz);
    }
    Mat(DWORD rows,DWORD cols):R(rows),C(cols){
        dat=new BYTE[sz=R*C];
        memset(dat,0,sz);
    }
    Mat(const char *path){Bmp bmp(path);__fromBmp(bmp);}
    Mat(Bmp &bmp){__fromBmp(bmp);}
    //图像信息获取与编辑
    DWORD size(){return sz;}
    DWORD rows(){return R;}
    DWORD cols(){return C;}
    BYTE get(DWORD r,DWORD c){return dat[__idx(r,c)];}
    void set(DWORD r,DWORD c,BYTE val){dat[__idx(r,c)]=val;}
    BYTE* operator[](DWORD r){return dat+r*C;}
    BYTE* operator[](DWORD r) const {return dat+r*C;}
    BYTE* getDatptr(){return dat;}
    //文件操作
    Bmp toBmp(){
        Bmp bmp(C,R,24);
        DWORD lw1=bmp.getLineSize();
        int i,j;BYTE* p2=dat;
        for(i=R-1;i>=0;i--){
            BYTE* p1=bmp.getDatptr()+i*lw1;
            for(j=0;j<C;j++){
                *(p1)=*p2;*(p1+1)=*p2;*(p1+2)=*p2;
                p1+=3;p2++;
            }
        }
        return bmp;
    }
    void save(const char *path){toBmp().save(path);}
    void saveHistogram(const char* path){
        int mx=0;
        DWORD hist[256];
        histogram(hist);
        for(int i=0;i<256;++i){mx=std::max((int)hist[i],mx);}
        Mat hmat(256,512);
        for(int i=0;i<256;++i){
            int h=hist[i]*256/mx;
            for(int j=255;j>255-h;--j){
                hmat[j][i<<1]=255;
            }
        }
        hmat.pointTrans([](BYTE a){return abs(a-255);});
        hmat.save(path);
    }
    //矩阵运算
    Mat operator+(const Mat &m){
        if(R!=m.R ||C!=m.C) throw "Mat::size_not_match";
        Mat ret(R,C);
        for(int i=0;i<R;++i)
            for(int j=0;j<C;++j)
                ret[i][j]=dat[i*C+j]+m[i][j];
        return ret;
    }
    //空间域点变换
    void pointTrans(std::function<int(BYTE)> func){
        BYTE* p=dat,*pend=dat+sz;
        while(p<pend) {int x=func(*p);*(p++)=byte_cast(x);}
    }
    void threshold(DWORD thre){
        pointTrans([&](BYTE x)->BYTE{return x>=thre?255:0;});
    }
    void histogram(DWORD hist[256]){
        memset(hist,0,sizeof(DWORD)*256);
        BYTE* p=dat,*pend=dat+sz;
        while(p<pend){++hist[*(p++)];}
    }
    void equalizeHistogram(){
        DWORD hist[256],f[256];
        histogram(hist);
        DWORD s=0,i;
        double C=256.0/sz;
        for(i=0;i<256;i++){s+=hist[i];f[i]=s*C;}
        BYTE* p=dat,*pend=dat+sz;
        while(p<pend){*p=f[*p];++p;}
    }
    //空间域滤波
    Mat filter(FilterCore &core){
        Mat ret(R,C);
        int sz=core.size(),hsz=sz>>1;
        int md=core.getOutRangeMode();
        for(int i=hsz;i<R-hsz;i++){
            for(int j=hsz;j<C-hsz;j++){
                float s=0;
                for(int k=0;k<sz;k++)
                    for(int l=0;l<sz;l++)
                        s+=(*this)[i+k-hsz][j+l-hsz]*core[k][l];
                if(md==0)ret[i][j]=byte_cast(s);
                else if(md==-1)ret[i][j]=byte_cast2(s);
                else ret[i][j]=byte_cast(s+md);
            }
        }
        return ret;
    }
    Mat filter(FilterS &filt){
        Mat ret(R,C);
        int sz=filt.getSize();
        if(!(sz&1)) throw "Mat::bad_filtercore_size";
        int hsz=sz>>1;
        std::vector<BYTE> tmp(sz*sz,0);
        for(int i=hsz;i<R-hsz;i++){
            for(int j=hsz;j<C-hsz;j++){
                for(int k=0;k<sz;k++)
                    for(int l=0;l<sz;l++)
                        tmp[k*sz+l]=(*this)[i+k-hsz][j+l-hsz];
                ret[i][j]=byte_cast(filt.proc(tmp));
            }
        }
        return ret;
    }
    Mat averageFilter(DWORD ksize){
        if(!(ksize&1)) throw "Mat::bad_filter_size";
        FilterCore fc(ksize);int s2=ksize*ksize;
        for(int i=0;i<ksize;++i)
            for(int j=0;j<ksize;++j)
                fc[i][j]=1.0/s2;
        return filter(fc);
    }
    Mat medianFilter(DWORD ksize){
        if(!(ksize&1)) throw "Mat::bad_filter_size";
        FilterS filt(ksize,[&](std::vector<BYTE> &v)->int{
            auto it=v.begin()+ksize*ksize/2;
            std::nth_element(v.begin(),it,v.end());
            return *it;
        });
        return filter(filt);
    }
    //边缘提取
    Mat sobelX(int out_range_mode=FILTER_OUTRANGE_CUT){
        static FilterCore fc("-1 0 1;-2 0 2;-1 0 1",1,out_range_mode);
        return filter(fc);
    }
    Mat sobelY(int out_range_mode=FILTER_OUTRANGE_CUT){
        static FilterCore fc("1 2 1;0 0 0;-1 -2 -1",1,out_range_mode);
        return filter(fc);
    }
    Mat sobel(int out_range_mode=FILTER_OUTRANGE_CUT){
        return sobelX(out_range_mode)+sobelY(out_range_mode);
    }
    Mat priwittX(int out_range_mode=FILTER_OUTRANGE_CUT){
        static FilterCore fc("-1 0 1;-1 0 1;-1 0 1",1,out_range_mode);
        return filter(fc);
    }
    Mat priwittY(int out_range_mode=FILTER_OUTRANGE_CUT){
        static FilterCore fc("-1 -1 -1;0 0 0;1 1 1",1,out_range_mode);
        return filter(fc);
    }
    Mat priwitt(int out_range_mode=FILTER_OUTRANGE_CUT){
        return priwittX(out_range_mode)+priwittY(out_range_mode);
    }
    Mat laplacian(int out_range_mode=FILTER_OUTRANGE_CUT){
        static FilterCore fc("-1 -1 -1;-1 8 -1;-1 -1 -1",1,out_range_mode);
        return filter(fc);
    }
    //形态学
    Mat morph(MorphCore &mm,MorphFunc &mf){
        Mat ret(R,C);
        int r=mm.getRows(),c=mm.getCols(),sz=mm.getSize(),
            mr=mm.getCenterRow(),mc=mm.getCenterCol();
        for(int i=0;i<R;++i){
            for(int j=0;j<C;++j){
                int cnt0=0,cnt1=0;
                for(int k=0;k<mr;++k){
                    for(int l=0;l<mc;++l){
                        int x=i+k-mr,y=j+l-mc;
                        if(mm[k][l]=='.' || x<0 || x>=R || y<0 || y>=C) continue;
                        if(dat[__i(x,y)]) ++cnt1;
                        else ++cnt0;
                    }
                }
                ret[i][j]=mf(cnt0,cnt1);
            }
        }
        return ret;
    }
    Mat dilate(MorphCore &mm){
        static MorphFunc dil=[](int cnt0,int cnt1)->BYTE{return cnt1?255:0;};
        return morph(mm,dil);
    }
    Mat erode(MorphCore &mm){
        static MorphFunc ero=[](int cnt0,int cnt1)->BYTE{return (!cnt0)?255:0;};
        return morph(mm,ero);
    }
    Mat close(MorphCore &mm){return dilate(mm).erode(mm);}
    Mat open(MorphCore &mm){return erode(mm).dilate(mm);}
    //噪声发生器
    void addPepperNoise(float density){
        int cnt=sz*density;
        std::uniform_int_distribution<int> row(0,R-1),col(0,C-1),val(0,1);
        while(cnt--){(*this)[row(__rnd)][col(__rnd)]=val(__rnd)*255;}
    }
    void addGaussianNoise(float mean,float var){
        std::normal_distribution<float> norm(mean, var);
        BYTE* p=dat,*pend=dat+sz;
        while(p<pend){int x=*p+norm(__rnd);*(p++)=byte_cast(x);}
    }
    ~Mat(){delete[] dat;}
};

#endif

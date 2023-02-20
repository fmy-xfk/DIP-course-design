#ifndef __MATF_H__
#define __MATF_H__

#include "base.h"
#include "filter.h"
#include "mat.h"
#include "fft.h"

class MatF{
	DWORD R,C,sz;
	cpx *dat;
	FFTWrapper fwr,fwc;
public:
    MatF(Mat &mat):R(mat.rows()),C(mat.cols()),fwr(mat.rows()),fwc(mat.cols()){
        dat=new cpx[sz=R*C];
        for(int i=0;i<R;++i) fwc.fft(mat[i],dat+i*C);
        for(int j=0;j<C;++j) fwr.fft(dat+j,C,dat+j,C);
    }
    MatF(const MatF &matf):fwr(matf.R),fwc(matf.C){
        R=matf.R;C=matf.C;dat=new cpx[sz=R*C];
        memcpy(dat,matf.dat,sz*sizeof(cpx));
    }
	void save(const char *path){
        Mat mat(R,C);double mx=0;
        double *buf=new double[sz],*p=buf;
        cpx *pc=dat,*pcend=dat+sz;
        while(pc<pcend){
            *p=pc->logmod();
            mx=std::max(mx,*p);
            ++p;++pc;
        }
        p=buf;
        BYTE *pb=mat.getDatptr(),*pbend=pb+mat.size();
        while(pb<pbend) *pb++=(*p++)*255/mx;
        delete[] buf;
        mat.save(path);
    }
	Mat toMat(){
        Mat mat(R,C);cpx *dat2=new cpx[sz];
        memcpy(dat2,dat,sz*sizeof(cpx));
        for(int i=0;i<R;++i) fwc.ifft(dat2+i*C);
        for(int j=0;j<C;++j) fwr.ifft(dat2+j,C,mat.getDatptr()+j,C);
        return mat;
    }
	void filter(FilterF ff){
        int hR=R/2,hC=C/2;
        for(int i=0;i<hR;++i){
            cpx* p=dat+i*C;
            for(int j=0;j<hC;++j){
                double d=sqrt(i*i+j*j);
                p[j]=p[j]*ff(d);
            }
        }
        for(int i=hR;i<R;++i){
            cpx* p=dat+i*C;
            for(int j=0;j<hC;++j){
                double d=sqrt((R-1-i)*(R-1-i)+j*j);
                p[j]=p[j]*ff(d);
            }
        }
        for(int i=0;i<hR;++i){
            cpx* p=dat+i*C;
            for(int j=hC;j<C;++j){
                double d=sqrt(i*i+(C-1-j)*(C-1-j));
                p[j]=p[j]*ff(d);
            }
        }
        for(int i=hR;i<R;++i){
            cpx* p=dat+i*C;
            for(int j=hC;j<C;++j){
                double d=sqrt((R-1-i)*(R-1-i)+(C-1-j)*(C-1-j));
                p[j]=p[j]*ff(d);
            }
        }
    }
    void idealFilter(double d,bool lpf=true){
        if(lpf){
            filter([&](double dist)->double{return dist<=d?1:0;});
        }else{
            filter([&](double dist)->double{return dist<=d?0:1;});
        }
    }
    void butterworthFilter(int n,double d0,bool lpf=true){
        if(lpf){
            filter([&](double dist)->double{return 1.0/(1+pow(dist/d0,2*n));});
        }else{
            filter([&](double dist)->double{return 1.0/(1+pow(d0/dist,2*n));});
        }
    }
    void gaussianFilter(int n,double d0,bool lpf=true){
        if(lpf){
            filter([&](double dist)->double{return exp(-dist*dist/(2*d0*d0));});
        }else{
            filter([&](double dist)->double{return 1-exp(-dist*dist/(2*d0*d0));});
        }
    }
    ~MatF(){delete[] dat;}
};

#endif

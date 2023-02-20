#ifndef __FFT_H__
#define __FFT_H__

#include "base.h"
using namespace std;

const double pi=acos(-1);
const double eps=1e-6;

//复数类
struct cpx{
    double x,y;
    cpx(double x=0,double y=0):x(x),y(y){}
    cpx operator+(cpx b){return cpx(x+b.x,y+b.y);}
    cpx operator-(cpx b){return cpx(x-b.x,y-b.y);}
    cpx operator*(cpx b){return cpx(x*b.x-y*b.y,x*b.y+y*b.x);}
	cpx operator*(double d){return cpx(x*d,y*d);}
	cpx operator/(int n){return cpx(x/n,y/n);}
	double modulus(){return sqrt(x*x+y*y);}
	double logmod(){return log(1+modulus());}
};

//基2 FFT
void __b2fft(cpx A[],const int r[],const int limit,const int type) {
    for(int i=0;i<limit;i++) 
        if(i<r[i]) swap(A[i],A[r[i]]);
    for(int mid=1;mid<limit;mid<<=1)
    {
        cpx Wn(cos(pi/mid), type*sin(pi/mid));
        for(int R=mid<<1,j=0;j<limit;j+=R)
        {
            cpx w(1,0);
            for(int k=0;k<mid;k++,w=w*Wn)
            {
                cpx x=A[j+k],y=w*A[j+mid+k];
                A[j+k]=x+y;
                A[j+mid+k]=x-y;
            }
        }
    }
    if(type==-1){
    	for(int i=0;i<limit;++i) A[i]=A[i]/limit;
	}
}

//生成蝴蝶操作所用的反转数组
int __gen_rev(int n,int* &rev){
	int N=1,L=0;
	while(N<n){++L;N<<=1;}
	rev=new int[N]; rev[0]=0;
	for(int i=0;i<N;i++) rev[i]=(rev[i>>1]>>1)|((i&1)<<(L-1));
	return N;
}

//Bluestein法任意基FFT
//https://www.cnblogs.com/birchtree/p/12470386.html
void __bfft(cpx a[],int n,const int rev[],int N,int type){
    cpx *x=new cpx[N],*y=new cpx[N];
	for(int i=0;i<n;i++) x[i]=cpx(cos(pi*i*i/n),type*sin(pi*i*i/n))*a[i];
	for(int i=0;i<n*2;i++) y[i]=cpx(cos(pi*(i-n)*(i-n)/n),-type*sin(pi*(i-n)*(i-n)/n));
	__b2fft(x,rev,N,1);
	__b2fft(y,rev,N,1);
	for(int i=0;i<N;i++) x[i]=x[i]*y[i];
	__b2fft(x,rev,N,-1);
	for(int i=0;i<n;i++){
		a[i]=x[i+n]*cpx(cos(pi*i*i/n),type*sin(pi*i*i/n));
		if(type==-1) a[i]=a[i]/n;
	}
	delete[] x;
	delete[] y;
}

//使用已经生成好的反转数组进行任意基FFT
void __fft_no_rev(int n,cpx a[],int m,int rev[],int type){
	if(m==n) __b2fft(a,rev,n,type);
	else __bfft(a,n,rev,m,type);
}

//任意基FFT
void fft(int n,cpx a[]){
	int *rev;
	int m=__gen_rev(n<<2,rev);
	__fft_no_rev(n,a,m,rev,1);
}

//任意基iFFT
void ifft(int n,cpx a[]){
	int *rev;
	int m=__gen_rev(n<<2,rev);
	__fft_no_rev(n,a,m,rev,-1);
}

//FFT封装类：给定长度，预先计算好反转数组，减少计算量
class FFTWrapper{
	int n,m,*rev;
public:
	FFTWrapper(DWORD _n):n(_n){
		if(__builtin_popcount(n)==1){
			m=__gen_rev(n,rev);
		}else{
			m=__gen_rev(n<<2,rev);
		}
	}
	void fft(cpx a[]) {__fft_no_rev(n,a,m,rev,1);}
	void ifft(cpx a[]){__fft_no_rev(n,a,m,rev,-1);}
	void fft(BYTE a[],cpx dst[]){
		for(int i=0;i<n;++i)
			dst[i]=cpx(a[i],0);
		fft(dst);
	}
	void fft(cpx* src_beg,int src_step,cpx* dst_beg,int dst_step){
		cpx *ret=new cpx[n];
		for(int i=0;i<n;++i) ret[i]=src_beg[i*src_step];
		fft(ret);
		for(int i=0;i<n;++i) dst_beg[i*dst_step]=ret[i];
		delete[] ret;
	}
	void ifft(BYTE dst[],cpx a[]){
		ifft(a);
		for(int i=0;i<n;++i) {
			int x=round(a[i].x);
			dst[i]=byte_cast(x);
		}
	}
	void ifft(cpx* src_beg,int src_step,BYTE* dst_beg,int dst_step){
		cpx *ret=new cpx[n];
		for(int i=0;i<n;++i) ret[i]=src_beg[i*src_step];
		ifft(ret);
		for(int i=0;i<n;++i) {
			int x=round(ret[i].x);
			dst_beg[i*dst_step]=byte_cast(x);
		}
	}
	~FFTWrapper(){delete[] rev;}
};

#endif

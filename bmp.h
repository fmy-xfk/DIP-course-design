#ifndef __BMP_H__
#define __BMP_H__

#include "base.h"

typedef struct _bmp_file_header{
    WORD    Type;           //Always "BM"
    DWORD   Size;
    WORD    Reserved1;      //Always 0
    WORD    Reserved2;      //Always 0
    DWORD   OffBits;
}BmpFileHeader;

#define BMP_COMPRESSION_RGB         0
#define BMP_COMPRESSION_RLE8        1
#define BMP_COMPRESSION_RLE4        2
#define BMP_COMPRESSION_BITFIELDS   3
#define BMP_COMPRESSION_JPEG        4
#define BMP_COMPRESSION_PNG         5

typedef struct _bmp_info_header{
    DWORD   Size;           //Always 40
    LONG    Width;
    LONG    Height;
    WORD    Planes;         //Always 1
    WORD    BitCount;       //1,4,8,16,24,32
    DWORD   Compression;    //0,1,2,3,4,5
    DWORD   SizeOfImage;
    LONG    XPelsPerMeter;
    LONG    YPelsPerMeter;
    DWORD   ColorUsed;
    DWORD   ColorImportant;
}BmpInfoHeader;

typedef struct _rgba_quad {
    BYTE    Blue;
    BYTE    Green;
    BYTE    Red;
    BYTE    Alpha;
}BgraQuad;

typedef struct _bmp_palette{
    BgraQuad Colors[256];
}BmpPalette;

class Bmp{
    BYTE* dat;
    DWORD datsz;
    BmpFileHeader FileHeader;
    BmpInfoHeader InfoHeader;
    DWORD __calc_datsz(){datsz=getLineSize()*InfoHeader.Height;}
    void __init_wh(DWORD w,DWORD h,WORD bpp){
        if(bpp!=8 && bpp!=24) throw "Bmp::bad_bpp";
        InfoHeader.Size=40;
        InfoHeader.Width=w;
        InfoHeader.Height=h;
        InfoHeader.Planes=1;
        InfoHeader.BitCount=bpp;
        InfoHeader.Compression=0;
        InfoHeader.SizeOfImage=0;
        InfoHeader.XPelsPerMeter=0;
        InfoHeader.XPelsPerMeter=0;
        InfoHeader.ColorUsed=0;
        InfoHeader.ColorImportant=0;
        __calc_datsz();
        FileHeader.Type=0x4d42;
        FileHeader.Size=14+40+datsz;
        FileHeader.Reserved1=0; 
        FileHeader.Reserved2=0; 
        FileHeader.OffBits=54; 
        dat=new BYTE[datsz];
    }
    void __load(const char *path){
        FILE* fp=fopen(path,"rb");
        if(fp==NULL) throw "Bmp::bad_path";
        fread(&FileHeader.Type,2,1,fp);
        fread(&FileHeader.Size,4,1,fp);
        fread(&FileHeader.Reserved1,2,1,fp);
        fread(&FileHeader.Reserved2,2,1,fp);
        fread(&FileHeader.OffBits,4,1,fp);
        if(FileHeader.Type!=0x4d42) throw "Bmp::bad_type";
        if(FileHeader.Reserved1) throw "Bmp::bad_reserved1";
        if(FileHeader.Reserved2) throw "Bmp::bad_reserved2";
        fread(&InfoHeader,sizeof(BmpInfoHeader),1,fp);
        __calc_datsz();
        dat=new BYTE[datsz];
        fread(dat,1,datsz,fp);
        fclose(fp);
    }
public:
    Bmp() = delete;
    Bmp(Bmp &bmp){
        datsz=bmp.datsz;
        FileHeader=bmp.FileHeader;
        InfoHeader=bmp.InfoHeader;
        memcpy(dat,bmp.dat,datsz);
    }
    Bmp(DWORD w,DWORD h,WORD bpp){__init_wh(w,h,bpp);}
    Bmp(std::string &path){__load(path.c_str());}
    Bmp(const char *path){__load(path);}
    DWORD getLineSize(){
        BYTE BYTES=InfoHeader.BitCount/8;
        if(BYTES<1) BYTES=1;
        DWORD ret=InfoHeader.Width*BYTES;
        while(ret&3) ++ret;
        return ret;
    }
    DWORD getWidth(){return InfoHeader.Width;}
    DWORD getHeight(){return InfoHeader.Height;}
    void save(const char *path){
        FILE* fp=fopen(path,"wb");
        if(fp==NULL) throw "Bmp::bad_path";
        fwrite(&FileHeader.Type,2,1,fp);
        fwrite(&FileHeader.Size,4,1,fp);
        fwrite(&FileHeader.Reserved1,2,1,fp);
        fwrite(&FileHeader.Reserved2,2,1,fp);
        fwrite(&FileHeader.OffBits,4,1,fp);
        fwrite(&InfoHeader,sizeof(BmpInfoHeader),1,fp);
        fwrite(dat,1,datsz,fp);
        fclose(fp);
    }
    void toGray(){
        DWORD lw1=getLineSize();
        int i,j;
        for(i=0;i<InfoHeader.Height;i++){
            BYTE* p1=dat+i*lw1;
            for(j=0;j<InfoHeader.Width;j++){
                *(p1+2)=*(p1+1)=*(p1)=(BYTE)((*(p1)*114+*(p1+1)*587+*(p1+2)*299)/1000);
                p1+=3;
            }
        }
    }
    BYTE* getDatptr(){return dat;}
    ~Bmp(){delete[] dat;}
};
#endif

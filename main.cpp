#include "imgproc.h"
using namespace std;

int main(){
	cout<<"equalize histogram"<<endl;
	{
	    Mat mat("src.bmp");
	    mat.save("gray.bmp");
		mat.saveHistogram("h_gray.bmp");
	    mat.equalizeHistogram();
	    mat.save("eqhist.bmp");
		mat.saveHistogram("h_eqed.bmp");
    }
	cout<<"threshold & morphology"<<endl;
	{
		MorphCore mc("ooo;oxo;ooo");
		MorphCore mc2("ooooo;ooooo;ooxoo;ooooo;ooooo");
	    Mat mat("src.bmp");
	    mat.threshold(128);
	    mat.save("thre.bmp");
	    mat.dilate(mc).save("dilate3.bmp");
	    mat.erode(mc).save("erode3.bmp");
	    mat.open(mc).save("open3.bmp");
	    mat.close(mc).save("close3.bmp");
	    mat.dilate(mc2).save("dilate5.bmp");
	    mat.erode(mc2).save("erode5.bmp");
	    mat.open(mc2).save("open5.bmp");
	    mat.close(mc2).save("close5.bmp");
    }
    cout<<"filter"<<endl;
	{
		FilterCore wavgc("1 2 1;2 4 2;1 2 1",16);
		Mat mat("src.bmp");
		mat.medianFilter(3).save("med_filt3.bmp");
	    mat.averageFilter(3).save("avg_filt3.bmp");
	    mat.medianFilter(5).save("med_filt5.bmp");
	    mat.averageFilter(5).save("avg_filt5.bmp");
	    mat.filter(wavgc).save("wavg_filt.bmp");
	}
    cout<<"smoothen"<<endl;
    {
    	FilterCore wavgc("1 2 1;2 4 2;1 2 1",16);
	    Mat mat("src.bmp");
	    mat.addGaussianNoise(0,16);
	    mat.save("g_noise.bmp");
	    mat.medianFilter(3).save("g_noise_med_filt.bmp");
	    mat.averageFilter(3).save("g_noise_avg_filt.bmp");
	    mat.filter(wavgc).save("g_noise_wavg_filt.bmp");
	    
	    Mat mat2("src.bmp");
	    mat2.addPepperNoise(0.01);
	    mat2.save("p_noise.bmp");
	    mat2.medianFilter(3).save("p_noise_med_filt.bmp");
	    mat2.averageFilter(3).save("p_noise_avg_filt.bmp");
	    mat2.filter(wavgc).save("p_noise_wavg_filt.bmp");
	}
	cout<<"contour"<<endl;
    {
	    Mat mat("src.bmp");
	    mat.sobel().save("sobel.bmp");
	    mat.priwitt().save("priwitt.bmp");
	    mat.laplacian().save("laplacian.bmp");
	}
	cout<<"freq"<<endl;
	Mat mat("src.bmp");
	MatF src(mat);
	src.save("freq.bmp");
	cout<<"ilpf"<<endl;
	{
		MatF matf(src);
		matf.idealFilter(50);
		matf.save("freq_ideal_lpf.bmp");
		matf.toMat().save("f_ideal_lpf.bmp");
	}
	cout<<"ihpf"<<endl;
	{
		MatF matf(src);
		matf.idealFilter(20,false);
		matf.save("freq_ideal_hpf.bmp");
		matf.toMat().save("f_ideal_hpf.bmp");
	}
	cout<<"bwlpf"<<endl;
	{
		MatF matf(src);
		matf.butterworthFilter(2,20);
		matf.save("freq_bw_lpf.bmp");
		matf.toMat().save("f_bw_lpf.bmp");
	}
	cout<<"bwhpf"<<endl;
	{
		MatF matf(src);
		matf.butterworthFilter(2,50,false);
		matf.save("freq_bw_hpf.bmp");
		matf.toMat().save("f_bw_hpf.bmp");
	}
	cout<<"glpf"<<endl;
	{
		MatF matf(src);
		matf.gaussianFilter(2,20);
		matf.save("freq_g_lpf.bmp");
		matf.toMat().save("f_g_lpf.bmp");
	}
	cout<<"ghpf"<<endl;
	{
		MatF matf(src);
		matf.gaussianFilter(2,50,false);
		matf.save("freq_g_hpf.bmp");
		matf.toMat().save("f_g_hpf.bmp");
	}
    return 0;
}

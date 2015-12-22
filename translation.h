#pragma once
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/core.hpp"
#include"opencv2/highgui/highgui.hpp"
#include"fftw3.h"
#include<vector>
#include<algorithm>
#include<numeric>
#include<iostream>
#include<fstream>
//#define  MATTYPE CV_16UC1
//typedef  unsigned short PIXTYPE;
#define  MATTYPE CV_8UC1
typedef  unsigned char PIXTYPE;

extern const int RATIO;
#define CHECK_MAT(M) if(M.data==NULL) {std::cout<<"��ͼƬʧ�ܣ��˳�"<<std::endl;return 1;}
/*
	ʹ�÷�������Ĥ���б�Ե����
*/
cv::Mat EdgeEnhance(cv::Mat mat);
/*���һ��ͼ���harris�ǵ��ֵͼ��*/
cv::Mat GetHarris(const cv::Mat& mat);
/*
	����translationParams��imges��ͼ������ں�
*/
cv::Mat Fuse(const std::vector<cv::Mat>& images, std::vector<std::pair<int, int>> translationParams);
/*
	�ں�roi1��roi2�������ŵ�roiResult�У�roi1��roi2���ж���ͬ��ǰ���� roiResult�Ѿ������ڴ�
*/
void FuseFadeOut(const cv::Mat& roi1, const cv::Mat& roi2,cv::Mat& roiResult);
/*
	the fade in and fade out methode is so abrupt,the seam is still obvious when the exposures between images are very different.
	Use a S curv instead straight line can make better result.
*/
void FuseCurve(const cv::Mat& roi1, const cv::Mat& roi2, cv::Mat& roiResult);
/*
ʹ�ö�ֱ��ʶ�roi1,��roi2�����ں�,�����ŵ�roiResult�У�ǰ���� roiResult�Ѿ������ڴ�
*/
void FuseMultisolution(const cv::Mat& roi1, const cv::Mat& roi2, cv::Mat& roiResult);
/*
	�������ͼ������Images������׼��������ͼ���ŵ�imageOut�У�imageOut����������ͼ��׼�����ŵ�TranslationParams��
*/
void Register(const std::vector<cv::Mat>& images, std::vector<cv::Mat>& imagesOut, std::vector<std::pair<int, int>>& TranslationParams);
/*
	��images�е�ͼ���ж�һ�»���
*/
std::vector<cv::Mat> ColsUnitize(const std::vector<cv::Mat>& images);
/*
	��mat1��mat2��Сͳһ��mat1ȡ�����°벿�֣�mat2ȡ�����ϰ벿��
	mat1out-(out)ͳһ��С��mat1�����
	mat2out-(out)ͳһ��С��mat2�����
*/
void ColsRowsUnitize(const cv::Mat& mat1, const cv::Mat& mat2, cv::Mat& mat1out, cv::Mat& mat2out);
/*
	�����Ȥ����mat
*/
cv::Mat GetRows(const cv::Mat& mat,int start,int end);
/*
	��image1��image2����ƽ�����������ŵ�x,y��,����ͼ���Сһ��
	���ڸ���Ҷ�������ԣ���������ƽ��Ϊ��.����ʱ��һ����Image1���ϣ�image2���£�y>0,���������ˮƽƽ�ƴ���2/cols��������x<0
*/
void FFTTranslation(const cv::Mat& image1, const cv::Mat& image2,int& x,int& y);

bool WriteMatFile(const cv::Mat& mat, const std::string& fileName);
#pragma once
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/core.hpp"
#include"opencv2/highgui/highgui.hpp"
#include"fftw3.h"
#include<vector>
#include<algorithm>
#include<numeric>
#include<iostream>
#define  MATTYPE CV_16UC1
typedef  unsigned short PIXTYPE;
//typedef  unsigned char PIXTYPE;
extern const int RATIO;
#define CHECK_MAT(M) if(M.data==NULL) {std::cout<<"��ͼƬʧ�ܣ��˳�"<<std::endl;return 1;}
/*
	����translationParams��imges��ͼ������ں�
*/
cv::Mat Fuse(const std::vector<cv::Mat>& images, std::vector<std::pair<int, int>> translationParams);
/*
	�ں�roi1��roi2�������ŵ�roiResult�У�roi1��roi2���ж���ͬ��ǰ���� roiResult�Ѿ������ڴ�
*/
void FuseFadeOut(const cv::Mat& roi1, const cv::Mat& roi2,cv::Mat& roiResult);
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
	�����Ȥ����mat
*/
cv::Mat GetRows(const cv::Mat& mat,int start,int end);
/*
	��image1��image2����ƽ�����������ŵ�x,y��,����ͼ���Сһ��
	���ڸ���Ҷ�������ԣ���������ƽ��Ϊ��.����ʱ��һ����Image1���ϣ�image2���£�y>0,���������ˮƽƽ�ƴ���2/cols��������x<0
*/
void FFTTranslation(const cv::Mat& image1, const cv::Mat& image2,int& x,int& y);
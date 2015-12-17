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
#define CHECK_MAT(M) if(M.data==NULL) {std::cout<<"打开图片失败，退出"<<std::endl;return 1;}
/*
	根据translationParams将imges中图像进行融合
*/
cv::Mat Fuse(const std::vector<cv::Mat>& images, std::vector<std::pair<int, int>> translationParams);
/*
	融合roi1、roi2将结果存放到roiResult中，roi1、roi2行列都相同，前提是 roiResult已经分配内存
*/
void FuseFadeOut(const cv::Mat& roi1, const cv::Mat& roi2,cv::Mat& roiResult);
/*
使用多分辨率对roi1,和roi2进行融合,结果存放到roiResult中，前提是 roiResult已经分配内存
*/
void FuseMultisolution(const cv::Mat& roi1, const cv::Mat& roi2, cv::Mat& roiResult);
/*
	对输入的图像序列Images进行配准，处理后的图像存放到imageOut中，imageOut中相邻两幅图配准结果存放到TranslationParams中
*/
void Register(const std::vector<cv::Mat>& images, std::vector<cv::Mat>& imagesOut, std::vector<std::pair<int, int>>& TranslationParams);
/*
	将images中的图像列都一致化了
*/
std::vector<cv::Mat> ColsUnitize(const std::vector<cv::Mat>& images);
/*
	获得兴趣区域mat
*/
cv::Mat GetRows(const cv::Mat& mat,int start,int end);
/*
	对image1、image2计算平移量，结果存放到x,y中,两幅图像大小一样
	由于傅立叶的周期性，向下向右平移为正.计算时，一定是Image1在上，image2在下，y>0,若计算出来水平平移大于2/cols就是向左x<0
*/
void FFTTranslation(const cv::Mat& image1, const cv::Mat& image2,int& x,int& y);
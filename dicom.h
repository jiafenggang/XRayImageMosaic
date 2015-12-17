#include"dcmtk\dcmdata\dctk.h"
#include"translation.h"
#include<string>
bool Dcm2Mat(const std::string& fileName, cv::Mat& mat, int* windowCenter = NULL, int* windowWidth = NULL);
cv::Mat Mat16ToMat8(const cv::Mat& mat16, const int windowCenter, const int windowWidth);
bool SaveMat2Dcm(const cv::Mat& mat, const std::string& fileIn, const std::string& fileOut);
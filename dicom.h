#include"dcmtk\dcmdata\dctk.h"
#include"translation.h"
#include<string>
/*
	extract pixel data from dicom file, and put it to the opencv data type "mat", if necessary return the window center and window width
	fileName-dicom file name
	mat-(out)destination mat
	windowCenter-(out)
	windowWidth-(out)
	return:if success, return true
*/
bool Dcm2Mat(const std::string& fileName, cv::Mat& mat, int* windowCenter = NULL, int* windowWidth = NULL);
/*
	convert CV_16UC1 Mat to CV_8UC1 Mat use the windowCenter and windowWidth, then CV_8UC1 mat can be show on windows
	return£ºreturn the  CV_8UC1 Mat
*/
cv::Mat Mat16ToMat8(const cv::Mat& mat16, const int windowCenter, const int windowWidth);
/*
	save the mat data to dicom file as the pixel data,note the mat type is CV_16U1,because almost of all dicom files are 16 bit(OW)
	mat-input mat,CV_16UC1
	fileIn-the name of the destination dicom file to be inserted
	fileOut-result dicom file name
	return: return true if success
*/
bool SaveMat2Dcm(const cv::Mat& mat, const std::string& fileIn, const std::string& fileOut);
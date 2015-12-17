#include"dcmtk\dcmdata\dctk.h"
#include"translation.h"
#include"dicom.h"
#include<ctime>

int main(){
	cv::Mat image1, image2, image3;
	int windowWidth = 0, windowCenter = 0;
	if (!Dcm2Mat("1_1.dcm", image1, &windowCenter, &windowWidth)){
		return 1;
	}
	if (!Dcm2Mat("1_2.dcm", image2)){
		return 1;
	}
	if (!Dcm2Mat("1_3.dcm", image3)){
		return 1;
	}
	std::vector<cv::Mat> images;
	images.push_back(image1);
	images.push_back(image2);
	images.push_back(image3);
	std::vector<cv::Mat> imagesout;
	std::vector<std::pair<int, int>> translationParams;
	Register(images, imagesout, translationParams);
	std::for_each(translationParams.begin(), translationParams.end(),
		[](const std::pair<int, int>& param){std::cout << param.first << ":" << param.second << std::endl; });
	cv::Mat result=Fuse(imagesout, translationParams);
	SaveMat2Dcm(result, "1_1.dcm", "2.dcm");
	cv::Mat bitresult = Mat16ToMat8(result, windowCenter, windowWidth);
	cv::imwrite("dcmresult.bmp", bitresult);
	return 0;
}
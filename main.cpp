//#include"translation.h"
//#include<ctime>
//int main(){
//	cv::Mat image1 = cv::imread("1_1.bmp", CV_8UC1);
//	cv::Mat image2 = cv::imread("1_2.bmp", CV_8UC1);
//	cv::Mat image3 = cv::imread("1_3.bmp", CV_8UC1);
//	std::vector<cv::Mat> images;
//	images.push_back(image1);
//	images.push_back(image2);
//	images.push_back(image3);
//	std::vector<cv::Mat> imagesOut;
//	std::vector<std::pair<int, int>> translationParams;
//	clock_t start, end;
//	start = clock();
//	Register(images, imagesOut, translationParams); 
//	cv::Mat result = Fuse(imagesOut, translationParams);
//	end=clock();
//	std::for_each(translationParams.begin(), translationParams.end(), [](const std::pair<int,int>& param){
//		std::cout << "x=" << param.first << ",y=" << param.second << std::endl;
//	});
//	std::cout << "Ê±¼ä=" << (double)(end - start) / CLOCKS_PER_SEC << std::endl;
//	cv::imwrite("result.bmp", result);
//	return 0;
//}
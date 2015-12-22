#include"translation.h"
const int RATIO = 4;

void FFTTranslation(const cv::Mat& image1, const cv::Mat& image2, int& x, int& y){
	const int rows = image1.rows;
	const int cols = image1.cols;
	PIXTYPE* pImage1 = (PIXTYPE*)image1.data;
	PIXTYPE* pImage2 = (PIXTYPE*)image2.data;
	const unsigned int memSize = rows*cols*sizeof(fftw_complex);
	const unsigned int fSize = rows*cols;
	fftw_complex* image1In, *image2In, *image1Out, *image2Out, *crossIn, *crossOut;
	image1In = (fftw_complex*)fftw_malloc(memSize);
	image2In = (fftw_complex*)fftw_malloc(memSize);
	image1Out = (fftw_complex*)fftw_malloc(memSize);
	image2Out = (fftw_complex*)fftw_malloc(memSize);
	crossIn = (fftw_complex*)fftw_malloc(memSize);
	crossOut = (fftw_complex*)fftw_malloc(memSize);
	fftw_plan p1 = fftw_plan_dft_1d(fSize, image1In, image1Out,FFTW_FORWARD,FFTW_ESTIMATE);
	fftw_plan p2 = fftw_plan_dft_1d(fSize, image2In, image2Out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_plan p3 = fftw_plan_dft_1d(fSize, crossIn, crossOut, FFTW_BACKWARD, FFTW_ESTIMATE);
	//����image1��image2������
	int j = 0;
	assert(image1.isContinuous());
	for (j = 0; j < fSize; ++j){
		image1In[j][0] = pImage1[j];
		image1In[j][1] = 0;
		image2In[j][0] = pImage2[j];
		image2Out[j][1] = 0;
	}
	//image1��image2���и���Ҷ�任
	fftw_execute(p1);
	fftw_execute(p2);
	/*���㽻�湦����*/
	//(a+bi)��(c+di)��(a+bi)(c-di)/|(a+bi)(c-di)|
	//(a+bi)(c-di)= ac+bd-adi+bci
	double temp = 0;
	
	for (j = 0; j < fSize; ++j){
		crossIn[j][0] = image1Out[j][0] * image2Out[j][0] + image1Out[j][1] * image2Out[j][1];
		crossIn[j][1] = image1Out[j][1] * image2Out[j][0] - image1Out[j][0] * image2Out[j][1];
		temp = sqrt(pow(crossIn[j][0], 2) + pow(crossIn[j][1], 2));
		crossIn[j][0] /= temp;
		crossIn[j][1] /= temp;
	}
	/*���湦���׷�����Ҷ�任*/
	fftw_execute(p3);


	cv::Mat mat(rows, cols, CV_64FC1, cv::Scalar(0));
	double* matData=(double*) mat.data;
	/*��ȡ��������*/
	temp = INT_MIN;
	int j0 = 0;
	for (j = 0; j < fSize; ++j){
		crossOut[j][0] /= (double)fSize;

		matData[j] = crossOut[j][0];

		if (crossOut[j][0]>temp){
			temp = crossOut[j][0];
			j0 = j;
		}
	}
	WriteMatFile(mat, "pulse.mat");
	unsigned int x0 = 0, y0 = 0;
	y0 = j0 / cols;
	x0 = j0%cols;
	y = y0;
	x = x0;
	if (x0 > (cols / 2)){
		x = -(cols - x0);
	}
	
	//�����ڴ�
	fftw_destroy_plan(p1);
	fftw_destroy_plan(p2);
	fftw_destroy_plan(p3);
	fftw_free(image1In);
	fftw_free(image1Out);
	fftw_free(image2In);
	fftw_free(image2Out);
	fftw_free(crossIn);
	fftw_free(crossOut);
}
bool WriteMatFile(const cv::Mat& mat, const std::string& fileName){
	std::ofstream out;
	out.open(fileName.c_str(), std::ofstream::binary);
	if (!out){
		std::cerr << "faild to open the file: " << fileName << std::endl;
		return false;
	}
	//doubleռ8���ֽ�
	cv::Mat mat2;
	mat.convertTo(mat2, CV_64FC1);
	int rows = mat2.rows;
	int cols = mat2.cols;
	if (mat2.isContinuous()){
		cols = rows*cols;
		rows = 1;
	}
	for (int r = 0; r < rows; ++r){
		const char* ptr = (char*)mat2.ptr(r);
		out.write(ptr, cols*mat2.elemSize());
	}
	out.close();
	return true;
}
cv::Mat GetRows(const cv::Mat& mat, int start, int end){
	cv::Mat roi = mat.rowRange(start, end);
	cv::Mat duplicateMat = roi.clone();
	return duplicateMat;
}

cv::Mat GetHarris(const cv::Mat& mat){
	//1.�õ�harris�ǵ����
	cv::Mat cornerStrength;
	//3-��˹ģ���С��3-sobelģ���С��0.05-k
	cv::cornerHarris(mat, cornerStrength, 3, 3, 0.1);
	//2.�ǵ�����оֲ�����ֵ���
	cv::Mat dilated;
	cv::dilate(cornerStrength, dilated, cv::Mat());
	//�ֲ�����ֵ����
	cv::Mat localMask;
	cv::compare(cornerStrength, dilated, localMask, cv::CMP_EQ);
	//3.�ǵ������ֵ����
	double mins, maxs;
	cv::minMaxLoc(cornerStrength, &mins, &maxs);
	//0.00001*maxs -�ǵ���ֵ
	cv::threshold(cornerStrength, cornerStrength, 0.002*maxs, 255, CV_THRESH_BINARY);
	//4.ȥ���Ǽ���ֵ�ǵ�
	cornerStrength.convertTo(cornerStrength, CV_8U);
	cv::bitwise_and(cornerStrength, localMask, cornerStrength);
	return cornerStrength;
}
std::vector<cv::Mat> ColsUnitize(const std::vector<cv::Mat>& images){
	auto minColsIter= std::min_element(images.begin(), images.end(), [](const cv::Mat& mat1,const cv::Mat& mat2){return mat1.cols<mat2.cols; });
	const unsigned int minCols = (*minColsIter).cols;
	std::vector<cv::Mat> imagesOut;
	for (auto iter = images.begin(); iter != images.end(); ++iter){
		const int start = ((*iter).cols - minCols) / 2;
		cv::Mat roi = (*iter).colRange(start, start + minCols);
		cv::Mat image = roi.clone();
		
		//cv::GaussianBlur(image, image, cv::Size(9, 9), 1);
		imagesOut.push_back(image);
	}
	return imagesOut;
}
/*
	��mat1��mat2��Сͳһ��mat1ȡ�����°벿�֣�mat2ȡ�����ϰ벿��
	mat1out-(out)ͳһ��С��mat1�����
	mat2out-(out)ͳһ��С��mat2�����
*/
void ColsRowsUnitize(const cv::Mat& mat1, const cv::Mat& mat2, cv::Mat& mat1out, cv::Mat& mat2out){
	cv::Mat roi1, roi2;
	const int rows = std::min(mat1.rows, mat2.rows);
	mat1.rowRange(mat1.rows - rows, mat1.rows).copyTo(roi1);
	mat2.rowRange(0, rows).copyTo(roi2);
	std::vector<cv::Mat> images;
	images.push_back(roi1);
	images.push_back(roi2);
	std::vector<cv::Mat> image2s=ColsUnitize(images);
	mat1out = image2s[0];
	mat2out = image2s[1];
}
void Register(const std::vector<cv::Mat>& images, std::vector<cv::Mat>& imagesOut, std::vector<std::pair<int, int>>& TranslationParams){
	imagesOut = ColsUnitize(images);
	auto maxRowsIter = std::max_element(imagesOut.begin(), imagesOut.end(), [](const cv::Mat& mat1, const cv::Mat& mat2){
		return mat1.rows < mat2.rows;
	});
	const int roiRows = maxRowsIter->rows / RATIO;
	for (auto iter = imagesOut.begin(); iter != imagesOut.end()-1; ++iter){
		//ÿ��ȥiter���°벿�֣�iter+1���ϰ벿��
		cv::Mat roi1 = (*iter).rowRange(iter->rows-roiRows, iter->rows);
		cv::Mat roi2 = (*(iter + 1)).rowRange(0, roiRows);
		//cv::imwrite("roi1.bmp", roi1);
		//cv::imwrite("roi2.bmp", roi2);
		int x = 0, y = 0;
		FFTTranslation(roi1, roi2, x, y);
		y += (*iter).rows - roiRows;
		TranslationParams.push_back(std::pair<int,int>(x, y));
	}
}

cv::Mat EdgeEnhance(cv::Mat mat){
	cv::Mat blur;
	cv::GaussianBlur(mat, blur, cv::Size(3, 3), 0);
	cv::Mat temp = mat - blur;
	return mat + temp;
}
cv::Mat Fuse(const std::vector<cv::Mat>& images, std::vector<std::pair<int, int>> translationParams){
	int x0 = translationParams[0].first;
	//xs��translationParams�ж�x���ۼƺͼ���
	std::vector<int> xs;
	xs.push_back(x0);
	for (auto iter = translationParams.begin() + 1; iter != translationParams.end(); ++iter){
		x0 += iter->first;
		xs.push_back(x0);
	}
	const int minX = *std::min_element(xs.begin(), xs.end());
	const int maxX = *std::max_element(xs.begin(), xs.end());
	//Ϊ���ս��mat����洢
	int colsAddtion = 0;
	if (minX*maxX < 0){
		colsAddtion = abs(minX) + abs(maxX);
	}
	else{
		colsAddtion = MAX(abs(minX), abs(maxX));
	}
	cv::Mat image0 = images[0];
	int cols = image0.cols + colsAddtion;
	int rows= 0;
	int sum = 0;
	const int imagesNum = images.size();
	std::for_each(translationParams.begin(), translationParams.end(), [&sum](const std::pair<int, int>& param){sum += param.second; });
	rows = sum + images[imagesNum-1].rows;
	cv::Mat result(rows, cols, MATTYPE, cv::Scalar(0));
	//��ʼ��
	int y0 = 0;
	x0 = 0;
	if (minX < 0){
		x0 = abs(minX);
	}
	cv::Mat roiResult = result(cv::Rect(x0, y0, image0.cols, image0.rows));
	image0.copyTo(roiResult);
	//�����ں�ÿ��ͼ��
	auto iter1 = translationParams.begin();
	auto iter2 = images.begin()+1;
	cv::Mat image1;
	int roiWidth = 0, roiHeight = 0;
	cv::Mat roi0, roi1, roiTemp;
	for (; iter1 != translationParams.end(); ++iter1, ++iter2){
		//ÿ�ε������ǽ�image1�ŵ�result�У�����image0�Ѿ���result����
		roiWidth = image0.cols - abs(iter1->first);
		roiHeight = image0.rows - iter1->second;
		image1 = *iter2;
		//ȡ�ü�����Ȥ����
		if (iter1->first >= 0){
			//ȡimage0���½�
			roiResult = result(cv::Rect(x0 + iter1->first, y0 + iter1->second, roiWidth, roiHeight));
			roi0 = image0(cv::Rect(iter1->first,iter1->second, roiWidth, roiHeight));
			//ȡimage1���Ͻ�
			roi1 = image1(cv::Rect(0,0,roiWidth,roiHeight));
		}
		else{
			//ȡimage0���½�
			roiResult = result(cv::Rect(x0, y0 + iter1->second, roiWidth, roiHeight));
			roi0 = image0(cv::Rect(0,iter1->second,roiWidth,roiHeight));
			//ȡimage1���Ͻ�
			roi1 = image1(cv::Rect(abs(iter1->first), 0, roiWidth, roiHeight));
		}
		//��image1�ŵ�result�У�����iter1��
		roiTemp = result(cv::Rect(iter1->first + x0, iter1->second + y0, image1.cols, image1.rows));
		image1.copyTo(roiTemp);
		//�ں�roi0��roi1��roiResult��
		//FuseFadeOut(roi0, roi1, roiResult);
		FuseCurve(roi0, roi1, roiResult);
		//Ϊ���ֵ���׼������
		x0 += iter1->first;
		y0 += iter1->second;
		image0=image1;
	}
	return result;
}

void FuseFadeOut(const cv::Mat& roi1, const cv::Mat& roi2, cv::Mat& roiResult){
	//���ý���������ratio1Ϊroi1�ı�����ͬ��ratio2
	double ratio1 = 0, ratio2 = 0;
	const int rows = roi1.rows;
	const int cols = roi1.cols;
	const int step = roi1.step;
	int offset = 0;
	int r = 0, c = 0;
	//�ڴ˴��漰����step��ÿ�в�ֻ��cols�������ˣ�����step����mat.ptr��ȫ
	for (r = 0; r < rows; ++r){
		ratio2 = (double)r / (double)rows;
		ratio1 = 1 - ratio2;
		const PIXTYPE* data1 = roi1.ptr<PIXTYPE>(r);
		const PIXTYPE* data2 = roi2.ptr<PIXTYPE>(r);
		PIXTYPE* dataResult = roiResult.ptr<PIXTYPE>(r);
		for (c = 0; c < cols; ++c){
			dataResult[c] = ratio1*data1[c] + ratio2*data2[c];
		}
	}
}

void FuseMultisolution(const cv::Mat& roi1, const cv::Mat& roi2, cv::Mat& roiResult){

}


void FuseCurve(const cv::Mat& roi1, const cv::Mat& roi2, cv::Mat& roiResult){
	const int rows = roi1.rows;
	const int cols = roi2.cols;
	int r = 0, c = 0;
	double rat = 0;
	const int center = rows / 2;
	int dist = 0;
	for (r = 0; r < rows; ++r){
		if (r <= center){
			dist = r;
			rat = 2 * (pow((double)dist / rows, 2));
		}
		else{
			dist = rows - r;
			rat = 1 - 2 * (pow((double)dist / rows, 2));
		}
		const PIXTYPE* data1 = roi1.ptr<PIXTYPE>(r);
		const PIXTYPE* data2 = roi2.ptr<PIXTYPE>(r);
		PIXTYPE* dataResult = roiResult.ptr<PIXTYPE>(r);
		for (c = 0; c < cols; ++c){
			dataResult[c] = (1 - rat)*data1[c] + rat*data2[c];
		}
	}
}
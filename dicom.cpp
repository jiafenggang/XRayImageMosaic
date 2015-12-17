#include"dicom.h"

bool Dcm2Mat(const std::string& fileName, cv::Mat& mat, int* windowCenter, int* windowWidth){
	OFCondition condition;
	DcmFileFormat fileFormat;
	condition =fileFormat.loadFile(fileName.c_str());
	if (condition.bad()){
		std::cout << "请确认-" << fileName << "是否存在,打开失败" << std::endl;
		return false;
	}
	DcmDataset* dataset = fileFormat.getDataset();
	Uint16 rows = 0, cols = 0;
	dataset->findAndGetUint16(DCM_Rows, rows);
	dataset->findAndGetUint16(DCM_Columns, cols);
	Uint16 center = 0, width = 0;
	if (windowCenter != NULL){
		const char* strCenter;
		condition = dataset->findAndGetString(DCM_WindowCenter, strCenter);
		sscanf(strCenter, "%d\\", windowCenter);
	}
	if (windowWidth != NULL){
		const char* strWidth;
		condition = dataset->findAndGetString(DCM_WindowWidth, strWidth);
		sscanf(strWidth, "%d\\", windowWidth);
	}
	
	
	DcmElement* element1;
	dataset->findAndGetElement(DCM_PixelData, element1);
	Uint16* data1;
	condition = element1->getUint16Array(data1);
	if (condition.bad()){
		std::cout << "读取数据失败，文件名"<<fileName << std::endl;
		return false;
	} 
	
	//这里一定要重新分配内存
	const UINT32 memsize = rows*cols*sizeof(Uint16);
	Uint16* data = new Uint16[memsize];
	memcpy(data, data1, memsize);
	cv::Mat raw(rows, cols, CV_16UC1, data);
	std::cout << raw.size() << "-" << raw.type() << std::endl;
	mat = raw;
	return true;
}


//主要涉及调窗
cv::Mat Mat16ToMat8(const cv::Mat& mat16, const int windowCenter, const int windowWidth){
	Uint16* data16 = (Uint16*)mat16.data;
	const int rows = mat16.rows;
	const int cols = mat16.cols;
	//必须cv::Scalar(0)负责没有分配内存
	cv::Mat mat8(rows, cols, CV_8UC1,cv::Scalar(0));
	int r = 0, c = 0;
	Uint16 temp16 = 0, temp8 = 0;
	const int lowerEdge = windowCenter - windowWidth / 2;
	const int uperEdge = windowCenter + windowWidth / 2;
	double rangeRat = 255 / (double)windowWidth;
	for (r = 0; r < rows; ++r){
		const Uint16* mat16RowPtr = mat16.ptr<Uint16>(r);
		uchar* mat8RowPtr = mat8.ptr<uchar>(r);
		for (c = 0; c < cols; ++c){
			temp16 = mat16RowPtr[c];
			temp8 = mat8RowPtr[c];
			if (temp16 <= lowerEdge){
				mat8RowPtr[c] = 0;
			}
			else if (temp16 >= uperEdge){
				mat8RowPtr[c] = 255;
			}
			else{
				mat8RowPtr[c] = static_cast<uchar>(rangeRat*(mat16RowPtr[c]-windowCenter+windowWidth/2));
			}
		}
	}
	return mat8;
}


bool SaveMat2Dcm(const cv::Mat& mat, const std::string& fileIn, const std::string& fileOut){
	OFCondition condition;
	DcmFileFormat fileFormat;
	condition=fileFormat.loadFile(fileIn.c_str());
	DcmFileFormat* fileFormatOut =(DcmFileFormat*) fileFormat.clone();


	DcmDataset* dataset = fileFormatOut->getDataset();
	//删除原来的像素数据
	//dataset->findAndDeleteElement(DCM_PixelData);
	const int rows = mat.rows;
	const int cols = mat.cols;
	Uint16* matData = new Uint16[rows*cols];
	memcpy(matData, mat.data, rows*cols*sizeof(Uint16));
	//默认就替换了
	dataset->putAndInsertUint16Array(DCM_PixelData, matData,rows*cols);
	dataset->putAndInsertUint16(DCM_Rows, rows);
	dataset->putAndInsertUint16(DCM_Columns, cols);
	condition=fileFormatOut->saveFile(fileOut.c_str(), EXS_LittleEndianExplicit);
	//DcmDataset* dataset = fileFormat.getDataset();
	//DcmDataset* datasetOut=(DcmDataset*) dataset->clone();
	return true;
}
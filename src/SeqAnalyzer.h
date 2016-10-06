#include <opencv2/opencv.hpp>
#include "Predefined.h"
using namespace std;

class SeqAnalyzer{
public:
	string UID;
	int mode;

	SeqAnalyzer(int givenMode, std::string givenUID);
	int init();
	void update(cv::Mat& emgData, cv::Mat& oriData, cv::Mat& accData);
};


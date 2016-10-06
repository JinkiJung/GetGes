#include "SeqAnalyzer.h"
using namespace cv;

SeqAnalyzer::SeqAnalyzer(int givenMode, string givenUID){
	UID = givenUID;
	mode = givenMode;
}

int SeqAnalyzer::init(){
	return 1;
}

void SeqAnalyzer::update(cv::Mat& emgData, cv::Mat& oriData, cv::Mat& accData){

}

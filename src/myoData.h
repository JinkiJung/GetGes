#include <cmath>
#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <myo/myo.hpp>
#include <opencv2/opencv.hpp>
#include "Predefined.h"

#define VISUALIZING_ELEMENT			13
#define VISUALIZING_BAR_WIDTH		20
#define VISUALIZING_INTERVAL		40

class MyoData{
public:
	bool onArm;
	myo::Arm whichArm;
	int ID;
	std::string UID;

	// This is set by onUnlocked() and onLocked() above.
	bool isUnlocked;
	bool calibrated;
	int calibEmgOffset;
	std::vector<float> calibDegOffset;
	std::array<int8_t, 8> emgSamples;

	// These values are set by onOrientationData() and onPose() above.
	float roll_w, pitch_w, yaw_w;
    cv::Mat accelMat;
	cv::Mat quatMat;
	cv::Mat emgMat;
	myo::Pose currentPose;

	// for visualization
	cv::Mat visual;
	std::vector<cv::Scalar> colorPot;

	inline MyoData(std::string& givenUID) : emgSamples(), onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose() {
		visual = cv::Mat::zeros(300, 80 * VISUALIZING_ELEMENT, CV_8UC3);
		UID = givenUID;
		calibEmgOffset = 0;
		for (int i = 0; i<3; i++)
			calibDegOffset.push_back(0.0f);
		calibrated = false;
		cv::RNG rng(0xFFFFFFFF);
		for (int i = 0; i< VISUALIZING_ELEMENT; i++){
			int icolor = (unsigned)rng;
			colorPot.push_back(cv::Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255));
		}
	}
};

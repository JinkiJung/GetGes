#pragma once

#define _USE_MATH_DEFINES
#include "myoData.h"

#if defined(__APPLE__) && defined(__MACH__)
class MyoCollector : public myo::DeviceListener {
#else

#ifdef GETGES_EXPORTS
#define GETGES_API __declspec(dllexport)
#else
#define GETGES_API __declspec(dllimport)
#endif

class GETGES_API MyoCollector : public myo::DeviceListener {
#endif

public:
	MyoCollector();
	int init(myo::Hub*& hub);
	int getMyoIdx(myo::Myo* myo);
	void collect(myo::Hub*& hub);

	void onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion);
	void onConnect(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion);
	void onDisconnect(myo::Myo* myo, uint64_t timestamp);
	void onUnpair(myo::Myo* myo, uint64_t timestamp);
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat);
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose);
	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,	myo::WarmupState warmupState);
	void onArmUnsync(myo::Myo* myo, uint64_t timestamp);
	void onUnlock(myo::Myo* myo, uint64_t timestamp);
	void onLock(myo::Myo* myo, uint64_t timestamp);
	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg);
	void onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3<float> &accel);
	void visualizeRawData(bool showRawData = false);
	
	std::vector<myo::Myo*> knownMyos;
	std::vector<MyoData> myoData;
	int myoIndex = 0;

	inline int getNumOfMyo() { return (int)knownMyos.size(); }

};


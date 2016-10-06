#include "MyoCollector.h"

#define EMG_ELEMENT_NUM             8
#define NOT_CALIBRATED              -1

using namespace cv;
using namespace std;

MyoCollector::MyoCollector()
{
	
}

void MyoCollector::collect(myo::Hub*& hub){
	// set sensor frequency for collecting data
	hub->run(MYO_FREQUENCY);
	//hub->run(50);
}

int MyoCollector::init(myo::Hub*& hub){
	// We catch any exceptions that might occur below -- see the catch statement for more details.
	try {

		// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
		// publishing your application. The Hub provides access to one or more Myos.
		hub = new myo::Hub("dev.jinki.getges");
		std::cout << "Attempting to find a Myo..." << std::endl;

		myo::Myo* myo = hub->waitForMyo(50);

		// If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}

		// We've found a Myo.
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

		// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.

		// Hub::addListener() takes the address of saveTrainDataany object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		hub->addListener(this);
		
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return -2;
	}

	return 1;
}

void MyoCollector::onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel){
    int curMyoIdx = getMyoIdx(myo);
    myoData[curMyoIdx].accelMat = (Mat_<float>(1,3) << accel[0], accel[1], accel[2] );
}

// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
void MyoCollector::onUnpair(myo::Myo* myo, uint64_t timestamp)
{
	// We've lost a Myo.
	// Let's clean up some leftover state.
	int curMyoIdx = getMyoIdx(myo);
	myoData[curMyoIdx].emgSamples.fill(0);
	myoData[curMyoIdx].roll_w = 0;
	myoData[curMyoIdx].pitch_w = 0;
	myoData[curMyoIdx].yaw_w = 0;
	myoData[curMyoIdx].onArm = false;
	myoData[curMyoIdx].isUnlocked = false;
}

// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
// as a unit quaternion.
void MyoCollector::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
{
	int curMyoIdx = getMyoIdx(myo);

	using std::atan2;
	using std::asin;
	using std::sqrt;
	using std::max;
	using std::min;

	// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
	float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
		1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
	float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
	float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
		1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
	myoData[curMyoIdx].quatMat = (Mat_<float>(1, 4) << quat.x(), quat.y(), quat.z(), quat.w());

	// Convert the floating point angles in radians to a scale from 0 to 180.
	myoData[curMyoIdx].roll_w = (float)((roll + (float)M_PI) / (M_PI * 2.0f) * 180) - myoData[curMyoIdx].calibDegOffset[0];
	myoData[curMyoIdx].pitch_w = (float)((pitch + (float)M_PI / 2.0f) / M_PI * 180) - myoData[curMyoIdx].calibDegOffset[1];
	myoData[curMyoIdx].yaw_w = (float)((yaw + (float)M_PI) / (M_PI * 2.0f) * 180) - myoData[curMyoIdx].calibDegOffset[2];
}

void MyoCollector::onConnect(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)    {
	std::cout << "Myo " << myo << " has connected." << std::endl; 
	knownMyos.push_back(myo);
    
	myo->setStreamEmg(myo::Myo::streamEmgEnabled);
    std::ostringstream s;
    s << myo;
    string myoAdd = s.str();
	MyoData newMyoData1(myoAdd);
	myoData.push_back(newMyoData1);
}

void MyoCollector::onDisconnect(myo::Myo* myo, uint64_t timestamp)    { 
	std::cout << "Myo " << myo << " has disconnected." << std::endl; 

	// erase element from myo data array
	// to notify one of myo has disconnected
	for (int i = 0; i < myoData.size(); i++){
        std::ostringstream s;
        s << myo;
        string myoAdd = s.str();
        if (myoData[i].UID == myoAdd){//dynamic_cast<std::ostringstream&>(std::ostringstream() << myo).str()){
			myoData.erase(myoData.begin() + i);
			break;
		}
	}
}

// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
// making a fist, or not making a fist anymore.
void MyoCollector::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
{
	myoData[getMyoIdx(myo)].currentPose = pose;

	if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
		// Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
		// Myo becoming locked.
		myo->unlock(myo::Myo::unlockHold);

		// Notify the Myo that the pose has resulted in an action, in this case changing
		// the text on the screen. The Myo will vibrate.
		myo->notifyUserAction();
	}
	else {
		// Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
		// are being performed, but lock after inactivity.
		myo->unlock(myo::Myo::unlockTimed);
	}
}


// onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
// arm. This lets Myo know which arm it's on and which way it's facing.
void MyoCollector::onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
	myo::WarmupState warmupState)
{
	//myoData[getMyoIdx(myo)].onArm = true;
	//myoData[getMyoIdx(myo)].whichArm = arm;
}

// onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
// when Myo is moved around on the arm.
void MyoCollector::onArmUnsync(myo::Myo* myo, uint64_t timestamp)
{
	//myoData[getMyoIdx(myo)].onArm = false;
}


void MyoCollector::onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion){
	// Print out the MAC address of the armband we paired with.        
	// The pointer address we get for a Myo is unique - in other words, it's safe to compare two Myo pointers to        
	// see if they're referring to the same Myo.        
	// Add the Myo pointer to our list of known Myo devices. This list is used to implement getMyoIdx() below so        
	// that we can give each Myo a nice short identifier.        
	//knownMyos.push_back(myo);        // Now that we've added it to our list, get our short ID for it and print it out.        

    std::ostringstream s;
    s << myo;
    string myoAdd = s.str();
	std::cout << "Paired with " << myoAdd << "." << std::endl;
	
	//knownMyos.push_back()
}

int MyoCollector::getMyoIdx(myo::Myo* myo) {
	// Walk through the list of Myo devices that we've seen pairing events for.        
	for (size_t i = 0; i < knownMyos.size(); ++i) {
		// If two Myo pointers compare equal, they refer to the same Myo device.            
		if (knownMyos[i] == myo) {
			return i ;
		}
	}        return 0;
}

// onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
void MyoCollector::onUnlock(myo::Myo* myo, uint64_t timestamp)
{
	myoData[getMyoIdx(myo)].isUnlocked = true;
}

// onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
void MyoCollector::onLock(myo::Myo* myo, uint64_t timestamp)
{
	myoData[getMyoIdx(myo)].isUnlocked = false;
}

// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
void MyoCollector::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
	int curMyoIdx = getMyoIdx(myo);
	std::vector<int> values;
	for (int i = 0; i < EMG_ELEMENT_NUM; i++) {
		int idx = i - myoData[curMyoIdx].calibEmgOffset;
        if( idx < 0 )
            idx += EMG_ELEMENT_NUM;
		myoData[curMyoIdx].emgSamples[idx] = emg[i];
	}
	for (int i = 0; i<myoData[curMyoIdx].emgSamples.size(); i++)
		values.push_back(myoData[curMyoIdx].emgSamples[i]);
    
	Mat tempValues(values);
	myoData[curMyoIdx].emgMat = tempValues.clone();
}

// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
// For this example, the functions overridden above are sufficient.

// We define this function to print the current values that were updated by the on...() functions above.
void MyoCollector::visualizeRawData(bool showRawData)
{
	// Clear the current line
	//std::cout << '\r';

	for (int i = 0; i < myoData.size(); i++){
		int curMyoIdx = i;
		int baseline4Rows = myoData[curMyoIdx].visual.rows - 50;
		//if ( !myoData[i].onArm )
		//	continue;

		float avgMag = 0;
		Mat values;
		// Print out the EMG data.
		for (size_t i = 0; i < myoData[curMyoIdx].emgSamples.size(); i++) {
			std::ostringstream oss;
			oss << static_cast<int>(myoData[curMyoIdx].emgSamples[i]);
			std::string emgString = oss.str();

			int value = (atoi(emgString.c_str()));
			int width = (int)i * VISUALIZING_INTERVAL + 20;
			rectangle(myoData[curMyoIdx].visual, Point(width, baseline4Rows / 2), Point(width + VISUALIZING_BAR_WIDTH, baseline4Rows / 2 + value), myoData[curMyoIdx].colorPot[i], -1);

			avgMag += value*value;
			values.push_back(value);
			if (showRawData)
				std::cout << '[' << emgString << std::string(4 - emgString.size(), ' ') << ']';
			string valueTitle = to_string(i);
			putText(myoData[curMyoIdx].visual, valueTitle, Point(width, myoData[curMyoIdx].visual.rows - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
		}
		int index = 8;
		int width = 8 * VISUALIZING_INTERVAL + 20;
		Scalar mean, sdv;
		values = abs(values);
		meanStdDev(values, mean, sdv);
		rectangle(myoData[curMyoIdx].visual, Point(width, baseline4Rows), Point(width + VISUALIZING_BAR_WIDTH, baseline4Rows - mean(0)), myoData[curMyoIdx].colorPot[index], -1);
		putText(myoData[curMyoIdx].visual, "mean", Point(width, myoData[curMyoIdx].visual.rows - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
		index++;
		width = 9 * VISUALIZING_INTERVAL + 20;
		rectangle(myoData[curMyoIdx].visual, Point(width, baseline4Rows), Point(width + VISUALIZING_BAR_WIDTH, baseline4Rows - sdv(0)), myoData[curMyoIdx].colorPot[index], -1);
		putText(myoData[curMyoIdx].visual, "sdv", Point(width, myoData[curMyoIdx].visual.rows - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
		index++;
		//std::cout << "\t" << mean(0) << "\t" << sdv(0) << std::endl;

		Mat ori_mat = (Mat_<float>(1, 3) << myoData[curMyoIdx].roll_w, myoData[curMyoIdx].pitch_w, myoData[curMyoIdx].yaw_w);

		//*

		width = 10 * VISUALIZING_INTERVAL + 20;
		rectangle(myoData[curMyoIdx].visual, Point(width, 0), Point(width + VISUALIZING_BAR_WIDTH, ori_mat.at<float>(0, 0)), myoData[curMyoIdx].colorPot[index++], -1);
		putText(myoData[curMyoIdx].visual, "roll", Point(width, myoData[curMyoIdx].visual.rows - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
		width = 11 * VISUALIZING_INTERVAL + 20;
		rectangle(myoData[curMyoIdx].visual, Point(width, 0), Point(width + VISUALIZING_BAR_WIDTH, ori_mat.at<float>(0, 1)), myoData[curMyoIdx].colorPot[index++], -1);
		putText(myoData[curMyoIdx].visual, "pitch", Point(width, myoData[curMyoIdx].visual.rows - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
		width = 12 * VISUALIZING_INTERVAL + 20;
		rectangle(myoData[curMyoIdx].visual, Point(width, 0), Point(width + VISUALIZING_BAR_WIDTH, ori_mat.at<float>(0, 2)), myoData[curMyoIdx].colorPot[index++], -1);
		putText(myoData[curMyoIdx].visual, "yaw", Point(width, myoData[curMyoIdx].visual.rows - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
		//*/

		// Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
		//std::cout << '[' << std::string(roll_w, '*') << std::string(180 - roll_w, ' ') << ']' << '[' << std::string(pitch_w, '*') << std::string(180 - pitch_w, ' ') << ']'	<< '[' << std::string(yaw_w, '*') << std::string(180 - yaw_w, ' ') << ']';

		if (myoData[curMyoIdx].onArm) {
			// Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.

			// Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
			// output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
			// that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
			std::string poseString = myoData[curMyoIdx].currentPose.toString();

			//std::cout << '[' << (isUnlocked ? "unlocked" : "locked  ") << ']' << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']' << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
		}
		else {
			// Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
			//std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
		}

		if (showRawData)
			std::cout << std::endl;
		std::string name = "graph - " + myoData[curMyoIdx].UID;
		imshow(name.c_str(), myoData[curMyoIdx].visual);
		myoData[curMyoIdx].visual = myoData[curMyoIdx].visual* 0.6f;
		//waitKey(1);

	}
	
}


#include "GestureGetter.h"

using namespace cv;
using namespace GetGes;

GestureGetter::GestureGetter(){
    mode = TRAIN_MODE;
}

GestureGetter::~GestureGetter(){
	delete hub;

}

int GestureGetter::test(){
	return 1;
}

int GestureGetter::init(){
	return collector.init(hub);
}

void GestureGetter::sync(){
	// we create seqAnalyzer matching with the number of recognized myo
	while (collector.myoData.size() > analyzers.size()){
		
		for (int i = 0; i < collector.myoData.size(); i++){
			bool alreadyHave = false;
			for (int t = 0; t < analyzers.size(); t++){

				if (collector.myoData[i].UID == analyzers[t].UID){
					alreadyHave = true;
					break;
				}
			}
			// here we assume that the new myo data should be placed backside: a queue
			if (!alreadyHave){
				analyzers.push_back(SeqAnalyzer(mode, collector.myoData[i].UID));
				analyzers[analyzers.size() - 1].init();
			}
		}
		
	}

	// for analyzer removal when a myo is disconnected
	while (collector.getNumOfMyo() < analyzers.size()){
		for (int i = 0; i < analyzers.size(); i++){
			bool alreadyHave = false;
			for (int t = 0; t < collector.myoData.size(); t++){

				if (collector.myoData[t].UID == analyzers[i].UID){
					alreadyHave = true;
					break;
				}

			}
			if (!alreadyHave)
				analyzers.erase(analyzers.begin() + i);
		}
	}
}

int GestureGetter::update(){
	// collect sensor data
	collector.collect(hub);
		
	// sync the number of known myos with the number of analyzers
	sync();

	collector.visualizeRawData();
	
	// get input from the user
	char inputKey = waitKey(1);

	if (inputKey == 'q' || inputKey == 'Q'){
		return -1;
	}

	return 1;
}


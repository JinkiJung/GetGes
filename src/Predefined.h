#define NONE_MODE						0
#define TRAIN_MODE						1
#define TEST_MODE						2
#define CALIBRATION_MODE                3

#define TARGET_NONE_GESTURE             0
#define TARGET_PAPER_GESTURE            1
#define TARGET_ROCK_GESTURE             2
#define TARGET_THUMB_CLICK	            3
#define TARGET_INDEX_CLICK				4
#define TARGET_MIDDLE_CLICK             5
#define TARGET_RING_CLICK				6
#define TARGET_PINKY_CLICK				7
#define TARGET_FINGER_STATE				8

#define SEQ_SHORT_TERM					6
#define SEQ_MID_TERM					8
#define SEQ_LONG_TERM					12

#define MYO_FREQUENCY                   1

#define TRAIN_SEQ_LENGTH				100

#define NUM_OF_DIMENSION				8

#define TRAIN_DATA_TAG_ROCK             "../database/data_rock_"
#define TRAIN_DATA_TAG_PAPER            "../database/data_paper_"
#define TRAIN_DATA_TAG_NONE             "../database/data_none_"

#define TRAIN_DATA_TAG_THUMB            "../database/data_thumb_"
#define TRAIN_DATA_TAG_INDEX            "../database/data_index_"
#define TRAIN_DATA_TAG_MIDDLE            "../database/data_middle_"
#define TRAIN_DATA_TAG_RING            "../database/data_ring_"
#define TRAIN_DATA_TAG_PINKY            "../database/data_pinky_"
#define TRAIN_DATA_TAG_FINGER             "../database/data_finger_"

#if defined(__APPLE__) && defined(__MACH__)
#define XML_DATABASE_PATH				"/Users/Getko/Develop/GitHub/GetGes/database/"
#else
#define XML_DATABASE_PATH				"../database/*.xml"
#endif
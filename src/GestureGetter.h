#include "SeqAnalyzer.h"
#include "myoCollector.h"
namespace GetGes{

#if defined(__APPLE__) && defined(__MACH__)
class GestureGetter{
#else

#ifdef GETGES_EXPORTS
#define GETGES_API __declspec(dllexport)
#else
#define GETGES_API __declspec(dllimport)
#endif

class GETGES_API GestureGetter{
#endif
public:
    int mode;
    
	GestureGetter();
	~GestureGetter();
	MyoCollector collector;
	std::vector<SeqAnalyzer> analyzers;
	myo::Hub* hub;

	int init();
	int update();
	int test();
	void sync();
};

}
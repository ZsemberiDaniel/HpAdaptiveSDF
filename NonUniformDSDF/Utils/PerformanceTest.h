#ifndef PERFORMANCETEST_H
#define PERFORMANCETEST_H

#include <vector>

class PerformanceTest
{
	bool testIsRunning = false;
	int maxFrame = 50;
	int actualFrame = 0;

	std::vector<double> times;
public:
	struct Results {
		int frames; // number of frames
		double avg; // avreage time
		double dev; // sample standard deviation
		double min;
		double max;
	};

	// returns if stating the test was successful
	inline bool startTest(int frames);
	inline void stopTest();
	// returns if the test is still running or not
	inline bool testInProgress() const;
	// returns if the test is still running or not
	inline operator bool() const { return testInProgress(); }
	// returns if the test is still running or not
	inline bool addFrameTime(double frameTime);
	// retruns the results
	inline Results getResults();
};

bool PerformanceTest::startTest(int frames)
{
	if (!testIsRunning) {
		maxFrame = frames > 0 ? frames : 0;
		actualFrame = 0;
		times.clear();
		times.reserve(maxFrame);
		return testIsRunning = maxFrame > 0;
	}
	else return false;
}
void PerformanceTest::stopTest()
{
	testIsRunning = false;
}
bool PerformanceTest::testInProgress() const
{
	return testIsRunning;
}
bool PerformanceTest::addFrameTime(double frameTime)
{
	if (!testIsRunning) return false;
	times.push_back(frameTime);
	actualFrame++;
	if (actualFrame >= maxFrame)
		testIsRunning = false;
	return testIsRunning;
}
PerformanceTest::Results PerformanceTest::getResults()
{
	int frames = int(times.size());
	if (testIsRunning || frames == 0) return {};

	double sum = 0.0;
	double min = times[0], max = times[0];
	for (double t : times) {
		sum += t;
		min = t < min ? t : min;
		max = t > max ? t : max;
	}
	double avg = sum / (double)frames; // avreage time
	sum = 0.0;
	for (double t : times) {
		sum += (avg - t)*(avg - t);
	}
	double dev = sqrt(sum / ((double)frames - 1)); // sample standard deviation

	return PerformanceTest::Results {
		frames, avg, dev, min, max
	};
}
#endif
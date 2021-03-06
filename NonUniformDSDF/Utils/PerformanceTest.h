#ifndef PERFORMANCETEST_H
#define PERFORMANCETEST_H

#include <deque>

class PerformanceTest
{
	int maxFrame;
	int storedFrameCount = 0;

	std::deque<double> times;
public:
	PerformanceTest(int maxFrame_ = 50) : maxFrame(maxFrame_) {}

	struct Results {
		size_t frames; // number of frames
		double avg; // avreage time
		double dev; // sample standard deviation
		double min;
		double max;
	};

	// returns if the test is still running or not
	inline void addFrameTime(double frameTime);
	// retruns the results
	inline Results getResults();
};

void PerformanceTest::addFrameTime(double frameTime)
{
	if (storedFrameCount == maxFrame)
	{
		times.pop_front();
		times.push_back(frameTime);
	}
	else
	{
		times.push_back(frameTime);
		storedFrameCount++;
	}
}
PerformanceTest::Results PerformanceTest::getResults()
{
	size_t frames = times.size();
	if (frames == 0) return {};

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

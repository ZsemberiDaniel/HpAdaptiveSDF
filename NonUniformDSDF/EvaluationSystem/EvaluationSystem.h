#ifndef EVAL_SYSTEM_H
#define EVAL_SYSTEM_H

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>

#include "../Utils/TablePrinter.h"
#include "../Utils/PerformanceTest.h"
#include "../Utils/ErrorStatistics.h"
#include "../Octree/SaveableOctree.h"
#include "../App.h"

namespace Evaluation 
{
	struct TestCase
	{
		std::string pathToSaveFile;
		glm::vec3 camPos;
		// 0 - default, 1 - relaxed, 2 - enhanced
		int sphereTraceType;
		bool useLookupTable;

		friend std::istream& operator>>(std::istream& stream, TestCase& testCase)
		{
			stream >> testCase.pathToSaveFile
				>> testCase.camPos.x 
				>> testCase.camPos.y 
				>> testCase.camPos.z 
				>> testCase.sphereTraceType 
				>> testCase.useLookupTable;

			return stream;
		}

		std::string name()
		{
			std::string path = std::string(pathToSaveFile);
			// Remove directory if present.
			const size_t last_slash_idx = path.find_last_of("\\/");
			if (std::string::npos != last_slash_idx)
			{
				path.erase(0, last_slash_idx + 1);
			}

			// Remove extension if present.
			const size_t period_idx = path.rfind('.');
			if (std::string::npos != period_idx)
			{
				path.erase(period_idx);
			}

			std::string traceType;
			switch (sphereTraceType)
			{
			case 0: traceType = "DEF"; break;
			case 1: traceType = "REL"; break;
			case 2: traceType = "ENH"; break;
			}

			std::string lookupTable = useLookupTable ? "LT" : "";

			std::stringstream ss;
			ss << path << "_" << traceType << "_" << lookupTable;
			return ss.str();
		}
	};

	class TestCaseResult
	{
	public:
		TestCase testCase;

		float minRenderTime;
		float maxRenderTime;
		float avgRenderTime;

		float minError;
		float maxError;
		float avgError;
		float medianError;
		float l2Error;

		TestCaseResult(TestCase testCase_, PerformanceTest::Results performance, ErrorStatistics::ErrorStatResult error) :
			testCase(testCase_)
		{
			minRenderTime = performance.min;
			maxRenderTime = performance.max;
			avgRenderTime = performance.avg;

			minError = error.min;
			maxError = error.max;
			avgError = error.mean;
			medianError = error.median;
			l2Error = error.l2;
		}

		TestCaseResult(TestCase testCase_, PerformanceTest::Results performance) : TestCaseResult(testCase_, performance, {}) { }
		TestCaseResult(TestCase testCase_, ErrorStatistics::ErrorStatResult error) : TestCaseResult(testCase_, {}, error) { }
	};

	class EvaluationResults
	{
	private:
		std::vector<TestCaseResult> results;

	public:
		EvaluationResults(std::vector<TestCaseResult> results_) : results(results_) {}

		/* Helper class to print data from TestCaseResult class. See more in PrintResult static class.
		*/
		class ResultDataPrinter
		{
		private:
			const std::string dataName;
			std::function<std::string(TestCaseResult& testCaseResult)> applyFunc;

		public:
			ResultDataPrinter(const std::string name, std::function<std::string(TestCaseResult& testCaseResult)> applyFunc_) :
				applyFunc(applyFunc_), dataName(name) {}

			std::string operator()(TestCaseResult& testCaseResult)
			{
				return applyFunc(testCaseResult);
			}

			const std::string name() const { return dataName; }

			/*ResultDataPrinter operator|(const ResultDataPrinter& printer) const
			{
				return ResultDataPrinter(dataName + printer.dataName, [&](std::ostream& stream, TestCaseResult& testCaseResult) {
					this->applyFunc(stream, testCaseResult);
					stream << ", ";
					printer.applyFunc(stream, testCaseResult);
				});
			}*/
		};

		/* Its' static fields define how test results can be printed.
		*/
		static class PrintResult
		{
		public:
			inline static const ResultDataPrinter TIME_MIN = ResultDataPrinter("TMin", [](TestCaseResult& testCaseResult) -> std::string {
				return std::to_string(testCaseResult.minRenderTime);
			});
			inline static const ResultDataPrinter TIME_MAX = ResultDataPrinter("TMax", [](TestCaseResult& testCaseResult) -> std::string {
				return std::to_string(testCaseResult.maxRenderTime);
			});
			inline static const ResultDataPrinter TIME_AVG = ResultDataPrinter("TAvg", [](TestCaseResult& testCaseResult) -> std::string {
				return std::to_string(testCaseResult.avgRenderTime);
			});
			inline static const ResultDataPrinter ERROR_MIN = ResultDataPrinter("EMin", [](TestCaseResult& testCaseResult) -> std::string {
				return std::to_string(testCaseResult.minError);
			});
			inline static const ResultDataPrinter ERROR_MAX = ResultDataPrinter("EMax", [](TestCaseResult& testCaseResult) -> std::string {
				return std::to_string(testCaseResult.maxError);
			});
			inline static const ResultDataPrinter ERROR_AVG = ResultDataPrinter("EAvg", [](TestCaseResult& testCaseResult) -> std::string {
				return std::to_string(testCaseResult.avgError);
			});
			inline static const ResultDataPrinter ERROR_MEDIAN = ResultDataPrinter("EMed", [](TestCaseResult& testCaseResult) -> std::string {
				return std::to_string(testCaseResult.medianError);
				});
			inline static const ResultDataPrinter ERROR_L2 = ResultDataPrinter("EL2", [](TestCaseResult& testCaseResult) -> std::string {
				return std::to_string(testCaseResult.l2Error);
				});
			// inline static const ResultDataPrinter ALL = TIME_MIN | TIME_MAX | TIME_AVG;
		};

		/* A Sorter class that provides comparators for the test cases in the results.
		*/
		class ResultSorter
		{
		private:
			// > 0 if 1st parameter is higher
			// = 0 if equal
			// < 0 if second parameter is lower
			std::function<int(const TestCaseResult&, const TestCaseResult&)> comparator;

		public:
			ResultSorter(std::function<bool(const TestCaseResult&, const TestCaseResult&)> comparator_) :
				comparator(comparator_) {}

			/* 'After' operator. a >> b means first sort by a, then sort by b.
			*/
			ResultSorter operator>>(const ResultSorter& sorter) const
			{
				return ResultSorter([&](const TestCaseResult& tr1, const TestCaseResult& tr2) -> int {
					int result = this->comparator(tr1, tr2);
					if (result == 0) 
					{
						return sorter.comparator(tr1, tr2);
					}

					return result;
				});
			}

			void sortResults(std::vector<TestCaseResult>& results)
			{
				std::sort(results.begin(), results.end(), [&](const TestCaseResult& tr1, const TestCaseResult& tr2) -> bool
				{
					return this->comparator(tr1, tr2) > 0;
				});
			}
		};

		/* Contains comparators for test results to sort them. They can be chained via operator >>. 
		 * a >> b means first sort by a, then sort by b.
		*/
		static class ResultSorters
		{
		public:
			inline static const ResultSorter TRACE_TYPE = ResultSorter([](const TestCaseResult& tr1, const TestCaseResult& tr2) {
				return tr1.testCase.sphereTraceType - tr2.testCase.sphereTraceType;
			});
			inline static const ResultSorter USE_LOOKUP_TABLE = ResultSorter([](const TestCaseResult& tr1, const TestCaseResult& tr2) {
				return tr1.testCase.useLookupTable - tr2.testCase.useLookupTable;
			});
			inline static const ResultSorter DEFAULT = TRACE_TYPE >> USE_LOOKUP_TABLE;
		};

		/* Prints the results in a table:
		*	             || result1 | result2 | ... | resultLast
		*	|| testCase1 || 0.23    | 0.45    | ... | 0.456
		*   || testCase2 || 0.354   | 0.56    | ... | 0.123
		*   ...
		*/
		void printResultsTo(
			std::ostream& stream, 
			std::initializer_list<ResultDataPrinter> printResult = { PrintResult::TIME_MIN, PrintResult::TIME_AVG, PrintResult::TIME_MAX },
			ResultSorter sorter = ResultSorters::DEFAULT)
		{
			trl::TablePrinter tablePrinter(stream);

			// add table columns (the result names)
			tablePrinter.AddColumn("Testcase name", 25);
			for (ResultDataPrinter printer : printResult)
			{
				tablePrinter.AddColumn(printer.name(), 15);
			}
			
			tablePrinter.PrintHeader();
			// add table data

			sorter.sortResults(results);
			for (TestCaseResult result : results)
			{
				tablePrinter << result.testCase.name();

				for (ResultDataPrinter printer : printResult)
				{
					tablePrinter << printer(result);
				}
			}
			tablePrinter.PrintFooter();
		}
	};

	static class EvaluationSystem
	{
	public:
		static void evaluate(const std::string& testFilePath, const std::string& outPath, int frameCount)
		{
			std::ifstream istream(testFilePath);
			if (!istream.is_open())
			{
				std::cerr << "Could not open test file at " << testFilePath << std::endl;
				return;
			}

			std::ofstream ostream(outPath);
			if (!ostream.is_open())
			{
				std::cerr << "Could not open output test file at " << outPath << std::endl;
				return;
			}

			std::vector<TestCaseResult> results;
			TestCase tc;
			while (istream >> tc)
			{
				auto param = df::Sample::FLAGS::WINDOW_BORDERLESS;
				df::Sample sam("Non-uniform DSDF TESTS", 1920, 1080, param);

				// sam.AddHandlerClass<df::ImGuiHandler>(10);
				App app(sam, frameCount,
					SaveableOctree::loadFrom(std::wstring(tc.pathToSaveFile.begin(), tc.pathToSaveFile.end())));
				sam.AddHandlerClass(app, 5);

				app.EnterTestMode(tc.camPos, tc.sphereTraceType, tc.useLookupTable);

				int count = 0;
				sam.Run([&](float deltaTime) //delta time in ms
					{
						app.Update();
						app.Render();
						app.RenderGUI();

						if (++count >= frameCount + 10)
						{
							sam.Quit();
						}
					}
				);

				results.push_back(TestCaseResult(tc, app.getPerfTestResults(), app.getErrorResults()));
			}

			EvaluationResults evalResults(results);
			evalResults.printResultsTo(ostream);

			istream.close();
			ostream.close();
		}
	};
};

#endif
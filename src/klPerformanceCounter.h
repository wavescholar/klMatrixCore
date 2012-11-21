
#pragma once
#using <System.dll>
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace System::Diagnostics;

namespace klCounters {

	public ref class  klPerformanceCounter
	{
	public:
		klPerformanceCounter()
		{

		}
		klPerformanceCounter(klPerformanceCounter^ p)
		{

		}

		void OutputSample( CounterSample s );

		float MyComputeCounterValue( CounterSample s0, CounterSample s1 );

		bool SetupCategory();

		void RemoveCategory(String^ Category );

		//This counter set is for average time, TicsPerSec for the high frequency counter, and units completed.
		//void CreateCounters( PerformanceCounter^% AIT, PerformanceCounter^% AITB, PerformanceCounter^% II );

		void CreateCounters( PerformanceCounter^% PC, PerformanceCounter^% BPC );
		
		void CollectSamples( ArrayList^ samplesList, PerformanceCounter^ PC, PerformanceCounter^ BPC );

		void CalculateResults( ArrayList^ samplesList );

		//Unit Test
		int TestPerformanceCounter();
	};
}

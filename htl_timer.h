#include<ctime>
#include<iostream>
namespace htl {
	class Timer
	{
	private:
		clock_t start, end;

	public:
		//start colok mark
		inline void Start() { start = clock(); }
		//end colok mark
		void End() { end = clock(); }
		//print duration time
		void Print() const {
			long t = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
			std::cout << (long)t / 1000<<" s "<< (t % 1000) << " ms\n"; 
		}
	};
}
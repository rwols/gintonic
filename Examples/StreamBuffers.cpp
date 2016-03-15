#include "gintonic.hpp"
#include "Foundation/BinaryStreamBuffer.hpp"
#include <fstream>

int main(int argc, const char** argv)
{
	std::cerr << "Testing integer serialization and deserialization.\n";
	for (int i = std::numeric_limits<int>::min(); i < std::numeric_limits<int>::max(); ++i)
	{
		if (i % 1000000 == 0)
		{
			double percent = static_cast<double>(i) - static_cast<double>(std::numeric_limits<int>::min());
			std::cerr << 100.0 * (percent / ((double) std::numeric_limits<int>::max() - (double) std::numeric_limits<int>::min()));
			std::cerr << "\%\n";
		}
		{
			std::ofstream output("testblob", std::ios::binary);
			gintonic::serializeInt(output, i);	
		}
		{
			std::ifstream input("testblob", std::ios::binary);
			int testvalue = gintonic::deserializeInt(input);
			assert(testvalue == i);
		}		
	}
	std::cerr << "Testing float serialization and deserialization.\n";
	for (float f = -1000.0f; f <= 1000.0f; f += 0.001f)
	{
		{
			std::ofstream output("testblob", std::ios::binary);
			gintonic::serializeFloat(output, f);
		}
		{
			std::ifstream input("testblob", std::ios::binary);
			float testvalue = gintonic::deserializeFloat(input);
			assert(testvalue == f);
		}
	}

	return EXIT_SUCCESS;
}
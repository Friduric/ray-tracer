#pragma once

#include <vector>

namespace Utility {
	namespace Math {
		/// <summary>
		/// Returns a vector with indices sorted based on the values in the vector /values/.
		/// </summary>
		std::vector<int> GetSortedIndices(const std::vector<float>& values);
	}
}
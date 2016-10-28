#include "Other.h"

#include <algorithm>
#include <numeric>

std::vector<int> Utility::Math::GetSortedIndices(const std::vector<float>& values) {
	std::vector<int> indices(values.size());
	std::iota(indices.begin(), indices.end(), 0);
	std::sort(indices.begin(), indices.end(), [&values](size_t i1, size_t i2) {return values[i1] < values[i2]; });
	return indices;
}
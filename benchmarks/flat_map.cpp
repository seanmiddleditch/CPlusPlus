#include "benchmark/benchmark.h"

#include <vector>
#include <algorithm>
#include <random>
#include <sstream>
#include <map>

struct key_comparator
{
	template <typename K, typename V>
	bool operator()(std::pair<K, V> const& lhs, K const& rhs) const { return lhs.first < rhs; }
	template <typename K, typename V>
	bool operator()(K const& lhs, std::pair<K, V> const& rhs) const { return lhs < rhs.first; }
};

template <typename K, typename V, typename A = std::allocator<std::pair<K, V>>>
class flat_map
{
	std::vector<std::pair<K, V>, A> _data;

	using vector_type = decltype(_data);

public:
	using iterator = typename vector_type::iterator;
	using const_iterator = typename vector_type::const_iterator;
	using size_type = typename vector_type::size_type;
	using value_type = typename vector_type::value_type;
	using key_type = K;
	using mapped_type = V;

	size_type size() const { return _data.size(); }
	bool empty() const { return _data.empty(); }

	void reserve(size_type capacity) { _data.reserve(capacity); }

	iterator begin() { return _data.begin(); }
	iterator end() { return _data.end(); }

	const_iterator begin() const { return _data.begin(); }
	const_iterator end() const { return _data.end(); }

	void insert(value_type&& value)
	{
		auto it = std::lower_bound(_data.begin(), _data.end(), value.first, key_comparator{});
		if (it == _data.end() || it->first != value.first)
			_data.insert(it, std::move(value));
	}
	
	void insert(value_type const& value)
	{
		auto it = std::lower_bound(_data.begin(), _data.end(), value.first, key_comparator{});
		if (it == _data.end() || it->first != value.first)
			_data.insert(it, std::move(value));
	}

	iterator find(key_type const& key)
	{
		auto it = std::lower_bound(_data.begin(), _data.end(), key, key_comparator{});
		if (it != _data.end() && it->first == key)
			return it;
		else
			return _data.end();
	}
};

using int_flat_map = flat_map<int, int>;
using int_map = std::map<int, int>;

template <typename MapT>
static void insert(benchmark::State& state)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis;

	while (state.KeepRunning())
	{
		MapT map;

		for (int i = 0; i != state.range_x(); ++i)
			map.insert({dis(gen), i});

		benchmark::DoNotOptimize(map);
	}
}
BENCHMARK_TEMPLATE(insert, int_flat_map)->Range(1, 10000);
BENCHMARK_TEMPLATE(insert, int_map)->Range(1, 10000);

template <typename MapT>
static void insert_reserve(benchmark::State& state)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis;

	while (state.KeepRunning())
	{
		MapT map;
		map.reserve(state.range_x());

		for (int i = 0; i != state.range_x(); ++i)
			map.insert({dis(gen), i});

		benchmark::DoNotOptimize(map);
	}
}
BENCHMARK_TEMPLATE(insert_reserve, int_flat_map)->Range(1, 10000);

template <typename MapT>
static void find(benchmark::State& state)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis;

	std::vector<int> numbers;
	numbers.reserve(state.range_x());

	flat_map<int, int> map;

	for (int i = 0; i != state.range_x(); ++i)
	{
		int num = dis(gen);
		numbers.push_back(num);
		map.insert({num, i});
	}
	
	while (state.KeepRunning())
	{
		for (int i = 0; i < (state.range_x() >> 1); ++i)
		{
			auto it = map.find(numbers[i]);
			if (it != map.end())
				benchmark::DoNotOptimize(it);
		}
	}
}
BENCHMARK_TEMPLATE(find, int_flat_map)->Range(1, 10000);
BENCHMARK_TEMPLATE(find, int_map)->Range(1, 10000);
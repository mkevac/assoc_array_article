#include <sparsepp/spp.h>
#include <sparsepp/spp_timer.h>

#include <sparsehash/dense_hash_map>
#include <sparsehash/sparse_hash_map>

#include <cstdio>
#include <utility>
#include <unordered_map>

#include <Judy.h>
#include <xxhash.h>

using std::make_pair;

struct xxhash {
	size_t operator()(uint64_t str) const {
		return XXH64(&str, sizeof(uint64_t), 2001);
	}
};

template <class T>
void test(T &s, int count)
{
	spp::Timer<std::milli> timer;

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.insert({rand(), {}});

	printf("%d random inserts         in %5.2f seconds\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.find(rand());

	printf("%d random finds           in %5.2f seconds\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(1);
	for (int i = 0; i < count; ++i)
		s.find(rand());
	printf("%d random not-finds       in %5.2f seconds\n", count, timer.get_delta() / 1000);

	s.clear();
	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.insert({i, {}});
	printf("%d sequential inserts     in %5.2f seconds\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.find(i);
	printf("%d sequential finds       in %5.2f seconds\n", count, timer.get_delta() / 1000);
	s.clear();

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.insert({-i, {}});

	printf("%d neg sequential inserts in %5.2f seconds\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.find(-i);

	printf("%d neg sequential finds   in %5.2f seconds\n", count, timer.get_delta() / 1000);
	s.clear();
}

void test_judy(int count)
{
	spp::Timer<std::milli> timer;
	Pvoid_t arr = NULL;

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLIns(&arr, rand(), NULL);

	printf("%d random inserts         in %5.2f seconds\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLGet(arr, rand(), NULL);

	printf("%d random finds           in %5.2f seconds\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(1);
	for (int i = 0; i < count; ++i)
		JudyLGet(arr, rand(), NULL);
	printf("%d random not-finds       in %5.2f seconds\n", count, timer.get_delta() / 1000);

	JudyLFreeArray(&arr, NULL);
	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLIns(&arr, i, NULL);
	printf("%d sequential inserts     in %5.2f seconds\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLGet(arr, i, NULL);

	printf("%d sequential finds       in %5.2f seconds\n", count, timer.get_delta() / 1000);
	JudyLFreeArray(&arr, NULL);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLIns(&arr, -i, NULL);

	printf("%d neg sequential inserts in %5.2f seconds\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLGet(arr, -i, NULL);

	printf("%d neg sequential finds   in %5.2f seconds\n", count, timer.get_delta() / 1000);
	JudyLFreeArray(&arr, NULL);
}

int main(int argc, char const *argv[])
{
	if (argc < 3)
	{
		printf("usage: %s <judy|spp|dense|sparse|std> <n_iterations>\n", argv[0]);
		return 1;
	}

	char const *test_type = argv[1];
	int const n_iterations = atoi(argv[2]);

	if (strcmp(test_type, "spp") == 0)
	{
		printf("Testing spp::sparse_hash_map (%d iterations)\n", n_iterations);
		spp::sparse_hash_map<uint32_t, void*, std::hash<uint32_t>> s;
		test(s, n_iterations);
		printf("\n\n");

		return 0;
	}

	if (strcmp(test_type, "sparse") == 0)
	{
		printf ("Testing google::sparse_hash_map (%d iterations)\n", n_iterations);
		google::sparse_hash_map<uint32_t, void*, std::hash<uint32_t>> s;
		// s.set_empty_key(-INT_MAX);
		s.set_deleted_key(-(INT_MAX - 1));
		test(s, n_iterations);
		printf ("\n\n");
		return 0;
	}

	if (strcmp(test_type, "dense") == 0)
	{
		printf ("Testing google::dense_hash_map (%d iterations)\n", n_iterations);
		google::dense_hash_map<uint32_t, void*, std::hash<uint32_t>> s;
		s.set_empty_key(-INT_MAX);
		s.set_deleted_key(-(INT_MAX - 1));
		test(s, n_iterations);
		printf ("\n\n");
		return 0;
	}

	if (strcmp(test_type, "std") == 0)
	{
		printf ("Testing std::unordered_map (%d iterations)\n", n_iterations);
		std::unordered_map<uint32_t, void*, std::hash<uint32_t>> s;
		test(s, n_iterations);
		printf ("\n\n");
		return 0;
	}

	if (strcmp(test_type, "judy") == 0)
	{
		printf ("Testing Judy (%d iterations)\n", n_iterations);
		test_judy(n_iterations);
		printf ("\n\n");
		return 0;
	}

	return 0;
}

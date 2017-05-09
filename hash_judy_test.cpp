#include <sparsepp/spp.h>
#include <sparsepp/spp_timer.h>

#include <sparsehash/dense_hash_map>
#include <sparsehash/sparse_hash_map>

#include <cstdio>
#include <utility>
#include <unordered_map>

#include <Judy.h>
#include <xxhash.h>
#include <t1ha.h>

using std::make_pair;

struct xxhash {
	size_t operator()(uint64_t str) const {
		return XXH64(&str, sizeof(uint64_t), 2001);
	}
};

struct t1hash {
	size_t operator()(uint64_t str) const {
		return t1ha(&str, sizeof(uint64_t), 2001);
	}
};

template <class T>
void test(T &s, int count, const char *name, const char *hash_fn_type)
{
	spp::Timer<std::milli> timer;

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.insert({rand(), {}});

	printf("%s,%s,%d,random inserts,%.2f\n", name, hash_fn_type, count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.find(rand());

	printf("%s,%s,%d,random finds,%.2f\n", name, hash_fn_type, count, timer.get_delta() / 1000);

	timer.snap();
	srand(1);
	for (int i = 0; i < count; ++i)
		s.find(rand());
	printf("%s,%s,%d,random not-finds,%.2f\n", name, hash_fn_type, count, timer.get_delta() / 1000);

	s.clear();
	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.insert({i, {}});
	printf("%s,%s,%d,sequential inserts,%.2f\n", name, hash_fn_type, count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.find(i);
	printf("%s,%s,%d,sequential finds,%.2f\n", name, hash_fn_type, count, timer.get_delta() / 1000);
	s.clear();

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.insert({-i, {}});

	printf("%s,%s,%d,neg sequential inserts,%.2f\n", name, hash_fn_type, count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		s.find(-i);

	printf("%s,%s,%d,neg sequential finds,%.2f\n", name, hash_fn_type, count, timer.get_delta() / 1000);
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

	printf("judy,nil,%d,random inserts,%.2f\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLGet(arr, rand(), NULL);

	printf("judy,nil,%d,random finds,%.2f\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(1);
	for (int i = 0; i < count; ++i)
		JudyLGet(arr, rand(), NULL);
	printf("judy,nil,%d,random not-finds,%.2f\n", count, timer.get_delta() / 1000);

	JudyLFreeArray(&arr, NULL);
	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLIns(&arr, i, NULL);
	printf("judy,nil,%d,sequential inserts,%.2f\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLGet(arr, i, NULL);

	printf("judy,nil,%d,sequential finds,%.2f\n", count, timer.get_delta() / 1000);
	JudyLFreeArray(&arr, NULL);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLIns(&arr, -i, NULL);

	printf("judy,nil,%d,neg sequential inserts,%.2f\n", count, timer.get_delta() / 1000);

	timer.snap();
	srand(0);
	for (int i = 0; i < count; ++i)
		JudyLGet(arr, -i, NULL);

	printf("judy,nil,%d,neg sequential finds,%.2f\n", count, timer.get_delta() / 1000);
	JudyLFreeArray(&arr, NULL);
}

int main(int argc, char const *argv[])
{
	if (argc < 4)
	{
		printf("usage: %s <judy|spp|dense|sparse|std> <std|xxhash|t1ha> <n_iterations>\n", argv[0]);
		return 1;
	}

	char const *test_type = argv[1];
	char const *hash_fn_type = argv[2];
	int const n_iterations = atoi(argv[3]);

	if (strcmp(test_type, "spp") == 0)
	{
		if (strcmp(hash_fn_type, "std") == 0) {
			spp::sparse_hash_map<uint32_t, void*, std::hash<uint32_t>> s;
			test(s, n_iterations, test_type, hash_fn_type);
		} else if (strcmp(hash_fn_type, "xxhash") == 0) {
			spp::sparse_hash_map<uint32_t, void*, xxhash> s;
			test(s, n_iterations, test_type, hash_fn_type);
		} else if (strcmp(hash_fn_type, "t1ha") == 0) {
			spp::sparse_hash_map<uint32_t, void*, t1hash> s;
			test(s, n_iterations, test_type, hash_fn_type);
		} else {
			printf ("Unsupported hash function.\n");
			return 1;
		}
		return 0;
	}

	if (strcmp(test_type, "sparse") == 0)
	{
		if (strcmp(hash_fn_type, "std") == 0) {
			google::sparse_hash_map<uint32_t, void*, std::hash<uint32_t>> s;
			s.set_deleted_key(-(INT_MAX - 1));
			test(s, n_iterations, test_type, hash_fn_type);
		} else if (strcmp(hash_fn_type, "xxhash") == 0) {
			google::sparse_hash_map<uint32_t, void*, xxhash> s;
			s.set_deleted_key(-(INT_MAX - 1));
			test(s, n_iterations, test_type, hash_fn_type);
		} else if (strcmp(hash_fn_type, "t1ha") == 0) {
			google::sparse_hash_map<uint32_t, void*, t1hash> s;
			s.set_deleted_key(-(INT_MAX - 1));
			test(s, n_iterations, test_type, hash_fn_type);
		} else {
			printf ("Unsupported hash function.\n");
			return 1;
		}
		return 0;
	}

	if (strcmp(test_type, "dense") == 0)
	{
		if (strcmp(hash_fn_type, "std") == 0) {
			google::dense_hash_map<uint32_t, void*, std::hash<uint32_t>> s;
			s.set_empty_key(-INT_MAX);
			s.set_deleted_key(-(INT_MAX - 1));
			test(s, n_iterations, test_type, hash_fn_type);
		} else if (strcmp(hash_fn_type, "xxhash") == 0) {
			google::dense_hash_map<uint32_t, void*, xxhash> s;
			s.set_empty_key(-INT_MAX);
			s.set_deleted_key(-(INT_MAX - 1));
			test(s, n_iterations, test_type, hash_fn_type);
		} else if (strcmp(hash_fn_type, "t1ha") == 0) {
			google::dense_hash_map<uint32_t, void*, t1hash> s;
			s.set_empty_key(-INT_MAX);
			s.set_deleted_key(-(INT_MAX - 1));
			test(s, n_iterations, test_type, hash_fn_type);
		} else {
			printf ("Unsupported hash function.\n");
			return 1;
		}
		return 0;
	}

	if (strcmp(test_type, "std") == 0)
	{
		if (strcmp(hash_fn_type, "std") == 0) {
			std::unordered_map<uint32_t, void*, std::hash<uint32_t>> s;
			test(s, n_iterations, test_type, hash_fn_type);
		} else if (strcmp(hash_fn_type, "xxhash") == 0) {
			std::unordered_map<uint32_t, void*, xxhash> s;
			test(s, n_iterations, test_type, hash_fn_type);
		} else if (strcmp(hash_fn_type, "t1ha") == 0) {
			std::unordered_map<uint32_t, void*, t1hash> s;
			test(s, n_iterations, test_type, hash_fn_type);
		} else {
			printf ("Unsupported hash function.\n");
			return 1;
		}
		return 0;
	}

	if (strcmp(test_type, "judy") == 0)
	{
		test_judy(n_iterations);
		return 0;
	}

	return 0;
}

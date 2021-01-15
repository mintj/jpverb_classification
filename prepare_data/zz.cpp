
std::map<int, int> shuffle_dict(int max = 200)
{
	std::vector<int> pool;
	for (int i = 0; i < max; ++i) {
		pool.push_back(i);
	}
	
	TRandom3 rndm(1997);
	std::map<int, int> dict;
	for (int i = 0; i < max; ++i) {
		int rest = pool.size();
		int j = (int)(rest*rndm.Rndm());
		dict[i] = pool[j];
		pool[j] = pool[rest-1];
		pool.pop_back();
	}
	return dict;
}

void zz()
{
	auto aa = shuffle_dict(200);
	for (int i = 0; i < 200; ++i) {
		cout << Form("shuffle_dict[%3d] = %3d", i, aa[i]) << endl;
	}
}

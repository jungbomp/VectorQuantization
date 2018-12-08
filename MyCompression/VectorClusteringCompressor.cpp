#include "stdafx.h"
#include "VectorClusteringCompressor.h"

//*****************************************************************************
//
// VectorClusteringCompressor.cpp : Defines the class
//
// Author - Jungbom Pak
//
//*****************************************************************************

#define MIN_UNUPDATED 5
#define MAX_KLUSTERING_PROCESS 1000


VectorClusteringCompressor::VectorClusteringCompressor() {
	pData = NULL; // Image data

	width = 0; // Image width
	height = 0; // Image height
	depth = 0;	// Image color depth

	numOfCluster = 0; // Count of cluster
	lengthOfVector = 0;	// Length Of vector
}


VectorClusteringCompressor::~VectorClusteringCompressor()
{
}

bool VectorClusteringCompressor::InitializeCordwords(vector<vector<int>>& codewords, const int numOfCluster, const int sizeOfVec, const byte* pData, const int w, const int h, const int d) {

	// N-Clusters according to N vectors
	unordered_map<int, int> myMap;
	unordered_map<int, vector<int>> myMap2;

	int width = static_cast<int>(ceil(sqrt(sizeOfVec)));
	int height = sizeOfVec / width;

	vector<int> origin(sizeOfVec);

	for (int i = 0; i < h / width; i++) {
		for (int j = 0; j < w / height; j++) {
			vector<int> vec;
			vec.reserve(sizeOfVec);

			for (int row = i * height; row < (i + 1) * height; row++) {
				for (int col = j * width; col < (j + 1) * width; col++) {
					vec.emplace_back(pData[w * row + col]);
				}
			}

			int distance = static_cast<int>(GetEuclideanDistance(vec, origin));
			if (myMap.find(distance) == myMap.end()) {
				myMap[distance] = 1;
				myMap2[distance] = vec;
				continue;
			}

			myMap[distance] = myMap[distance] + 1;
		}
	}

	vector<pair<int, int>> vec;
	vec.reserve(myMap.size());
	for (auto it : myMap) {
		vec.emplace_back(pair<int, int>(it.second, it.first));
	}

	sort(vec.begin(), vec.end());
	reverse(vec.begin(), vec.end());
	vector<pair<int, int>>::iterator it = vec.begin();
	for (int i = 0; i < numOfCluster; i++) {
		if (it != vec.end()) {
			codewords.emplace_back(myMap2[it->second]);
			it++;
			continue;
		}

		srand(time(NULL));
		vector<int> cordword(sizeOfVec);
		for (int j = 0; j < sizeOfVec; j++) {
			cordword[j] = rand() % 256;
		}

		codewords.emplace_back(cordword);
	}

	return true;

}

bool VectorClusteringCompressor::Clustering(const vector<vector<int>>& codewords, const int sizeOfVec, const byte* pData, const int w, const int h, const int d, vector<vector<vector<int>>>& clusters) {
	// N-Clusters according to N vectors

	int width = static_cast<int>(ceil(sqrt(sizeOfVec)));
	int height = sizeOfVec / width;

	if (clusters[0].size() < 1) {
		for (int i = 0; i < h / width; i++) {
			for (int j = 0; j < w / height; j++) {
				vector<int> vec;
				if (8 == d) {
					vec.reserve(sizeOfVec);

					for (int row = i * height; row < (i + 1) * height; row++) {
						for (int col = j * width; col < (j + 1) * width; col++) {
							vec.emplace_back(pData[w*row + col]);
						}
					}
				}
				else {
					vec.reserve(sizeOfVec * (d / 8));
					for (int row = i * height; row < (i + 1) * height; row++) {
						for (int col = j * width; col < (j + 1) * width; col++) {
							for (int k = 0; k < 3; k++) {
								vec.emplace_back(pData[(w * row + col) * 3 + k]);
							}
						}
					}
				}

				int minIndex = 0;
				double min = GetEuclideanDistance(vec, codewords[minIndex]);
				for (int i = 1; i < codewords.size(); i++) {
					double distance = GetEuclideanDistance(vec, codewords[i]);
					if (distance < min) {
						min = distance;
						minIndex = i;
					}
				}

				clusters[minIndex].emplace_back(vec);
			}
		}

		return true;
	}

	// Initialize cluters
	vector<vector<vector<int>>> locals(clusters.size());
	for (int i = 0; i < clusters.size(); i++) {
		//locals[i].reserve(1000);
		isUpdated[i].first = false;
	}

	int startMin = INT_MAX;
	for (int i = 0; i < clusters.size(); i++) {
		if (MIN_UNUPDATED <= isUpdated[i].second) continue;

		startMin = min(startMin, i);
		for (int j = 0; j < clusters[i].size(); j++) {
			vector<int>& vec = clusters[i][j];

			int minIndex = startMin;
			double minDistance = GetEuclideanDistance(vec, codewords[minIndex]);
			for (int k = minIndex + 1; k < codewords.size(); k++) {
				if (MIN_UNUPDATED <= isUpdated[k].second) continue;

				double distance = GetEuclideanDistance(vec, codewords[k]);
				if (distance < minDistance) {
					minDistance = distance;
					minIndex = k;
				}
			}

			locals[minIndex].emplace_back(vec);
			if (!isUpdated[minIndex].first) {
				int idx = locals[minIndex].size() - 1;
				if (idx < clusters[minIndex].size() && clusters[minIndex][idx] == vec) {
					continue;
				}
			}

			isUpdated[minIndex].first = true;
			isUpdated[minIndex].second = 0;
		}
	}

	for (int i = 0; i < clusters.size(); i++) {
		if (isUpdated[i].first) {
			clusters[i] = locals[i];
		}
		else {
			isUpdated[i].second++;
		}
	}

	return true;
}

int VectorClusteringCompressor::ComputeCodewords(const vector<vector<vector<int>>>& clusters, const int lengthOfVector, vector<vector<int>>& codewords) {
	int ret = 0;

	for (int i = 0; i < clusters.size(); i++) {
		if (!isUpdated[i].first) continue;

		vector<int> vec(lengthOfVector);

		if (clusters[i].size() < 1) {
			/* initialize random seed: */
			srand(time(NULL));
			for (int j = 0; j < lengthOfVector; j++) {
				vec[j] = rand() % 256;
			}

			ret++;
			codewords[i] = vec;
			continue;
		}

		for (int j = 0; j < lengthOfVector; j++) {
			double temp = 0.0;
			for (auto vector : clusters[i]) {
				temp += (vector[j] * (1.0 / (float)clusters[i].size()));
			}

			vec[j] = static_cast<int>(temp);
		}

		codewords[i] = vec;
		ret++;
	}

	return ret;
}

bool VectorClusteringCompressor::QuantizeVectors(const vector<vector<int>>& codewords, const int sizeOfVec, const byte* pData, const int w, const int h, const int d, vector<vector<int>>& quant) {
	const int width = static_cast<int>(ceil(sqrt(sizeOfVec)));
	const int height = sizeOfVec / width;

	const int quantWidth = static_cast<int>(w / width);
	const int quantHeight = static_cast<int>(h / height);

	quant.clear();
	quant.reserve(quantHeight);

	for (int i = 0; i < quantHeight; i++) {
		vector<int> row(quantWidth);

		for (int j = 0; j < quantWidth; j++) {
			vector<int> vec;
			if (8 == d) {
				vec.reserve(sizeOfVec);

				for (int row = i * height; row < (i + 1) * height; row++) {
					for (int col = j * width; col < (j + 1) * width; col++) {
						vec.emplace_back(pData[w*row + col]);
					}
				}
			}
			else {
				vec.reserve(sizeOfVec * (d / 8));
				for (int row = i * height; row < (i + 1) * height; row++) {
					for (int col = j * width; col < (j + 1) * width; col++) {
						for (int k = 0; k < 3; k++) {
							vec.emplace_back(pData[(w * row + col) * 3 + k]);
						}
					}
				}
			}

			int minIndex = 0;
			double min = GetEuclideanDistance(vec, codewords[minIndex]);
			for (int i = 1; i < codewords.size(); i++) {
				double distance = GetEuclideanDistance(vec, codewords[i]);
				if (distance < min) {
					min = distance;
					minIndex = i;
				}
			}

			row[j] = minIndex;
		}

		quant.emplace_back(row);
	}

	return true;
}

bool VectorClusteringCompressor::DecompressImage(const vector<vector<int>>& codewords, const vector<vector<int>>& quant, const int sizeOfVec, const int w, const int h, const int d, byte* pData) {
	int width = static_cast<int>(ceil(sqrt(sizeOfVec)));
	int height = sizeOfVec / width;

	for (int i = 0; i < quant.size(); i++) {
		for (int j = 0; j < quant[i].size(); j++) {
			vector<int> vec = codewords[quant[i][j]];
			vector<int>::iterator it = vec.begin();

			if (8 == depth) {
				for (int row = height * i; row < height * (i + 1); row++) {
					for (int col = width * j; col < width * (j + 1); col++) {
						pData[row * w + col] = static_cast<byte>(*it);
						it++;
					}
				}
			}
			else {
				for (int row = height * i; row < height * (i + 1); row++) {
					for (int col = width * j; col < width * (j + 1); col++) {
						for (int k = 0; k < 3; k++) {
							pData[(row * w + col) * 3 + k] = static_cast<byte>(*it);
							it++;
						}

					}
				}
			}


		}
	}

	return true;
}

bool VectorClusteringCompressor::Encode(const byte* pData, const int w, const int h, const int d, const int numCluster, const int vecSize) {
	if (!pData) return false;

	setImageData(pData);
	setWidth(w);
	setHeight(h);
	setDepth(d);
	setNumOfCluster(numCluster);
	setLengthOfVector(vecSize);

	codewords.reserve(numCluster);	// Allocate memory for codewords
	clusters.reserve(numCluster); // Allocate memory for cluster
	isUpdated.reserve(numCluster);

	// Initialize first N vectors (Divide diagonal by N) 
	for (int i = 0; i < numCluster; i++) {
		vector<int> vec;
		vec.reserve(vecSize * (d / 8));
		for (int j = 0; j < vecSize * (d / 8); j++) {
			vec.emplace_back(static_cast<int>((256 * (1.0 / ((double)numCluster + 1))) * (i + 1)));
		}

		codewords.emplace_back(vec);
		clusters.emplace_back(vector<vector<int>>());
		isUpdated.emplace_back(pair<bool, int>(true, 0));
	}

	int updated = numCluster;
	int cnt = 0;
	while (1 < updated && cnt < MAX_KLUSTERING_PROCESS) {
		Clustering(codewords, vecSize, pData, w, h, d, clusters);
		updated = ComputeCodewords(clusters, vecSize * (d / 8), codewords);
		printf("%d.Number of updated cluster: %d\n", ++cnt, updated);
	}

	QuantizeVectors(codewords, vecSize, pData, w, h, d, quant);

	return true;
}

bool VectorClusteringCompressor::Decode(byte* pData) {
	if (!pData) return false;

	return DecompressImage(codewords, quant, lengthOfVector, width, height, depth, pData);
}
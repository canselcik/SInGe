/*!
* simple-SDCH-gen
*
* Copyright 2014 Pavel Savchenkov <pavelsavchenkow@gmail.com>
* Released under the MIT license
*/

#include <fstream>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>

#include "dictionary.hpp"

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::cerr;

int main(int argc, char ** argv) {
	if (argc < 6) {
		cout << "./bin/pzip <contentPath> <outputDictPath> <maxDictSize> <minSubstLen> <minCountToSubst>" << endl;
		return -1;
	}

	vector<string> paths;
	const char* dir_files = argv[1];
	const char* dir_dict = argv[2];
	long lMaxDictSize = std::atol(argv[3]);
	long lMinSubstLen = std::atol(argv[4]);
	long lMinCountToSubst = std::atol(argv[5]);

	struct dirent *dp;
	DIR *dirp = opendir(dir_files);

	size_t fileCount = 0;
	cout << "Figuring out which files to read into memory..." << endl;
	while ((dp = readdir(dirp)) != NULL) {
		fileCount++;

		string path = std::string(dir_files) + "/" + dp->d_name;
		cout << "\t-> " << path << endl;

		struct stat st;
		if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode))
			paths.push_back(path);
	}

	cout << "Found" << fileCount << " files, reading them..." << endl;

	Dictionary dictionary;
	for (const auto& path : paths) {
		std::ifstream file(path);
		string content;

		std::copy(std::istreambuf_iterator<char>(file),
				  std::istreambuf_iterator<char>(),
				  std::back_inserter(content));
		file.close();
		dictionary.AddDocument(content);
	}

	cout << "Done reading. Building dictionary..." << endl;
	dictionary.BuildDict();

	cout << "Writing the dictionary to disk..." << endl;
	dictionary.OutputDictTo(string(dir_dict));

	cout << "COMPLETED in " << ((double) clock() / CLOCKS_PER_SEC) << ' seconds!\n';
	return 0;
}


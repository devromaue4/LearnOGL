#include "myutil.h"
#include <iostream>
#include <filesystem>

//#include <vector>
//#include <fstream>
//#include <sstream>

//unsigned long long GetCurrentTimeMillis() {
//#ifdef _WIN32
//	//return GetTickCount();
//	return GetTickCount64();
//#endif
//}

bool util::verifyPath(std::string_view path) { 
	namespace fs = std::filesystem;
	return fs::exists(path.data());
}
//bool util::verifyPath(std::string_view path) {
//	namespace fs = std::filesystem;
//	fs::path filePath = path.data();
//	if (!fs::exists(path.data()))
//		return false;
//
//	return true;
//}

// ----------------------------
// reads the contents of a text file
// ----------------------------

/*std::string util::readFile(const char* filename) {
	std::string code;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	shaderFile.open(filename);
	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();
	code = shaderStream.str();
	return code;
}

std::vector<char> util::readFileBin(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("failed to open file!");

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}*/

bool util::ReadFile(const char* fileName, std::string& outFile) {
	try {
		std::ifstream f(fileName);
		if (f.is_open()) {
			std::string line;
			while (getline(f, line)) { outFile.append(line); outFile.append("\n"); }
			f.close();
			return true;
		} else return false;
	}
	catch (std::ifstream::failure e) { log("ERROR::FILE_NOT_SUCCESFULLY_READ"); }

	return false;
}

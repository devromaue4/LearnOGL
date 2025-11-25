// ----------------------------------------------------
// Roman.P
// Copyright (c) 2025.
// ----------------------------------------------------
#include "../src/types.h"

#include <iostream>
//#include <print>
#include <fstream>
//#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define log(s) std::cout << s << '\n'
#define log_error(s) std::cerr << "Error: " << s << '\n'

int main(int argc, char* argv[]) try {
	if (argc != 2) {
		log("Usage: " << argv[0] << "<model filename>");
		return 1;
	}

	char* filename = argv[1];

	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(true);
	u8* data = stbi_load(filename, &width, &height, &nChannels, 0);
	if (!data) {
		log_error(stbi_failure_reason());
		throw std::exception("failed to load texture!");
	}

	// ---------------------------------------------------------------------
	// write bin data
	// ---------------------------------------------------------------------
	std::ofstream file("test.bin", std::ios::binary);
	if (!file) {
		log_error("can't create file!");
		throw std::exception("failed to write!");
	}

	file.write(reinterpret_cast<const char*>(data), width * height * nChannels);
	file.close();

	stbi_image_free(data);
	//system("pause");

	return 0;
}
catch (std::exception& e) { std::cout << "Standard error: " << e.what() << std::endl; }
catch (...) { std::cout << "Unknown error: \n"; }
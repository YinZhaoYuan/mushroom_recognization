#pragma once
#include <vector>
#include <cstdint>

struct image_t
{
	size_t width, height, channels;
	std::vector<uint8_t> data;
};

struct label_t
{
	uint8_t data;
};

bool load_images(const std::string& file_path, std::vector<image_t>& images, std::vector<label_t>& labels, size_t channels, size_t height, size_t width);

//bool load_labels(const std::string& file_path, std::vector<label_t>& labels);

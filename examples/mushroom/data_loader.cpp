#include <fstream>
#include <cassert>
#include <iostream>
#include <io.h>
#include "data_loader.h"

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;

template<typename T>
T reverse_endian(T p) {
	std::reverse(reinterpret_cast<char*>(&p), reinterpret_cast<char*>(&p)+sizeof(T));
	return p;
}

//inline bool is_little_endian() {
//	int x = 1;
//	return *(char*)&x != 0;
//}


void getFiles(string path, string subdir_name, vector<string>& files, vector<string>& subdir, vector<label_t>& labels)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p,s;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))//如果是目录,迭代之  
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//cout << fileinfo.name << endl;
					getFiles(p.assign(path).append("\\").append(fileinfo.name), fileinfo.name, files, subdir, labels);
				}
			}
			else//如果不是,加入列表  
			{
				files.push_back(p.assign(fileinfo.name));
				subdir.push_back(s.assign(subdir_name));
				label_t l;
				l.data = atoi(subdir_name.c_str());
				labels.push_back(l);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}



bool load_images(const std::string& file_path, std::vector<image_t>& images, std::vector<label_t>& labels, size_t imgResizeChannels, size_t imgResizeHeight, size_t imgResizeWidth)
{
	images.clear();

	cout << file_path.c_str() << endl;

	std::vector<string> files;
	std::vector<string> subdir;

	getFiles(file_path, "", files, subdir, labels);

	assert(files.size() > 0 && labels.size() > 0 && files.size() == labels.size());
	//images
	for (uint32_t i = 0; i < files.size(); i++)
	{
		image_t image;
		image.channels = imgResizeChannels;
		image.width = imgResizeWidth;
		image.height = imgResizeHeight;
		image.data.resize(imgResizeWidth*imgResizeHeight*imgResizeChannels);
		string sample_path = (file_path + "/" + subdir[i]) + "/" + files[i];
		cv::Mat sample = cv::imread(sample_path);
		assert(sample.channels() == imgResizeChannels);
		cv::resize(sample, sample, cv::Size((int)imgResizeWidth, (int)imgResizeHeight), 0, 0, cv::INTER_AREA);
		std::vector<uint8_t> vec;
		vec.assign((uint8_t*)sample.datastart, (uint8_t*)sample.dataend);
		image.data = vec;
		images.push_back(image);
	}
	
	return true;


	//images.clear();
	//std::ifstream ifs(file_path,std::ios::binary);
	//if (!ifs.is_open())
	//{
	//	return false;
	//}
	////detect platform information
	//const bool is_little_endian_flag = is_little_endian();
	////magic number
	//uint32_t magic_number;
	//ifs.read((char*)&magic_number, sizeof(magic_number));
	//if (is_little_endian_flag)
	//{
	//	magic_number = reverse_endian<uint32_t>(magic_number);
	//}
	//const bool magic_number_validate = (magic_number == 0x00000803);
	//assert(magic_number_validate);
	//if (!magic_number_validate)
	//{
	//	return false;
	//}
	////count
	//uint32_t images_total_count = 0;
	//ifs.read((char*)&images_total_count, sizeof(images_total_count));
	////image property
	//uint32_t width = 0, height = 0;
	//ifs.read((char*)&height, sizeof(height));
	//ifs.read((char*)&width, sizeof(width));
	//if (is_little_endian_flag)
	//{
	//	images_total_count = reverse_endian<uint32_t>(images_total_count);
	//	width = reverse_endian<uint32_t>(width);
	//	height = reverse_endian<uint32_t>(height);
	//}
	////images
	//for (uint32_t i = 0; i < images_total_count;i++)
	//{
	//	image_t image;
	//	image.channels = 1;
	//	image.width = width;
	//	image.height = height;
	//	image.data.resize(width*height);
	//	ifs.read((char*)&image.data[0], width*height);
	//	images.push_back(image);
	//}
	//return true;
}
//
//bool load_mnist_labels(const std::string& file_path, std::vector<label_t>& labels)
//{
//	labels.clear();
//	std::ifstream ifs(file_path, std::ios::binary);
//	if (!ifs.is_open())
//	{
//		return false;
//	}//detect platform information
//	const bool is_little_endian_flag = is_little_endian();
//	//magic number
//	uint32_t magic_number;
//	ifs.read((char*)&magic_number, sizeof(magic_number));
//	if (is_little_endian_flag)
//	{
//		magic_number = reverse_endian<uint32_t>(magic_number);
//	}
//	const bool magic_number_validate = (magic_number == 0x00000801);
//	assert(magic_number_validate);
//	if (!magic_number_validate)
//	{
//		return false;
//	}
//	//count
//	uint32_t labels_total_count = 0;
//	ifs.read((char*)&labels_total_count, sizeof(labels_total_count)); 
//	if (is_little_endian_flag)
//	{
//		labels_total_count = reverse_endian<uint32_t>(labels_total_count);
//	}
//	//labels
//	for (uint32_t i = 0; i < labels_total_count; i++)
//	{
//		label_t label;		
//		ifs.read((char*)&label.data, sizeof(label.data));
//		labels.push_back(label);
//	}
//	return true;
//}
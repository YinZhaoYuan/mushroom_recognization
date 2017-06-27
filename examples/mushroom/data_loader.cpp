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
	labels.clear();
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
		//assert(sample.channels() == imgResizeChannels);
		string sample_path = (file_path + "/" + subdir[i]) + "/" + files[i];
		int imread_color_flags = (imgResizeChannels ==1) ? 0 : 1;
		cv::Mat sample = cv::imread(sample_path, imread_color_flags);
		assert(sample.channels() == imgResizeChannels);
		cv::resize(sample, sample, cv::Size((int)imgResizeWidth, (int)imgResizeHeight), 0, 0, cv::INTER_AREA);
		std::vector<uint8_t> vec;
		vec.assign((uint8_t*)sample.datastart, (uint8_t*)sample.dataend);
		image.data = vec;
		images.push_back(image);
	}
	
	return true;
}
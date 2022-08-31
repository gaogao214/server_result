#pragma once
#include "GxJsonUtility.h"
#include "common.h"

#include <iostream>
#include <vector>
#include <io.h>
#include <fstream>
#include <filesystem>
#include "rapidjson/filereadstream.h"
namespace filestruct {

	struct ip_and_port {
		std::string ip;
		std::string port;
		bool operator<(const ip_and_port& b)const { return  port < b.port; }
		GX_JSON(ip, port);
	};
	struct id_and_server {
		int id;
		std::set<ip_and_port> server;
		GX_JSON(id, server);
	};
	struct files_Server {
		std::unordered_map<int, id_and_server> blocks;
		//vector<id_and_server> blocks;
		GX_JSON(blocks);
	};
	struct profile {//配置文件
		std::string path;
		std::string port;
		GX_JSON(path, port);
	};
	struct wget_c_file
	{
		std::string wget_name;
		uint32_t offset;
		GX_JSON(wget_name, offset);
	};
	struct wget_c_file_info
	{
		std::vector<wget_c_file> wget_c_file_list;
		GX_JSON(wget_c_file_list);
	};
};


inline filestruct::profile profile_;

inline std::string open_json_file(const std::string& json_name)//打开指定名称的json文本
{
	std::string content{};
	std::string tmp{};
	std::fill(content.begin(), content.end(), 0);       //清空
	std::fstream ifs(json_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
		return {};
	while (std::getline(ifs, tmp))
	{
		content.append(tmp);
	}
	return content;
}
inline void do_opendir()
{
	std::string readbuffer = open_json_file("open.json");
	profile_.deserializeFromJSON(readbuffer.c_str());
}


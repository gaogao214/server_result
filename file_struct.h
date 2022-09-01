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


static constexpr char profile_name[32] = "open.json";//配置文件名
static constexpr char list_name_[32] = "list.json";
static constexpr char id_name_[32] = "id.json";

inline filestruct::profile profile_;
inline filestruct::files_Server files_id;

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

inline void  parse_id_json()          //打开id_json文件 解析id.json得到IP port
{
	std::string readbuffer = open_json_file(id_name_);
	files_id.deserializeFromJSON(readbuffer.c_str());
}

inline int get_file_len(const std::string& filename)//获取文本的长度
{
	std::ifstream infile(filename.c_str());
	infile.seekg(0, std::ios_base::end);
	int fsize = infile.tellg();//list.json文本的大小
	infile.close();
	return fsize;
}

inline std::string get_file_context(const std::string& filename)//获取文本的内容
{

	std::ifstream File(filename.c_str());
	char file_buf = '0';//list.json文件
	std::string buf;//一个一个读之后存在这里，list.json文本
	while (File.get(file_buf))
	{
		buf.push_back(file_buf);
	}
	File.close();
	return buf;
}

inline void json_formatting(std::string& strtxt)
{
	unsigned int dzkh = 0; //括号的计数器
	bool isy = false; //是不是引号
	for (int i = 0; i < strtxt.length(); ++i) {
		if (isy || strtxt[i] == '"') // "前引号 "后引号
		{
			if (strtxt[i] == '"')
				isy = !isy;
			continue;
		}
		std::string tn = "";

#define ADD_CHANGE                          \
    for (unsigned int j = 0; j < dzkh; ++j) \
        tn += "\t";

		if (strtxt[i] == '{' || strtxt[i] == '[') {
			dzkh++;
			ADD_CHANGE
				strtxt = strtxt.substr(0, i + 1) + "\n" + tn + strtxt.substr(i + 1);
			i += dzkh + 1;
		}
		else if (strtxt[i] == '}' || strtxt[i] == ']') {
			dzkh--;
			ADD_CHANGE
				strtxt = strtxt.substr(0, i) + "\n" + tn + strtxt.substr(i);
			i += dzkh + 1;
		}
		else if (strtxt[i] == ',') {
			ADD_CHANGE
				strtxt = strtxt.substr(0, i + 1) + "\n" + tn + strtxt.substr(i + 1);
			i += dzkh + 1;
		}
	}
}
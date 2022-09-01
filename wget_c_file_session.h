#pragma once

#include "asio.hpp"
#include <iostream>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"
#include "upload_json_server.h"
#include "file_struct.h"
#include <deque>
#include <mutex>
#include "basic_session.hpp"
using namespace std;
using namespace asio::ip;

class wget_c_file_session:public basic_session
{
public:

	wget_c_file_session(asio::ip::tcp::socket socket)
		:basic_session(std::move(socket))
	{

	}

	
	//void start();

	void recive_wget_c_file_name();



	void do_wget_c_file();

	void send_file();


	//void send_file_file(const string& file_path_name,size_t remaining_total,int wget_offset, const string& wget_name, std::promise<bool>& promise);

protected:
	virtual int read_handle(uint32_t id)  override;

private:

	inline size_t get_file_len(const std::string& filename)
	{
		size_t fsize = 0;
		ifstream infile(filename.c_str());
		if (!infile.is_open())
		{
			cout << "文件夹内没有此文件 >:" << filename << endl;
			return 0;
		}
		infile.seekg(0, ios_base::end);
		fsize = infile.tellg();//list.json文本的大小
		infile.close();

		return fsize;
	}

	inline string get_file_context(const std::string& filename, int remaining_total, int offset/*,int count,int read_bytes=65536*/)//文本的内容
	{

		std::string content{};
		std::string tmp{};
		std::fill(content.begin(), content.end(), 0);//清空

		fstream ifs(filename, std::ios::in | std::ios::binary);
		if (!ifs.is_open())
			return {};

		//ifs.seekg(offset+(count*65536), ios::cur);
		ifs.seekg(offset, ios::cur);


		//ifs.seekg(i * send_count_size, ios::beg);   //文件指针移至断点值
		//ifs.read(count_file_buf, nleft);            //读4096个字符
		while (std::getline(ifs, tmp))
		{
			content.append(tmp);
		}

		return content;
	}

	void write(const std::string& msg);
	void do_write();


private:
	std::deque<std::string> write_msgs_;
	//std::deque<char *> write_msgs_;
	//std::deque<std::array<char, 8192 + 1024>> write_msgs_;

	std::mutex write_mtx_;

	char refile_name[1024] = { 0 };//接收名字的长度

	size_t filelen = 0;         //名字的长度

	char refile_file_len[sizeof(size_t)] = { 0 };//接收名字的长度

	string file_path_name;//路径+文件名

	std::string send_wget_name_and_offset_len{};

	std::string count_file_buf;//读到的字符
	//char* count_file_buf;//读到的字符

	size_t file_size = 0;//文件的总大小

	string wget_file_string;//发送断点文件中的文件名下的内容

	string wget_c_file_text; //接收文件的内容

	int buf_len;   //接收文件内容的长度

	filestruct::wget_c_file_info wcfi;   //断点续传文件 中  vector<>

	string wget_name;        //断点续传中的文件名

	size_t wget_offset;		//断点续传中的偏移量

	//string send_wget_name_and_offset_len;   //发送名字  偏移量  余下内容的长度
	string name_and_offset_remaining;       /* 名字  偏移量  余下内容的长度 以及余下的内容拼在一起 */

	size_t remaining_total;                 //计算余下的长度

	//upload_json_server ujs;

	char len[sizeof(size_t)] = { 0 };

	int count = 0;

	std::size_t send_size = 8192;


	std::size_t nchunkcount_;
	std::size_t nleft_;


	std::string readbuffer;
};



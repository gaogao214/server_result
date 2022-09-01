#pragma once

#include "asio.hpp"
#include <iostream>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"
#include "basic_session.hpp"

using namespace std;
using namespace asio::ip;

class upload_file_session :public basic_session
{


public:
	upload_file_session(tcp::socket socket)
		:basic_session(std::move(socket))
	{
	
	}




private:
	void do_recive_filename();
	void do_send_file(char id, const string& filename);


protected:
	virtual int read_handle(uint32_t id)  override;

private:
	
	char refile_name[1024] = { 0 };//接收名字的长度	
	std::string file_path_name;//路径+文件名
	size_t file_size;//文件的总大小	
	string file_string;//文件的内容


	std::size_t nchunkcount;
	std::size_t nleft;
	std::size_t send_count_size = 8192;

};

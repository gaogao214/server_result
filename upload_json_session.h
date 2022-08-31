#pragma once

#include "asio.hpp"
#include "rapidjson/rapidjson.h"

#include <iostream>
#include <vector>
#include <io.h>
#include <fstream>
#include <filesystem>
#include "rapidjson/filereadstream.h"
#include "basic_server.h"
#include "file_struct.h"
#include "basic_session.hpp"

extern filestruct::profile profile_;

class server_page;

class upload_json_session:public basic_session
{

public:

	upload_json_session(asio::ip::tcp::socket socket)
		:basic_session(std::move(socket))
	{
	
	}

	void start();

	//std::string open_json_file(const std::string& json_name);

	int get_file_len(const std::string& filename);//读文件的长度

	std::string get_file_context(const std::string& filename);//读文件的内容

	//void do_opendir();

	void save_json_file(filestruct::files_Server files_id, const std::string& name);

	void parse_id_json();

	void delete_all_ip_port();

protected:
	virtual int read_handle()  override;

private:

	void do_send_list();//发送 list.json 名字长度和名字

	void do_send_id();//发送 id.json 名字长度和名字

	void recive_id_port();//接收客户端传过来的id和port

	void gsh(std::string& strtxt);

	void delete_ip_port(filestruct::ip_and_port delete_ip_port);

	void save_file_server_ip_port_id_json();//在id.json里添加传输(a.txt...)的服务器ip port

private:


	std::string profile_name = "open.json";//配置文件名
	std::string list_name = "list.json";
	std::string id_name = "id.json";
	std::string list_name_text;//list.josn的长度+名字
	std::string send_list_file;//发送list.json文本的长度+内容
	std::string send_id_name;//id.josn的长度+名字
	std::string send_id_file;//发送id.json文本的长度+内容

	char id_port[1024] = { 0 };//接收的内容
	int id;
	filestruct::files_Server files_id;
	filestruct::files_Server files_id1;
	filestruct::ip_and_port save_ip_port;
	server_page* ser_ptr_;
	
};


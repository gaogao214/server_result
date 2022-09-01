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

class upload_json_session
	: public basic_session
	, public std::enable_shared_from_this<upload_json_session>
{

public:

	upload_json_session(asio::ip::tcp::socket socket)
		:basic_session(std::move(socket))
	{
		delete_all_ip_port();
		do_opendir();
		parse_id_json();
		save_file_server_ip_port_id_json();
	}

	void save_json_file(filestruct::files_Server files_id, const std::string& name);

	void delete_all_ip_port();

protected:
	virtual int read_handle(uint32_t id)  override;

private:

	void do_send_list();//发送 list.json 名字长度和名字

	void do_send_id();//发送 id.json 名字长度和名字

	void delete_ip_port(filestruct::ip_and_port delete_ip_port);

	void save_file_server_ip_port_id_json();//在id.json里添加传输(a.txt...)的服务器ip port


};


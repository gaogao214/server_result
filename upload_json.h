#pragma once
#include "asio.hpp"
#include "rapidjson/rapidjson.h"
#include "GxJsonUtility.h"
#include "common.h"
#include <iostream>
#include <vector>
#include <io.h>
#include <fstream>
#include <filesystem>
#include "rapidjson/filereadstream.h"

using namespace std;

namespace filestruct {

	struct ip_and_port {
		std::string ip;
		std::string port;
		bool operator<(const ip_and_port& b)const { return  port < b.port; }
		GX_JSON(ip, port);
	};
	struct id_and_server {
		int id;
		set<ip_and_port> server;
		GX_JSON(id, server);
	};
	struct files_Server {
		unordered_map<int, id_and_server> blocks;
		//vector<id_and_server> blocks;
		GX_JSON(blocks);
	};
	struct profile {//�����ļ�
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
		vector<wget_c_file> wget_c_file_list;
		GX_JSON(wget_c_file_list);
	};
};

//class upload_json_session;
//extern upload_json_session* upload_file_json;
extern filestruct::profile profile_;
class server_page;
//class upload_json_sesssion;
class upload_json
{
public:

	upload_json(asio::ip::tcp::socket socket);

	void start();

	std::string open_json_file(const std::string& json_name);

	int get_file_len(const std::string& filename);//���ļ��ĳ���

	std::string get_file_context(const std::string& filename);//���ļ�������

	void do_opendir();

	void save_json_file(filestruct::files_Server files_id, const string& name);

	void parse_id_json();

	void delete_all_ip_port();

private:

	void do_send_list();//���� list.json ���ֳ��Ⱥ�����

	void do_send_id();//���� id.json ���ֳ��Ⱥ�����

	void recive_id_port();//���տͻ��˴�������id��port

	void gsh(std::string& strtxt);

	void delete_ip_port(filestruct::ip_and_port delete_ip_port);

	void save_file_server_ip_port_id_json();//��id.json����Ӵ���(a.txt...)�ķ�����ip port

	asio::ip::tcp::socket socket_;

	string profile_name = "open.json";//�����ļ���
	string list_name = "list.json";
	string id_name = "id.json";
	string list_name_text;//list.josn�ĳ���+����
	string send_list_file;//����list.json�ı��ĳ���+����
	string send_id_name;//id.josn�ĳ���+����
	string send_id_file;//����id.json�ı��ĳ���+����

	char id_port[1024] = { 0 };//���յ�����
	int id;
	filestruct::files_Server files_id;
	filestruct::files_Server files_id1;
	filestruct::ip_and_port save_ip_port;
	server_page* ser_ptr_;
	//std::shared_ptr<upload_json_session> upload_json_session_;
};



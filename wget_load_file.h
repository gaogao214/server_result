#pragma once
#include "asio.hpp"
#include <iostream>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"
#include "upload_json.h"

#include <deque>
#include <mutex>

using namespace std;
using namespace asio::ip;

class wget_load_file:public std::enable_shared_from_this<wget_load_file>
{

public:
	wget_load_file(tcp::socket socket);

	void start();

	void recive_wget_c_file_name();

	void recive_wget_c_file(const string& file);

	void do_wget_c_file(const string& file_name);

	void send_file();

	
	//void send_file_file(const string& file_path_name,size_t remaining_total,int wget_offset, const string& wget_name, std::promise<bool>& promise);
private:

	inline size_t get_file_len(const std::string& filename)
	{
		size_t fsize = 0;
		ifstream infile(filename.c_str());
		if (!infile.is_open())
		{
			cout << "�ļ�����û�д��ļ� >:" << filename << endl;
			return 0;
		}	
		infile.seekg(0, ios_base::end);
		fsize = infile.tellg();//list.json�ı��Ĵ�С
		infile.close();

		return fsize;
	}

	inline string get_file_context(const std::string& filename, int remaining_total,int offset/*,int count,int read_bytes=65536*/)//�ı�������
	{

		std::string content{};
		std::string tmp{};
		std::fill(content.begin(), content.end(), 0);//���

		fstream ifs(filename, std::ios::in | std::ios::binary);
		if (!ifs.is_open())
			return {};

		//ifs.seekg(offset+(count*65536), ios::cur);
		ifs.seekg(offset, ios::cur);


		//ifs.seekg(i * send_count_size, ios::beg);   //�ļ�ָ�������ϵ�ֵ
		//ifs.read(count_file_buf, nleft);            //��4096���ַ�
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
	
	asio::ip::tcp::socket socket_;
	
	char refile_name[1024] = { 0 };//�������ֵĳ���
	
	size_t filelen=0;         //���ֵĳ���
	
	char refile_file_len[sizeof(size_t)] = {0};//�������ֵĳ���
	
	string file_path_name;//·��+�ļ���
	
	std::string send_wget_name_and_offset_len{};
	
	std::string count_file_buf;//�������ַ�
	//char* count_file_buf;//�������ַ�
	
	size_t file_size=0;//�ļ����ܴ�С
	
	string wget_file_string;//���Ͷϵ��ļ��е��ļ����µ�����
	
	string wget_c_file_text; //�����ļ�������
	
	int buf_len;   //�����ļ����ݵĳ���
	
	filestruct::wget_c_file_info wcfi;   //�ϵ������ļ� ��  vector<>
	
	string wget_name;        //�ϵ������е��ļ���
	
	size_t wget_offset;		//�ϵ������е�ƫ����
	
	//string send_wget_name_and_offset_len;   //��������  ƫ����  �������ݵĳ���
	string name_and_offset_remaining;       /* ����  ƫ����  �������ݵĳ��� �Լ����µ�����ƴ��һ�� */
	
	size_t remaining_total;                 //�������µĳ���
	
	upload_json ujs;
	
	char len[sizeof(size_t)] = {0};
	
	int count = 0;

	std::size_t send_size = 8192;


	std::size_t nchunkcount_;
	std::size_t nleft_;

};

class wget_c_file_accept
{
public:
	wget_c_file_accept(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		: io_service_(io_context)
		, acceptor_(io_context, endpoint)
	{
		do_accept();
	}

	void do_accept()
	{
		acceptor_.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					cout << "����˶˿� 12313 �Ϳͻ��˶˿� 12313 ���ӳɹ�\n";
					auto fs = std::make_shared<wget_load_file>(std::move(socket));
					fs->start();
					sessions.push_back(fs);
				}
				else
				{
					cout << "����˶˿� 12313 �Ϳͻ��˶˿� 12313 ����ʧ��\n";
				}
				do_accept();
			});
	}

	void run()
	{
		io_service_.run();
	}

private:
	std::vector<std::shared_ptr<wget_load_file>> sessions;
	asio::ip::tcp::acceptor acceptor_;
	asio::io_service& io_service_;

};

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
			cout << "�ļ�����û�д��ļ� >:" << filename << endl;
			return 0;
		}
		infile.seekg(0, ios_base::end);
		fsize = infile.tellg();//list.json�ı��Ĵ�С
		infile.close();

		return fsize;
	}

	inline string get_file_context(const std::string& filename, int remaining_total, int offset/*,int count,int read_bytes=65536*/)//�ı�������
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

	char refile_name[1024] = { 0 };//�������ֵĳ���

	size_t filelen = 0;         //���ֵĳ���

	char refile_file_len[sizeof(size_t)] = { 0 };//�������ֵĳ���

	string file_path_name;//·��+�ļ���

	std::string send_wget_name_and_offset_len{};

	std::string count_file_buf;//�������ַ�
	//char* count_file_buf;//�������ַ�

	size_t file_size = 0;//�ļ����ܴ�С

	string wget_file_string;//���Ͷϵ��ļ��е��ļ����µ�����

	string wget_c_file_text; //�����ļ�������

	int buf_len;   //�����ļ����ݵĳ���

	filestruct::wget_c_file_info wcfi;   //�ϵ������ļ� ��  vector<>

	string wget_name;        //�ϵ������е��ļ���

	size_t wget_offset;		//�ϵ������е�ƫ����

	//string send_wget_name_and_offset_len;   //��������  ƫ����  �������ݵĳ���
	string name_and_offset_remaining;       /* ����  ƫ����  �������ݵĳ��� �Լ����µ�����ƴ��һ�� */

	size_t remaining_total;                 //�������µĳ���

	//upload_json_server ujs;

	char len[sizeof(size_t)] = { 0 };

	int count = 0;

	std::size_t send_size = 8192;


	std::size_t nchunkcount_;
	std::size_t nleft_;


	std::string readbuffer;
};



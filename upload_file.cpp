#include "upload_file.h"
#include "fstream"

	//upload_json_session* upload_file_json = nullptr;
	upload_file_session::upload_file_session(asio::ip::tcp::socket socket)
		:socket_(std::move(socket))
	{
		
	}

	void upload_file_session::start()
	{
		do_recive_filename();	
	}

	void upload_file_session::do_recive_filename()//�����ļ�����
	{
		std::memset(refile_name, 0, 1024);//����ڴ�
		socket_.async_read_some(asio::buffer(refile_name, 1024),
			[this](std::error_code ec, std::size_t sz)
			{
				if (!ec)
				{
					std::size_t name_len = 0;
					std::memcpy(&name_len, refile_name, sizeof(size_t));

					std::string file_name(refile_name + sizeof(size_t));
					cout << "�����ļ���>: " << file_name << endl;
					do_send_file(file_name);
				} 
			});
	}

	void upload_file_session::do_send_file(const string filename)//�����ļ�����
	{	
		file_string.clear();	
		file_size = 0;
		file_path_name = profile_.path + "\\" + filename;
			//һ���Զ�����  ��ʼ

 		ifstream file(file_path_name.c_str(), ios::in | ios::binary);
		if (!file.is_open())//����ļ��Ƿ����
			return;

		file.seekg(0, ios_base::end); 
		file_size = file.tellg();//list.json�ı��Ĵ�С
		file.seekg(0, ios_base::beg);
		
		char* count_file_buf = new char[file_size];
		while (!file.eof())//û�н���
		{
			file.read(count_file_buf, file_size);	
		}			
		file.close();
		//����
	
		char buffer[4096] = { 0 };

		std::memcpy(buffer, &file_size, 8);
		std::memcpy(buffer + 8, count_file_buf, file_size);

		//file_string.resize(sizeof(size_t) + file_size);
		//std::memcpy(file_string.data(), &file_size, sizeof(size_t));
		//sprintf(&file_string[0], "%zd", file_size);

		/*file_string[sizeof(size_t)] = '*';*/
		
		//std::sprintf(&file_string[sizeof(size_t)/*+1*/], "%s", count_file_buf);
		std::memset(count_file_buf, 0, file_size);//����ڴ�

		//delete count_file_buf;
		//count_file_buf = nullptr;
		asio::async_write(socket_, asio::buffer(buffer,file_size+8),
			[this,filename](std::error_code ec, std::size_t sz)
			{
				if (!ec)
				{
					std::cout <<filename.data() << "  �ļ����ͳɹ�\n";
					do_recive_filename();									
				}				
			});
	}




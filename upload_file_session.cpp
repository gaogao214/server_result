#include "upload_file_session.h"
#include "fstream"
#include "upload_json_session.h"
#include "response.hpp"
#include "request.hpp"
//upload_json_session* upload_file_json = nullptr;


void upload_file_session::do_recive_filename()//接收文件名字
{
	//std::memset(refile_name, 0, 1024);//清空内存
	//socket_.async_read_some(asio::buffer(refile_name, 1024),
	//	[this](std::error_code ec, std::size_t sz)
	//	{
	//		if (!ec)
	//		{
	//			//std::size_t id = 0;
	//			//char file_name[32];
	//			/*std::memcpy(&id, &refile_name, sizeof(size_t));
	//			std::memcpy(file_name,&refile_name+s izeof(size_t), 8);*/
	//			//	std::memcpy(&id,&refile_name,1);

	//			char id = refile_name[0];
	//			size_t id_num = atoi(&id);
	//			std::string file_name(refile_name + 1/* sizeof(size_t)*/);
	//			Sleep(2);

	//			OutputDebugStringA(file_name.data());
	//			OutputDebugString(L"\n");

	//			do_send_file(id, file_name);
	//		}

	//		do_recive_filename();
	//	});
	//Sleep(2);

}


int upload_file_session::read_handle(uint32_t id)
{
	switch (id)
	{
	case 1002:

		id_name_request req;
		req.parse_bytes(buffer_);
		auto _id = req.body_.id_;
		auto file_name = req.body_.name_;


		do_send_file(_id, file_name);
		Sleep(2);


		break;
	}


	return 0;
}

void upload_file_session::do_send_file(char id, const string& filename)//发送文件内容
{
	file_string.clear();
	file_size = 0;
	file_path_name = profile_.path + "\\" + filename;
	//一次性读出来  开始
	Sleep(2);

	ifstream file(file_path_name.c_str(), ios::in | ios::binary);
	if (!file.is_open())//检查文件是否存在
		return;

	file.seekg(0, ios_base::end);
	file_size = file.tellg();//list.json文本的大小
	file.seekg(0, ios_base::beg);

	//char* count_file_buf = new char[file_size];
	//while (!file.eof())//没有结束
	//{
	//	file.read(count_file_buf, file_size);
	//}
	//file.close();
	////结束

	if (file_size > send_count_size)
	{
		nchunkcount = file_size / send_count_size;	//文件块数	=  文件总长度 / 一次读到的大小

		if (file_size % nchunkcount != 0)
		{
			nchunkcount++;
		}
		for (int i = 0; i < nchunkcount; i++)
		{
			if (i + 1 == nchunkcount)
			{
				//using namespace std::chrono_literals;
				//std::this_thread::sleep_for(200ms);
				nleft = file_size - send_count_size * (nchunkcount - 1);
				
			}
			else
			{
				//using namespace std::chrono_literals;
				//std::this_thread::sleep_for(200ms);
				nleft = send_count_size;
				
			}
			char* count_file_buf = new char[nleft];

			file.seekg(i * send_count_size, ios::beg);   //文件指针移至断点值
			file.read(count_file_buf, nleft);            //读4096个字符

			//char buffer[4096 + 8] = { 0 };
			char buffer[8192 + 1024] = { 0 };

			//std::memcpy(buffer, &file_size, 8);
			//std::memcpy(buffer + 8, count_file_buf, file_size);
			//std::size_t sum_number = nleft + 8 + 1 + 16;
			//std::memcpy(buffer, &sum_number, 8);              //客户端需要接收的大小    
			//std::memcpy(buffer + 8, &id, 1);
			//std::memcpy(buffer + 9, filename.data(), 16);      //名字
			//std::memcpy(buffer + 25, &nchunkcount, 8);         //块数
			//std::memcpy(buffer + 33, count_file_buf, nleft);  //内容


			id_text_response it_resp;
			
			it_resp.header_.length_ = nleft;
			std::memcpy(it_resp.header_.name_, filename.data(), filename.size());
			it_resp.header_.totoal_ = nchunkcount;
			it_resp.body_.id_ = id;
			it_resp.body_.set_const_text_(count_file_buf);




			//std::memset(count_file_buf, 0, file_size);//清空内存
			std::memset(count_file_buf, 0, nleft);//清空内存
			this->async_write(std::move(it_resp), [this, filename](std::error_code ec, std::size_t sz)
				{
					if (!ec)
					{
						OutputDebugString(L"\n发送成功\n");

					}
				});
			//asio::async_write(socket_, asio::buffer(buffer, file_size + 8),

			/*asio::async_write(socket_, asio::buffer(buffer, sum_number + 8),
				[this, filename](std::error_code ec, std::size_t sz)
				{
					if (!ec)
					{
						std::cout << filename.data() << "  文件发送成功\n";

					}
				});*/
		}

	}
	else if (file_size < send_count_size)
	{
		nleft = file_size;
		char* count_file_buf = new char[nleft];

		file.read(count_file_buf, nleft);            //读4096个字符

		char buffer[8192] = { 0 };
		std::size_t total_num = 1;

		//std::memcpy(buffer, &file_size, 8);
		//std::memcpy(buffer + 8, count_file_buf, file_size);
		std::size_t sum_number = nleft + 8 + 1 + 16;

		//std::memcpy(buffer, &sum_number, 8);
		//std::memcpy(buffer + 8, &id, 1);
		//std::memcpy(buffer + 9, filename.data(), 16);
		//std::memcpy(buffer + 25, &total_num, 8);
		//std::memcpy(buffer + 33, count_file_buf, nleft);


		id_text_response it_resp;
		std::memset(it_resp.header_.name_, 0, 32);//清空内存

		std::memcpy(it_resp.header_.name_, filename.data(), filename.size());
		it_resp.header_.length_ = nleft;
		it_resp.header_.totoal_ = total_num;
		it_resp.body_.id_ = id;
		it_resp.body_.set_const_text_(count_file_buf);


		//std::memset(count_file_buf, 0, file_size);//清空内存
		std::memset(count_file_buf, 0, nleft);//清空内存


		this->async_write(std::move(it_resp), [this, filename](std::error_code ec, std::size_t sz)
				{
					if (!ec)
					{
						//	std::cout << filename.data() << "  文件发送成功\n";
						OutputDebugString(L"\n");
						OutputDebugStringA(filename.data());
						OutputDebugString(L"\n");

					}
				});
		//asio::async_write(socket_, asio::buffer(buffer, file_size + 8),

		//asio::async_write(socket_, asio::buffer(buffer, sum_number + 8),
		//	[this, filename](std::error_code ec, std::size_t sz)
		//	{
		//		if (!ec)
		//		{
		//			//	std::cout << filename.data() << "  文件发送成功\n";
		//			OutputDebugString(L"\n");
		//			OutputDebugStringA(filename.data());
		//			OutputDebugString(L"\n");

		//		}
		//	});
	}



	file.close();

}



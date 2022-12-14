#include "wget_c_file_session.h"
#include "file_struct.h"
#include "request.hpp"
#include "response.hpp"
//void wget_c_file_session::start()
//{
//	//recive_wget_c_file_name();
//	do_opendir();
//
//}

/*接收断点续传名字以及文本内容*/
void wget_c_file_session::recive_wget_c_file_name()
{
	
	name_text_request req;
	
	req.parse_bytes(buffer_);


	ofstream wget_c_file(req.body_.name_, ios::binary);
	wget_c_file.write(req.body_.text_,req.header_.length_);
	wget_c_file.close();

	//socket_.async_read_some(asio::buffer(refile_name, 1024),
	//	[this](std::error_code ec, std::size_t)
	//	{
	//		if (!ec)
	//		{
	//			std::memcpy(&filelen, refile_name, sizeof(size_t));  //名字的长度
	//			std::string file_name(refile_name + sizeof(size_t));//名字
	//			std::cout << "接收" << file_name << "文件\n";
	//			recive_wget_c_file(file_name);
	//		}
	//	});

}

int wget_c_file_session::read_handle(uint32_t id)
{
	switch (id)
	{
	case 1003:

		name_text_request req;

		req.parse_bytes(buffer_);

		readbuffer = req.body_.text_;
		ofstream wget_c_file(req.body_.name_, ios::binary);
		wget_c_file.write(req.body_.text_, req.header_.length_);
		wget_c_file.close();
		send_file();
		break;
	}
	return 0;
}

/*解析json文件*/
void wget_c_file_session::do_wget_c_file()
{
	//string readbuffer = send_file_context(file_name);
	/*string readbuffer = open_json_file(file_name);*/
	wcfi.deserializeFromJSON(readbuffer.data());
}

/*发送 名字 偏移量 内容长度   内容*/    /*比较偏移量*/
void wget_c_file_session::send_file()
{
	file_size = 0;
	remaining_total = 0;
	do_opendir();
	do_wget_c_file();
	for (auto& iter : wcfi.wget_c_file_list)//遍历断点续传中的文件
	{
		wget_name = iter.wget_name;    //名字
		wget_offset = iter.offset;     //偏移量                            
		file_path_name = profile_.path + "\\" + wget_name;  //找到断点时 本地的文件

		file_size = get_file_len(file_path_name);          //计算文件长度

		ifstream file(file_path_name.c_str(), ios::in | ios::binary);      //打开文件
		if (!file.is_open())//检查文件是否存在
			return;



		if (wget_offset < file_size)
		{
			remaining_total = file_size - wget_offset;   //计算余下的长度

			if (remaining_total > send_size)
			{
				nchunkcount_ = remaining_total / send_size;	//文件块数	=  文件总长度 / 一次读到的大小

				if (remaining_total % nchunkcount_ != 0)
				{
					nchunkcount_++;
				}

				for (int i = 0; i < nchunkcount_; i++)
				{
					if (i + 1 == nchunkcount_)
					{
						nleft_ = remaining_total - send_size * (nchunkcount_ - 1);
					}
					else
					{
						nleft_ = send_size;
					}


					char* count_file_buf = new char[nleft_];

					std::size_t offset_ = i * send_size + wget_offset;
					file.seekg(offset_, ios::beg);   //文件指针移至断点值
					file.read(count_file_buf, nleft_);            //读4096个字符


					//char buffer_[8192 + 1024] = { 0 };
					//std::size_t sum_size_ = nleft_ + 8 + 8 + 8;

					//std::memcpy(buffer_, &sum_size_, 8);         //字符串总长度 （名字  总序号  偏移量  内容）
					//std::memcpy(buffer_ + 8, wget_name.data(), 8);
					//std::memcpy(buffer_ + 16, &nchunkcount_, 8);
					//std::memcpy(buffer_ + 24, &offset_, 8);
					//std::memcpy(buffer_ + 32, count_file_buf, nleft_);

					offset_text_response resp;
					resp.header_.length_ = nleft_;
					resp.header_.totoal_ = nchunkcount_;
					std::memcpy(resp.header_.name_,wget_name.data(),wget_name.size());
					resp.body_.offset_ = offset_;
					resp.body_.set_text(count_file_buf);

					std::memset(count_file_buf, 0, nleft_);

					/*std::string send_wget_name_and_offset_len(buffer_);
					write(send_wget_name_and_offset_len);*/
					this->async_write(std::move(resp), [this](std::error_code ec, std::size_t sz)
						{
							if (ec)
								return;
						});
				/*	asio::async_write(socket_, asio::buffer(buffer_, sum_size_ + 8),
						[this](std::error_code ec, std::size_t sz)
						{
							if (ec)
								return;
						});*/

				}

			}
			else if (remaining_total < send_size)
			{
				remaining_total = file_size - wget_offset;   //计算余下的长度
				char* count_file_buf = new char[remaining_total];
				file.read(count_file_buf, remaining_total);            //读remaining_total个字符

				//char buffer[8192] = { 0 };
				std::size_t total_num = 1;

				//std::size_t sum_number = remaining_total + 8 + 8 + 8;

			/*	std::memcpy(buffer, &sum_number, 8);
				std::memcpy(buffer + 8, wget_name.data(), 8);
				std::memcpy(buffer + 16, &total_num, 8);
				std::memcpy(buffer + 24, &wget_offset, 8);

				std::memcpy(buffer + 32, count_file_buf, remaining_total);*/


				offset_text_response resp;
				resp.header_.length_ = remaining_total;
				resp.header_.totoal_ = total_num;
				std::memcpy(resp.header_.name_, wget_name.data(), wget_name.size());
				resp.body_.offset_ = wget_offset;
				resp.body_.set_text(count_file_buf);
				std::memset(count_file_buf, 0, remaining_total);//清空内存

				this->async_write(std::move(resp), [this](std::error_code ec, std::size_t sz)
					{
						if (ec)
							return;
					});
				/*asio::async_write(socket_, asio::buffer(buffer, sum_number + 8),
					[this](std::error_code ec, std::size_t sz)
					{
						if (ec)
							return;
					});*/
			}

		}
	}
}




void wget_c_file_session::write(const std::string& msg)
{
	std::unique_lock lock(write_mtx_);
	bool write_in_progress = !write_msgs_.empty();
	write_msgs_.push_back(msg);
	if (!write_in_progress)
	{
		do_write();
	}
}



void wget_c_file_session::do_write()
{


	//asio::async_write(socket_, asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
	//	[this](std::error_code ec, std::size_t /*length*/)
	//	{
	//		if (!ec)
	//		{
	//			std::unique_lock lock(write_mtx_);
	//			write_msgs_.pop_front();
	//			if (!write_msgs_.empty())
	//			{
	//				OutputDebugString(L"s 断点续传时的文件发送成功");

	//				cout << "文件发送成功\n";
	//				do_write();

	//			}
	//		}
	//		else
	//		{
	//			socket_.close();
	//		}
	//	});
}










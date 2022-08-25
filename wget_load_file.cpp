#include "wget_load_file.h"

wget_load_file::wget_load_file(asio::ip::tcp::socket socket)
	:socket_(std::move(socket))
	,ujs(std::move(socket))
{

}

void wget_load_file::start()
{
	recive_wget_c_file_name();
	ujs.do_opendir();

}

/*接收断点续传名字*/
void wget_load_file::recive_wget_c_file_name()
{
	socket_.async_read_some(asio::buffer(refile_name, 1024),
		[this](std::error_code ec, std::size_t)
		{
			if (!ec)
			{
				std::memcpy(&filelen, refile_name, sizeof(size_t));  //名字的长度
				std::string file_name(refile_name+sizeof(size_t));//名字
				std::cout <<"接收"<<file_name<<"文件\n";
				recive_wget_c_file(file_name);
			}
		});

}

/*接收断点续传的文件*/
void wget_load_file::recive_wget_c_file(const string& file_name)  
{
	auto self = shared_from_this();
	//asio::async_read(socket_, asio::buffer(refile_file_len, sizeof(size_t)),   //接收断点续传文件的内容
	socket_.async_read_some(asio::buffer(refile_file_len, sizeof(size_t)),   //接收断点续传文件的内容
		[self,this, file_name](std::error_code ec, std::size_t)
		{
			if (!ec)
			{		
				memcpy(&buf_len,refile_file_len,sizeof(size_t));   //文件的大小
				cout << "buf_len  " << buf_len << endl;
				wget_c_file_text.resize(buf_len);                 
			
				//asio::async_read(socket_, asio::buffer(wget_c_file_text, buf_len),   //接收
				socket_.async_read_some(asio::buffer(wget_c_file_text, buf_len),   //接收
					[self, this, file_name](std::error_code ec, std::size_t)
					{
						if (!ec)
						{
							const char* readbufs = wget_c_file_text.data();
							ofstream wget_c_file(file_name.data(), ios::binary);
							wget_c_file.write(wget_c_file_text.c_str(), buf_len);
							wget_c_file.close();
							cout << file_name.data() << "文件接收成功\n";
							do_wget_c_file(file_name);
						}
						send_file();

					});
			}
		});	
}

/*解析json文件*/
void wget_load_file::do_wget_c_file(const string& file_name)
{
	//string readbuffer = send_file_context(file_name);
	string readbuffer = ujs.open_json_file(file_name);
	wcfi.deserializeFromJSON(readbuffer.c_str());
}

/*发送 名字 偏移量 内容长度   内容*/    /*比较偏移量*/
void wget_load_file::send_file()
{
	file_size = 0;
	remaining_total = 0;
	//wget();
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

				
							char buffer_[8192 + 1024] = { 0 };
							std::size_t sum_size_ = nleft_ + 8 + 8 + 8;

							std::memcpy(buffer_ , &sum_size_ , 8 );         //字符串总长度 （名字  总序号  偏移量  内容）
							std::memcpy(buffer_ + 8, wget_name.data(), 8);
							std::memcpy(buffer_ + 16, &nchunkcount_  , 8);
							std::memcpy(buffer_ + 24, &offset_, 8);
							std::memcpy(buffer_ + 32, count_file_buf, nleft_);


							std::memset(count_file_buf , 0 , nleft_);

							/*std::string send_wget_name_and_offset_len(buffer_);
							write(send_wget_name_and_offset_len);*/

							asio::async_write(socket_, asio::buffer(buffer_, sum_size_ + 8),
								[this](std::error_code ec, std::size_t sz)
								{
									if (ec)
										return;
								});

						}

				}
				else if(remaining_total < send_size)
				{
					remaining_total = file_size - wget_offset;   //计算余下的长度
					char* count_file_buf = new char[remaining_total];
					file.read(count_file_buf, remaining_total);            //读remaining_total个字符

					char buffer[8192] = { 0 };
					std::size_t total_num = 1;

					std::size_t sum_number = remaining_total + 8 + 8 + 8 ;

					std::memcpy(buffer, &sum_number, 8);
					std::memcpy(buffer + 8, wget_name.data(), 8);
					std::memcpy(buffer + 16, &total_num, 8);
					std::memcpy(buffer + 24, &wget_offset, 8);

					std::memcpy(buffer + 32, count_file_buf, remaining_total);

					std::memset(count_file_buf, 0, remaining_total);//清空内存

					asio::async_write(socket_, asio::buffer(buffer, sum_number + 8),
						[this](std::error_code ec, std::size_t sz)
						{
							if (ec)
								return;
						});
				}
		
		}
	}
}




void wget_load_file::write(const std::string& msg)
{
	std::unique_lock lock(write_mtx_);
	bool write_in_progress = !write_msgs_.empty();
	write_msgs_.push_back(msg);
	if (!write_in_progress)
	{
		do_write();
	}
}



void wget_load_file::do_write()
{


	asio::async_write(socket_, asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
		[this](std::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				std::unique_lock lock(write_mtx_);
				write_msgs_.pop_front();
				if (!write_msgs_.empty())
				{
					OutputDebugString(L"s 断点续传时的文件发送成功");

					cout << "文件发送成功\n";
					do_write();

				}
			}
			else
			{
				socket_.close();
			}
		});
}










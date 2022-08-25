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

/*���նϵ���������*/
void wget_load_file::recive_wget_c_file_name()
{
	socket_.async_read_some(asio::buffer(refile_name, 1024),
		[this](std::error_code ec, std::size_t)
		{
			if (!ec)
			{
				std::memcpy(&filelen, refile_name, sizeof(size_t));  //���ֵĳ���
				std::string file_name(refile_name+sizeof(size_t));//����
				std::cout <<"����"<<file_name<<"�ļ�\n";
				recive_wget_c_file(file_name);
			}
		});

}

/*���նϵ��������ļ�*/
void wget_load_file::recive_wget_c_file(const string& file_name)  
{
	auto self = shared_from_this();
	//asio::async_read(socket_, asio::buffer(refile_file_len, sizeof(size_t)),   //���նϵ������ļ�������
	socket_.async_read_some(asio::buffer(refile_file_len, sizeof(size_t)),   //���նϵ������ļ�������
		[self,this, file_name](std::error_code ec, std::size_t)
		{
			if (!ec)
			{		
				memcpy(&buf_len,refile_file_len,sizeof(size_t));   //�ļ��Ĵ�С
				cout << "buf_len  " << buf_len << endl;
				wget_c_file_text.resize(buf_len);                 
			
				//asio::async_read(socket_, asio::buffer(wget_c_file_text, buf_len),   //����
				socket_.async_read_some(asio::buffer(wget_c_file_text, buf_len),   //����
					[self, this, file_name](std::error_code ec, std::size_t)
					{
						if (!ec)
						{
							const char* readbufs = wget_c_file_text.data();
							ofstream wget_c_file(file_name.data(), ios::binary);
							wget_c_file.write(wget_c_file_text.c_str(), buf_len);
							wget_c_file.close();
							cout << file_name.data() << "�ļ����ճɹ�\n";
							do_wget_c_file(file_name);
						}
						send_file();

					});
			}
		});	
}

/*����json�ļ�*/
void wget_load_file::do_wget_c_file(const string& file_name)
{
	//string readbuffer = send_file_context(file_name);
	string readbuffer = ujs.open_json_file(file_name);
	wcfi.deserializeFromJSON(readbuffer.c_str());
}

/*���� ���� ƫ���� ���ݳ���   ����*/    /*�Ƚ�ƫ����*/
void wget_load_file::send_file()
{
	file_size = 0;
	remaining_total = 0;
	//wget();
	for (auto& iter : wcfi.wget_c_file_list)//�����ϵ������е��ļ�
	{ 
		wget_name = iter.wget_name;    //����
		wget_offset = iter.offset;     //ƫ����                            
		file_path_name = profile_.path + "\\" + wget_name;  //�ҵ��ϵ�ʱ ���ص��ļ�

		file_size = get_file_len(file_path_name);          //�����ļ�����

		ifstream file(file_path_name.c_str(), ios::in | ios::binary);      //���ļ�
		if (!file.is_open())//����ļ��Ƿ����
			return;

		
	
		if (wget_offset < file_size)
		{
				remaining_total = file_size - wget_offset;   //�������µĳ���

				if (remaining_total > send_size)
				{
						nchunkcount_ = remaining_total / send_size;	//�ļ�����	=  �ļ��ܳ��� / һ�ζ����Ĵ�С

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
							file.seekg(offset_, ios::beg);   //�ļ�ָ�������ϵ�ֵ
							file.read(count_file_buf, nleft_);            //��4096���ַ�

				
							char buffer_[8192 + 1024] = { 0 };
							std::size_t sum_size_ = nleft_ + 8 + 8 + 8;

							std::memcpy(buffer_ , &sum_size_ , 8 );         //�ַ����ܳ��� ������  �����  ƫ����  ���ݣ�
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
					remaining_total = file_size - wget_offset;   //�������µĳ���
					char* count_file_buf = new char[remaining_total];
					file.read(count_file_buf, remaining_total);            //��remaining_total���ַ�

					char buffer[8192] = { 0 };
					std::size_t total_num = 1;

					std::size_t sum_number = remaining_total + 8 + 8 + 8 ;

					std::memcpy(buffer, &sum_number, 8);
					std::memcpy(buffer + 8, wget_name.data(), 8);
					std::memcpy(buffer + 16, &total_num, 8);
					std::memcpy(buffer + 24, &wget_offset, 8);

					std::memcpy(buffer + 32, count_file_buf, remaining_total);

					std::memset(count_file_buf, 0, remaining_total);//����ڴ�

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
					OutputDebugString(L"s �ϵ�����ʱ���ļ����ͳɹ�");

					cout << "�ļ����ͳɹ�\n";
					do_write();

				}
			}
			else
			{
				socket_.close();
			}
		});
}










#include "wget_c_file_session.h"
#include "file_struct.h"
#include "request.hpp"
#include "response.hpp"

static constexpr std::size_t send_size = 8192;

int wget_c_file_session::read_handle(uint32_t id)
{
	switch (id)
	{
	case request_number::name_text_request:

		name_text_request req;

		req.parse_bytes(buffer_);

		std::ofstream wget_c_file(req.body_.name_, std::ios::binary);
		wget_c_file.write(req.body_.text_, req.header_.length_);
		wget_c_file.close();

		do_wget_c_file(req.body_.text_);

		OutputDebugStringA(req.body_.name_);
		OutputDebugString(L"接收成功\n");
		send_file();
		break;
	}
	return 0;
}



void wget_c_file_session::send_file()
{
	size_t file_size = 0;

	size_t remaining_total = 0 ;   

	std::size_t nleft_=0;

	do_opendir();
	for (auto& iter : wcfi.wget_c_file_list)
	{
		auto wget_name = iter.wget_name;    
		auto wget_offset = iter.offset;                               
		std::string file_path_name = profile_.path + "\\" + wget_name;  

		file_size = get_file_len(file_path_name);         

		std::ifstream file(file_path_name.c_str(), std::ios::in | std::ios::binary);
		if (!file.is_open())
			return;



		if (wget_offset < file_size)
		{
			remaining_total = file_size - wget_offset;   

			if (remaining_total > send_size)
			{
				nchunkcount_ = remaining_total / send_size;

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


					//char* count_file_buf = new char[nleft_];
					std::unique_ptr<char[]> count_file_buf(new char[nleft_]);

					std::size_t offset_ = i * send_size + wget_offset;
					file.seekg(offset_, std::ios::beg);   
					file.read(count_file_buf.get(), nleft_);

					offset_text_response resp;
					resp.header_.length_ = nleft_;
					resp.header_.totoal_ = nchunkcount_;
					resp.header_.set_name(wget_name.data());
					resp.body_.offset_ = offset_;
					resp.body_.set_text(count_file_buf.get());


					this->async_write(std::move(resp), [&,this](std::error_code ec, std::size_t sz)
						{
							if (ec)
								return;
							OutputDebugStringA(wget_name.data());
							OutputDebugStringA("文件发送成功\n");
							
						});
				}

			}
			else if (remaining_total < send_size)
			{
				remaining_total = file_size - wget_offset;   
				std::unique_ptr<char[]> count_file_buf(new char[remaining_total]);

				file.read(count_file_buf.get(), remaining_total);

				std::size_t total_num = 1;

				offset_text_response resp;
				resp.header_.length_ = remaining_total;
				resp.header_.totoal_ = total_num;
				resp.header_.set_name(wget_name.data());
				resp.body_.offset_ = wget_offset;
				resp.body_.set_text(count_file_buf.get());

				//this->async_write(std::move(resp));

				this->async_write(std::move(resp), [&, this](std::error_code ec, std::size_t sz)
					{
						if (ec)
							return;
						OutputDebugStringA(wget_name.data());
						OutputDebugStringA("文件发送成功\n");

					});
			}

		}
	}
}








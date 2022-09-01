#include "wget_c_file_session.h"
#include "file_struct.h"
#include "request.hpp"
#include "response.hpp"


int wget_c_file_session::read_handle(uint32_t id)
{
	switch (id)
	{
	case request_number::name_text_request:

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

/*����json�ļ�*/
void wget_c_file_session::do_wget_c_file()
{
	wcfi.deserializeFromJSON(readbuffer.data());
}

/*���� ���� ƫ���� ���ݳ���   ����*/    /*�Ƚ�ƫ����*/
void wget_c_file_session::send_file()
{
	file_size = 0;
	remaining_total = 0;
	do_opendir();
	do_wget_c_file();
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

					offset_text_response resp;
					resp.header_.length_ = nleft_;
					resp.header_.totoal_ = nchunkcount_;
					std::memcpy(resp.header_.name_, wget_name.data(), wget_name.size());
					resp.body_.offset_ = offset_;
					resp.body_.set_text(count_file_buf);

					std::memset(count_file_buf, 0, nleft_);

					this->async_write(std::move(resp), [this](std::error_code ec, std::size_t sz)
						{
							if (ec)
								return;
						});
				}

			}
			else if (remaining_total < send_size)
			{
				remaining_total = file_size - wget_offset;   //�������µĳ���
				char* count_file_buf = new char[remaining_total];
				file.read(count_file_buf, remaining_total);            //��remaining_total���ַ�

				std::size_t total_num = 1;

				offset_text_response resp;
				resp.header_.length_ = remaining_total;
				resp.header_.totoal_ = total_num;
				std::memcpy(resp.header_.name_, wget_name.data(), wget_name.size());
				resp.body_.offset_ = wget_offset;
				resp.body_.set_text(count_file_buf);
				std::memset(count_file_buf, 0, remaining_total);//����ڴ�

				this->async_write(std::move(resp));
			}

		}
	}
}








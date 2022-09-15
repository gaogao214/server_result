#include "upload_file_session.h"
#include <fstream>
#include "response.hpp"
#include "request.hpp"
#include "upload_json_session.h"

static constexpr std::size_t send_count_size = 8192;

int upload_file_session::read_handle(uint32_t id)
{
	switch (id)
	{
	case request_number::id_name_request:

		id_name_request req;
		req.parse_bytes(buffer_);
		auto _id = req.body_.id_;
		auto file_name = req.body_.name_;
		OutputDebugStringA(file_name);
		OutputDebugString(L"接收成功\n");

		do_send_file(_id, file_name);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);

		break;
	}


	return 0;
}

void upload_file_session::do_send_file(uint32_t id, const string& filename)
{
	std::size_t file_size = 0;
	std::string file_path_name = profile_.path + "\\" + filename;

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(200ms);

	ifstream file(file_path_name.c_str(), ios::in | ios::binary);
	if (!file.is_open())
		return;

	file.seekg(0, ios_base::end);
	file_size = file.tellg();
	file.seekg(0, ios_base::beg);

	std::size_t nleft{};

	if (file_size > send_count_size)
	{
		std::size_t nchunkcount = file_size / send_count_size;

		if (file_size % nchunkcount != 0)
		{
			nchunkcount++;
		}
		for (std::size_t i = 0; i < nchunkcount; i++)
		{
			if (i + 1 == nchunkcount)
			{
				nleft = file_size - send_count_size * (nchunkcount - 1);

			}
			else
			{
				nleft = send_count_size;

			}
			std::unique_ptr<char[]> count_file_buf(new char[nleft]);

			file.seekg(i * send_count_size, ios::beg);
			file.read(count_file_buf.get(), nleft);

			id_text_response it_resp;

			it_resp.header_.length_ = nleft;
			it_resp.header_.set_name(filename);
			it_resp.header_.totoal_ = nchunkcount;
			it_resp.body_.id_ = id;
			it_resp.body_.set_text_(count_file_buf.get()); //查看count_file_buf是否是空



			this->async_write(std::move(it_resp), [this, filename](std::error_code ec, std::size_t sz)
				{
					if (!ec)
					{
						OutputDebugStringA(filename.data());
						OutputDebugString(L"发送成功\n");
					}
				});
		}

	}
	else if (file_size < send_count_size)
	{
		nleft = file_size;
		std::unique_ptr<char[]> count_file_buf(new char[nleft]);

		file.read(count_file_buf.get(), nleft);

		id_text_response it_resp;
		it_resp.header_.set_name(filename);
		it_resp.header_.length_ = nleft;
		it_resp.body_.id_ = id;
		it_resp.body_.set_text_(count_file_buf.get());//查看count_file_buf是否是空

		this->async_write(std::move(it_resp), [this, filename](std::error_code ec, std::size_t sz)
			{
				if (!ec)
				{
					OutputDebugStringA(filename.data());
					OutputDebugString(L"发送成功\n");
				}
			});
	}

	file.close();
}



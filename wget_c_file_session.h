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


class wget_c_file_session:public basic_session
{
public:
	wget_c_file_session(asio::ip::tcp::socket socket)
		:basic_session(std::move(socket))
	{

	}

private:

	void send_file();

protected:

	virtual int read_handle(uint32_t id)  override;

private:
	std::deque<std::string> write_msgs_;

	std::mutex write_mtx_;

	std::size_t nchunkcount_;
};



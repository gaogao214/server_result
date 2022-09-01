#pragma once
#include "asio.hpp"
#include "basic_server.h"
#include "wget_c_file_session.h"


class wget_c_file_server : public basic_server<wget_c_file_session>
{
public:
	wget_c_file_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:basic_server<wget_c_file_session>(io_context, endpoint)
	{
	}
};

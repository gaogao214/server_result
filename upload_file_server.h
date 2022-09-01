#pragma once
#include "basic_server.h"
#include "upload_file_session.h"

class upload_file_server : public basic_server<upload_file_session>
{
public:
	upload_file_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		: basic_server<upload_file_session>(io_context,endpoint)
	{

	}
};

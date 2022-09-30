#pragma once
#include "asio.hpp"
#include "basic_server.h"
#include "upload_json_session.h"

class upload_json_server : public basic_server<upload_json_session>
{
	
public:
	upload_json_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:basic_server<upload_json_session>(io_context, endpoint)
	{
	
	}



};

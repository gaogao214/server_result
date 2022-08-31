#pragma once

#include "asio.hpp"

#include "wget_c_file_session.h"


class wget_c_file_server
	:public basic_server<wget_c_file_session>
{
public:
	wget_c_file_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:basic_server<wget_c_file_session>(io_context,endpoint)
		/*: io_service_(io_context)
		, acceptor_(io_context, endpoint)*/
	{
		//do_accept();
	}

	/*void do_accept()
	{
		acceptor_.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					cout << "服务端端口 12313 和客户端端口 12313 连接成功\n";
					auto fs = std::make_shared<wget_load_file>(std::move(socket));
					fs->start();
					sessions.push_back(fs);
				}
				else
				{
					cout << "服务端端口 12313 和客户端端口 12313 连接失败\n";
				}
				do_accept();
			});
	}

	void run()
	{
		io_service_.run();
	}*/

//public:
//	virtual int read_handle() override
//	{
//	
//	}

private:
	/*std::vector<std::shared_ptr<wget_load_file>> sessions;
	asio::ip::tcp::acceptor acceptor_;
	asio::io_service& io_service_;*/

};

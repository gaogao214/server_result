#pragma once

#include "upload_file_session.h"
#include "basic_server.h"

class upload_file_server 
	:public  basic_server<upload_file_session>
{
public:
	upload_file_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		//:acceptor_(io_context, endpoint)
		: basic_server<upload_file_session>(io_context,endpoint)
	{
		//do_accept();
	}

	//void do_accept()
	//{
	//	acceptor_.async_accept(
	//		[this](std::error_code ec, asio::ip::tcp::socket socket)
	//		{
	//			if (!ec)
	//			{
	//				cout << "服务端端口 12314 和客户端端口 12314 连接成功\n";
	//				OutputDebugString(L"12314 连接成功");

	//				auto fs = std::make_shared<upload_file_server>(std::move(socket));
	//				fs->start();
	//				sessions.push_back(fs);
	//			}
	//			else {
	//				cout << "服务端端口 12314 和客户端端口 12314 连接失败\n";
	//				OutputDebugString(L"12314 连接失败");

	//			}
	//			do_accept();
	//		});
	//}
//public:
//	virtual int read_handle() override
//	{
//
//	}

private:
	//std::vector<std::shared_ptr<upload_file_session>> sessions; 
	//asio::ip::tcp::acceptor acceptor_; 

};

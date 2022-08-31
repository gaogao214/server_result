#pragma once
#include "asio.hpp"
#include "rapidjson/rapidjson.h"

#include <iostream>
#include <vector>
#include <io.h>
#include <fstream>
#include <filesystem>
#include "rapidjson/filereadstream.h"
#include "basic_server.h"
#include "file_struct.h"
#include "upload_json_session.h"
using namespace std;


class server_page;

class upload_json_server 
	:public basic_server<upload_json_session>
{
	
public:
		upload_json_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:basic_server(io_context, endpoint)
	{
	
	}
		
	//void do_accept()
	//{
	//	//ser_ptr_->ui.text_log->insertPlainText(u8"服务器启动成功\n");

	//	acceptor_.async_accept(
	//		[this](std::error_code ec, asio::ip::tcp::socket socket)
	//		{
	//			if (!ec)
	//			{
	//				//ser_ptr_->ui.text_log->insertPlainText(u8"端口12312 连接成功\n");				
	//				emit sign_text_log(u8"端口12312连接成功\n");
	//				auto s = std::make_shared<upload_json>(std::move(socket));
	//				s->start();
	//				sessions.push_back(s);
	//			}
	//			else {
	//				//ser_ptr_->ui.text_log->insertPlainText(u8"s端口12312 连接失败\n");
	//				OutputDebugString(L"s端口12312 连接失败");
	//				emit sign_text_log(u8"端口12312连接失败\n");

	//			}

	//			do_accept();
	//		});
	//}

	/*	void run()
		{
			io_context_.run();
		}*/


private:

	//asio::ip::tcp::acceptor acceptor_;
	//std::vector<std::shared_ptr<upload_json>> sessions;
	//asio::io_context& io_context_;

};

#include "upload_json_session.h"
#include <iostream>
#include "upload_json.h"
#include "server_page.h"
	upload_json_session::upload_json_session(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint/*,server_page* sep_*/)
		:io_context_(io_context)
		,acceptor_(io_context, endpoint)
		//,ser_ptr_(sep_)
	{
		start();
	}

	void upload_json_session::start()
	{
		do_accept();
	}

	void upload_json_session::do_accept()
	{
		//ser_ptr_->ui.text_log->insertPlainText(u8"服务器启动成功\n");

		acceptor_.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					//ser_ptr_->ui.text_log->insertPlainText(u8"端口12312 连接成功\n");				
					emit sign_text_log(u8"端口12312连接成功\n");
					auto s = std::make_shared<upload_json>(std::move(socket));
					s->start();
					sessions.push_back(s);
				}
				else {					
					//ser_ptr_->ui.text_log->insertPlainText(u8"s端口12312 连接失败\n");
					OutputDebugString(L"s端口12312 连接失败");
					emit sign_text_log(u8"端口12312连接失败\n");

				}			

				do_accept();
			});
	}

	void upload_json_session::run()
	{
		io_context_.run();
	}

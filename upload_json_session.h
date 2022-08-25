#pragma once

#include "asio.hpp"
#include <iostream>
#include <QObject>
#include "upload_json.h"
class server_page;
class upload_json_session:public QObject,public std::enable_shared_from_this<upload_json_session>
{
	Q_OBJECT
public:
	upload_json_session(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint/*,server_page* sep_*/);
		
	void start();
	
	void do_accept();

	void run();

signals:
	void sign_text_log(QString text);

private:
	asio::ip::tcp::acceptor acceptor_;
	std::vector<std::shared_ptr<upload_json>> sessions;
	asio::io_context& io_context_;
	//server_page* ser_ptr_;
};

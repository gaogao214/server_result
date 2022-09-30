#include "asio.hpp"
#include "server_page.h"
#include <QPushbutton>
#include <QPainter>
#include "upload_file_server.h"
#include "wget_load_file_server.h"
server_page::server_page(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	qRegisterMetaType<QTextCursor>("QTextCursor");

	connect(ui.connect, &QPushButton::clicked, this, &server_page::start_connect);  
}

server_page::~server_page()
{}

void server_page::start_connect()
{
	do_profile_upload();

	do_upload_file();

	create_reset_upload();
}

void server_page::do_profile_upload()
{
	main_thread_ptr_.reset(new std::thread([this]()
		{
			ui.text_log->insertPlainText(u8"按钮被按下\n");

			asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), std::atoi("12312"));

			uj_ = std::make_shared<upload_json_server>(io_context, endpoint);

			/*QMetaObject::Connection connect_text_log=QObject::connect(uj_.get(),SIGNAL(sign_text_log(QString)),this,SLOT(show_text_log(QString)),Qt::DirectConnection);
			if (connect_text_log)
			{
				ui.text_log->insertPlainText(u8"text log 信号与槽函数连接成功\n");
			}*/

			uj_->run();
		}));

	main_thread_ptr_->detach();
}

void server_page::create_reset_upload()
{
	std::thread t1([this]()
		{
			asio::ip::tcp::endpoint endpoint_1(asio::ip::tcp::v4(), std::atoi("12313"));

			auto wcfs=std::make_shared<wget_c_file_server>(io_context_go_on, endpoint_1);

			wcfs->run();
		});

	t1.detach();
}

void server_page::do_upload_file()
{
	std::thread t2([this]()
		{
			asio::ip::tcp::endpoint _endpoint(asio::ip::tcp::v4(), std::atoi("12314"));

			auto cs = std::make_shared<upload_file_server>(io_context_upload, _endpoint);

			cs->run();
		});

	t2.detach();
}

void server_page::show_text_log(QString text_)
{
	ui.text_log->insertPlainText(text_);
}

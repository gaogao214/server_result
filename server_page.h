#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_server_page.h"
#include "upload_json_server.h"


class server_page : public QMainWindow
{
    Q_OBJECT

public:
    server_page(QWidget *parent = nullptr);
    ~server_page();
    
    void do_profile_upload();

    void create_reset_upload();

    void do_upload_file();

public:
    Ui::server_pageClass ui;
   
public slots:  
   
    void start_connect(); 

private:
  
    std::shared_ptr<upload_json_server> uj_;

    std::shared_ptr<std::thread> main_thread_ptr_;

    asio::io_context io_context;

    asio::io_context io_context_upload;

    asio::io_context io_context_go_on;

};




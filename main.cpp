#include "server_page.h"
#include <QtWidgets/QApplication>
#include "asio.hpp"

#include "server_page.h"
#include <QStyleFactory>
#include <Qimage>


int main(int argc, char *argv[]){
  
    std::filesystem::remove("wget_c_file_1.json");
    std::filesystem::remove("wget_c_file_2.json");

    QApplication a(argc, argv);
    server_page w;
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    w.setWindowIcon(QIcon("images/server.ico"));

    w.setWindowTitle(u8"版本管理工具服务端");
   
    w.show();
    
    return a.exec();
}

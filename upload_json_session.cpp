#include "upload_json_session.h"


#include "upload_json_server.h"
#include "server_page.h"
#include "ui_server_page.h"
#include "server_page.h"
#include "request.hpp"
#include "response.hpp"


void upload_json_session::start()
{
    delete_all_ip_port();
    do_opendir();
    parse_id_json();
    save_file_server_ip_port_id_json();     //在id.json里添加本地配置文件中的ip port 传输(a.txt...)的服务器ip port
}

void  upload_json_session::parse_id_json()          //打开id_json文件 解析id.json得到IP port
{
    string readbuffer = open_json_file(id_name);
    files_id.deserializeFromJSON(readbuffer.c_str());
}

void upload_json_session::delete_all_ip_port()          //在程序开始之前  删除 id.json中的IP port
{

    parse_id_json();
    for (auto& iter : files_id.blocks)
    {
        //  iter.server.erase(iter.server.begin(), iter.server.end());
        iter.second.server.erase(iter.second.server.begin(), iter.second.server.end());
    }
    save_json_file(files_id, id_name);//保存  id.json文件
    parse_id_json();
}

void upload_json_session::save_file_server_ip_port_id_json()//在id.json里添加传输(a.txt...)的服务器ip port
{

    filestruct::ip_and_port first_ip_port;
    first_ip_port.ip = socket_.local_endpoint().address().to_string();//服务器传输文件的ip
    first_ip_port.port = profile_.port;//服务器传输文件的port
    for (auto& iter : files_id.blocks)
    {
        // iter.server.insert(first_ip_port);//应该用一个集合
        iter.second.server.insert(first_ip_port);//应该用一个集合
    }
    save_json_file(files_id, id_name); //保存id.json文件
    do_send_list();
}

int upload_json_session::get_file_len(const std::string& filename)//获取文本的长度
{
    ifstream infile(filename.c_str());
    infile.seekg(0, ios_base::end);
    int fsize = infile.tellg();//list.json文本的大小
    infile.close();
    return fsize;
}

std::string upload_json_session::get_file_context(const std::string& filename)//获取文本的内容
{

    ifstream File(filename.c_str());
    char file_buf = '0';//list.json文件
    string buf;//一个一个读之后存在这里，list.json文本
    while (File.get(file_buf))
    {
        buf.push_back(file_buf);
    }
    File.close();
    return buf;
}

void upload_json_session::do_send_list()//发送list.json
{



    std::string list_buf = get_file_context(list_name);
    //string list_parse = "0" + list_name + "*" + list_buf;
    //size_t list_name_text_size = list_parse.size();
    //list_name_text.resize(list_name_text_size + sizeof(size_t));
    ////std::memcpy(list_name_text.data(), &list_name_text_size, sizeof(size_t));
    //sprintf(&list_name_text[0], "%zd", list_name_text_size);

    //sprintf(&list_name_text[sizeof(size_t)], "%s", list_parse.c_str());

    //char buffer[4096] = { 0 };
    //std::memcpy(buffer, &list_name_text_size, sizeof(std::size_t));
    //std::memcpy(buffer + 8, list_parse.data(), list_parse.size());

    name_text_response res;
   
    res.body_.set_name_text(list_name,list_buf);
    auto self = shared_from_this();
    async_write(std::move(res),
        [this](std::error_code ec, std::size_t sz)
            {
                if (!ec)
                {
                    //ser_ptr_->ui.text_log->insertPlainText(u8"上传list.json文件\n");
                   // ser_ptr_->ui.text_log->append(u8"上传list.hson文件");
                    OutputDebugString(L"list.json 发送成功\n");

                    //cout << self->list_name << "发送成功\n";
                    do_send_id();//发送id.json名字
                }
            });

    //asio::async_write(socket_, asio::buffer(/*list_name_text.data(), list_name_text.size()*/buffer, 8 + list_name_text_size),
    //    [this](std::error_code ec, std::size_t sz)
    //    {
    //        if (!ec)
    //        {
    //            //ser_ptr_->ui.text_log->insertPlainText(u8"上传list.json文件\n");
    //           // ser_ptr_->ui.text_log->append(u8"上传list.hson文件");
    //            OutputDebugString(L"发送list.json");

    //            cout << list_name << "发送成功\n";
    //            do_send_id();//发送id.json名字
    //        }
    //    });
    
}

void upload_json_session::do_send_id()//发送id.json名字和内容
{

    std::string id_buf = get_file_context(id_name);//获取文件的内容
    //std::string id_name_text = "1" + id_name + "*" + id_buf; //把名字和内容拼接在一起
    //size_t id_name_text_size = id_name_text.size();     //计算出拼接后的字符串长度s
    //send_id_file.resize(id_name_text_size + sizeof(size_t));
    ////std::memcpy(send_id_file.data(), &id_name_text_size, sizeof(size_t));
    //sprintf(&send_id_file[0], "%zd", id_name_text_size);

    //sprintf(&send_id_file[sizeof(size_t)], "%s", id_name_text.c_str());

    //char buffer[4096] = { 0 };
    //std::memcpy(buffer, &id_name_text_size, 8);
    //std::memcpy(buffer + 8, id_name_text.data(), id_name_text.size());

    name_text_response resp;
    resp.body_.set_name_text(id_name,id_buf);

    this->async_write(std::move(resp),[this](std::error_code ec, std::size_t sz)
            {
                if (!ec)
                {
                    //ser_ptr_->ui.text_log->insertPlainText(u8"上传id.json文件\n");

                    OutputDebugString(L"id.json 发送成功\n");

                   // recive_id_port();//接收变成服务端的id ip port
                }
            });
    //asio::async_write(socket_, asio::buffer(/*send_id_file.data(), send_id_file.size()*/buffer, 8 + id_name_text_size),
    //    [this](std::error_code ec, std::size_t sz)
    //    {
    //        if (!ec)
    //        {
    //            //ser_ptr_->ui.text_log->insertPlainText(u8"上传id.json文件\n");

    //            cout << id_name << "发送成功!\n";
    //            cout << "内容 >: " << send_id_file << endl;
    //            recive_id_port();//接收变成服务端的id ip port
    //        }
    //    });
}

void upload_json_session::recive_id_port()//接收变成服务端的id port
{
   // id_port_request req;
   // req.parse_bytes(buffer_);
   //// save_ip_port.ip = socket_.local_endpoint().address().to_string();
   // id = req.body_.id_;
   // save_ip_port.port = req.body_.port_;
   // files_id.blocks[id].server.insert(save_ip_port);
   // save_json_file(files_id, id_name);//保存id.json 文件
     
     
    // 
    //socket_.async_read_some(asio::buffer(id_port, 1024),
    //    [this](std::error_code ec, std::size_t)
    //    {
    //        if (!ec)
    //        {
    //            size_t buf_len = 0;
    //            std::memcpy(&buf_len, id_port, sizeof(size_t));//从 id_port复制sizeof(size_t)个字节到存储区 buf_len
    //            std::string id_port_text(id_port + sizeof(size_t));//把内容存到ip_port_text                 

    //            string request(id_port_text);
    //            auto pos = request.find_first_of(',');
    //            if (pos == string::npos)
    //                return;
    //            auto id1 = request.substr(0, pos);
    //            auto port1 = request.substr(pos + 1);
    //            cout << "id1====   " << id1 << "   port1====   " << port1 << endl;
    //            id = atoi(id1.c_str());
    //            save_ip_port.ip = socket_.local_endpoint().address().to_string();
    //            save_ip_port.port = port1;
    //            files_id.blocks[id].server.insert(save_ip_port);
    //            save_json_file(files_id, id_name);//保存id.json 文件
    //            recive_id_port();

    //            /* id_port_request req{};
    //             req.parse_bytes(id_port);*/

    //        }
    //        else {
    //            cout << "ip port : " << save_ip_port.ip << "  " << save_ip_port.port << "断开连接\n";//下载完下线，把ip port删除掉                      
    //            delete_ip_port(save_ip_port);
    //        }
    //    });
}


int upload_json_session::read_handle(uint32_t id)
{
    id_port_request req;
    req.parse_bytes(buffer_);
    // save_ip_port.ip = socket_.local_endpoint().address().to_string();
    id = req.body_.id_;
    save_ip_port.port = req.body_.port_;
    files_id.blocks[id].server.insert(save_ip_port);
    save_json_file(files_id, id_name);//保存id.json 文件


    return  0;
}
void upload_json_session::delete_ip_port(filestruct::ip_and_port delete_ip_port)//删除服务端下线的ip port  
{
    for (auto& iter : files_id.blocks)
    {
        auto tempIter = std::find_if(iter.second.server.begin(), iter.second.server.end(),
            //  auto tempIter = std::find_if(iter.server.begin(), iter.server.end(),
            [&delete_ip_port](const filestruct::ip_and_port& val)//在这个范围内找 ip port
            {
                return val.ip == delete_ip_port.ip && val.port == delete_ip_port.port;
            });
        // if (tempIter == iter.server.end())//没有找到
        if (tempIter == iter.second.server.end())//没有找到
            continue;
        iter.second.server.erase(tempIter);//找到则删除
        //iter.server.erase(tempIter);//找到则删除
    }
    save_json_file(files_id, id_name); //保存id.json         
}

void upload_json_session::save_json_file(filestruct::files_Server files_id, const string& name)  //保存json文件
{
    string text = RapidjsonToString(files_id.serializeToJSON());
    gsh(text);
    auto file = fopen(name.c_str(), "wb");
    const char* t = text.c_str();
    size_t length = text.length();
    fwrite(t, length, 1, file);
    fflush(file);
    fclose(file);
}

void upload_json_session::gsh(std::string& strtxt)//按照格式写入id.json 文件
{
    unsigned int dzkh = 0; //括号的计数器
    bool isy = false; //是不是引号
    for (int i = 0; i < strtxt.length(); ++i) {
        if (isy || strtxt[i] == '"') // "前引号 "后引号
        {
            if (strtxt[i] == '"')
                isy = !isy;
            continue;
        }
        std::string tn = "";

#define ADD_CHANGE                          \
    for (unsigned int j = 0; j < dzkh; ++j) \
        tn += "\t";

        if (strtxt[i] == '{' || strtxt[i] == '[') {
            dzkh++;
            ADD_CHANGE
                strtxt = strtxt.substr(0, i + 1) + "\n" + tn + strtxt.substr(i + 1);
            i += dzkh + 1;
        }
        else if (strtxt[i] == '}' || strtxt[i] == ']') {
            dzkh--;
            ADD_CHANGE
                strtxt = strtxt.substr(0, i) + "\n" + tn + strtxt.substr(i);
            i += dzkh + 1;
        }
        else if (strtxt[i] == ',') {
            ADD_CHANGE
                strtxt = strtxt.substr(0, i + 1) + "\n" + tn + strtxt.substr(i + 1);
            i += dzkh + 1;
        }
    }
}
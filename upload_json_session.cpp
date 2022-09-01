#include "upload_json_session.h"
#include "server_page.h"
#include "ui_server_page.h"
#include "server_page.h"
#include "request.hpp"
#include "response.hpp"



void upload_json_session::delete_all_ip_port()          //在程序开始之前  删除 id.json中的IP port
{

    parse_id_json();
    for (auto& iter : files_id.blocks)
    {
        iter.second.server.erase(iter.second.server.begin(), iter.second.server.end());
    }
    save_json_file(files_id, id_name_);//保存  id.json文件
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
    save_json_file(files_id, id_name_); //保存id.json文件
    do_send_list();
}

void upload_json_session::do_send_list()//发送list.json
{
    std::string list_buf = get_file_context(list_name_);

    name_text_response res;
   
    res.body_.set_name_text(list_name_,list_buf);
    auto self = shared_from_this();
    async_write(std::move(res),
        [this](std::error_code ec, std::size_t sz)
            {
                if (!ec)
                {
                    OutputDebugString(L"list.json 发送成功\n");

                    do_send_id();
                }
            });
}

void upload_json_session::do_send_id()
{

    std::string id_buf = get_file_context(id_name_);//获取文件的内容

    name_text_response resp;
    resp.body_.set_name_text(id_name_,id_buf);

    this->async_write(std::move(resp),[this](std::error_code ec, std::size_t sz)
            {
                if (!ec)
                {
                    OutputDebugString(L"id.json 发送成功\n");
                }
            });
}

int upload_json_session::read_handle(uint32_t id)
{
    switch (id)
    {
    case request_number::id_port_request:

        filestruct::ip_and_port save_ip_port;

        id_port_request req;
        req.parse_bytes(buffer_);

        id = req.body_.id_;
        save_ip_port.port = req.body_.port_;
        files_id.blocks[id].server.insert(save_ip_port);
        save_json_file(files_id, id_name_);

        break;
    }
   

    return  0;
}
void upload_json_session::delete_ip_port(filestruct::ip_and_port delete_ip_port)
{
    for (auto& iter : files_id.blocks)
    {
        auto tempIter = std::find_if(iter.second.server.begin(), iter.second.server.end(),
            [&delete_ip_port](const filestruct::ip_and_port& val)
            {
                return val.ip == delete_ip_port.ip && val.port == delete_ip_port.port;
            });

        if (tempIter == iter.second.server.end())
            continue;

        iter.second.server.erase(tempIter);
    }
    save_json_file(files_id, id_name_);     
}

void upload_json_session::save_json_file(filestruct::files_Server files_id, const std::string& name)
{
    std::string text = RapidjsonToString(files_id.serializeToJSON());
    json_formatting(text);
    auto file = fopen(name.c_str(), "wb");
    const char* t = text.c_str();
    size_t length = text.length();
    fwrite(t, length, 1, file);
    fflush(file);
    fclose(file);
}


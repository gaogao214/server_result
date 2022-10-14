#include "upload_json_session.h"
#include "server_page.h"
#include "ui_server_page.h"
#include "server_page.h"
#include "request.hpp"
#include "response.hpp"



void upload_json_session::delete_all_ip_port()      
{
    parse_id_json();

    for (auto& iter : files_id.blocks)
    {
        iter.second.server.erase(iter.second.server.begin(), iter.second.server.end());
    }

    save_json_file(files_id, id_name_);

    parse_id_json();
}

void upload_json_session::save_file_server_ip_port_id_json()
{
    parse_list_json();

    filestruct::ip_and_port first_ip_port;

    first_ip_port.ip = socket_.local_endpoint().address().to_string();
    first_ip_port.port = profile_.port;

    for (auto& iter : files_id.blocks)
    {
        iter.second.server.insert(first_ip_port);
    }

    save_json_file(files_id, id_name_); 

    for (auto& it : file_server.file_list)
    {
        auto file_name = profile_.path + "\\" + it.path;
        int file_size_ = get_file_len(file_name);
        it.filesize = file_size_;

    }

    save_json_file(file_server,list_name_);

    do_send_list();
}

void upload_json_session::do_send_list()
{
    std::string list_buf = get_file_context(list_name_);
    std::size_t buf_len = get_file_len(list_name_);

    name_text_response resp;

    resp.header_.set_name(list_name_);
    resp.body_.set_name_text(list_buf);
    resp.header_.totoal_length_ = buf_len;

    async_write(std::move(resp),
        [this](std::error_code ec, std::size_t sz)
            {
                if (!ec)
                {
                    do_send_id();
                }
            });
}

void upload_json_session::do_send_id()
{
    std::string id_buf = get_file_context(id_name_);
    std::size_t buf_len = get_file_len(id_name_);

    name_text_response resp;

    resp.header_.set_name(id_name_);
    resp.header_.totoal_length_ = buf_len;
    resp.body_.set_name_text(id_buf);

    this->async_write(std::move(resp),[this](std::error_code ec, std::size_t sz)
            {
                if (!ec)
                {
                    OutputDebugString(L"id.json ·¢ËÍ³É¹¦\n");
                }
            });
}

int upload_json_session::read_handle(uint32_t id)
{
    switch (id)
    {
    case uint32_t(request_number::id_port_request):


        id_port_request req;

        req.parse_bytes(buffer_);

        id = req.body_.id_;
        save_ip_port.ip = socket_.local_endpoint().address().to_string();

        save_ip_port.port = req.body_.port_;

        files_id.blocks[id].server.insert(save_ip_port);

        save_json_file(files_id, id_name_);

        break;
    }
   

    return  0;
}

int upload_json_session::read_error()
{

   delete_ip_port(save_ip_port);
    return 0;
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

template<typename _TSTRUCT>
void upload_json_session::save_json_file(/*filestruct::files_Server*/_TSTRUCT files_id, const std::string& name)
{
    std::string text = RapidjsonToString(files_id.serializeToJSON());
    json_formatting(text);
    
    save_file(name.c_str(),text.c_str());
}

void upload_json_session::save_file(const char* name, const char* file_buf)
{
    volatile int len = 0;

    std::ofstream save_file_(name, std::ios::out | std::ios::binary);

    if (file_buf != nullptr)
        save_file_.write(file_buf, strlen(file_buf) - len);

    save_file_.flush();
    save_file_.close();

}
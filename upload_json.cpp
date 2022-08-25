#include "upload_json.h"
#include "server_page.h"
#include "ui_server_page.h"
#include "server_page.h"

filestruct::profile profile_;

upload_json::upload_json(asio::ip::tcp::socket socket)
    :socket_(std::move(socket))         //����ƶ����캯��ʵ��move��socket�ǹ�����io���״̬�����Ժ���ſ����ظ�ʹ�ö����ٽ���io��ֵ
{
       
}

void upload_json::start()
{
    do_opendir();
    parse_id_json();
    save_file_server_ip_port_id_json();     //��id.json����ӱ��������ļ��е�ip port ����(a.txt...)�ķ�����ip port
}

void  upload_json::do_opendir()          //�������ļ������ҵ������ļ��е�·��,�鿴·���µ��ļ����ļ���
{
    string readbuffer = open_json_file(profile_name);
    profile_.deserializeFromJSON(readbuffer.c_str());
}

void  upload_json::parse_id_json()          //��id_json�ļ� ����id.json�õ�IP port
{
    string readbuffer = open_json_file(id_name);
    files_id.deserializeFromJSON(readbuffer.c_str());
}

std::string upload_json::open_json_file(const std::string& json_name)//��ָ�����Ƶ�json�ı�
{
    std::string content{};
    std::string tmp{};
    std::fill(content.begin(), content.end(), 0);       //���
    fstream ifs(json_name, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
        return {};
    while (std::getline(ifs, tmp))
    {
        content.append(tmp);
    }
    return content;
}

void upload_json::delete_all_ip_port()          //�ڳ���ʼ֮ǰ  ɾ�� id.json�е�IP port
{

    parse_id_json();
    for (auto& iter : files_id.blocks)
    {
        //  iter.server.erase(iter.server.begin(), iter.server.end());
        iter.second.server.erase(iter.second.server.begin(), iter.second.server.end());
    }
    save_json_file(files_id, id_name);//����  id.json�ļ�
    parse_id_json();
}

void upload_json::save_file_server_ip_port_id_json()//��id.json����Ӵ���(a.txt...)�ķ�����ip port
{
    
    filestruct::ip_and_port first_ip_port;
    first_ip_port.ip = socket_.local_endpoint().address().to_string();//�����������ļ���ip
    first_ip_port.port = profile_.port;//�����������ļ���port
    for (auto& iter : files_id.blocks)
    {
        // iter.server.insert(first_ip_port);//Ӧ����һ������
        iter.second.server.insert(first_ip_port);//Ӧ����һ������
    }
    save_json_file(files_id, id_name); //����id.json�ļ�
    do_send_list();
}

int upload_json::get_file_len(const std::string& filename)//��ȡ�ı��ĳ���
{
    ifstream infile(filename.c_str());
    infile.seekg(0, ios_base::end);
    int fsize = infile.tellg();//list.json�ı��Ĵ�С
    infile.close();
    return fsize;
}

std::string upload_json::get_file_context(const std::string& filename)//��ȡ�ı�������
{

    ifstream File(filename.c_str());
    char file_buf = '0';//list.json�ļ�
    string buf;//һ��һ����֮��������list.json�ı�
    while (File.get(file_buf))
    {
        buf.push_back(file_buf);
    }
    File.close();
    return buf;
}

void upload_json::do_send_list()//����list.json
{
    std::string list_buf = get_file_context(list_name);
    string list_parse = "0"+list_name + "*" + list_buf;
    size_t list_name_text_size = list_parse.size();
    list_name_text.resize(list_name_text_size + sizeof(size_t));
    //std::memcpy(list_name_text.data(), &list_name_text_size, sizeof(size_t));
    sprintf(&list_name_text[0], "%zd", list_name_text_size);

    sprintf(&list_name_text[sizeof(size_t)], "%s", list_parse.c_str());

    char buffer[4096] = { 0 };
    std::memcpy(buffer, &list_name_text_size, sizeof(std::size_t));
    std::memcpy(buffer + 8, list_parse.data(), list_parse.size());

    asio::async_write(socket_, asio::buffer(/*list_name_text.data(), list_name_text.size()*/buffer, 8+ list_name_text_size),
        [this](std::error_code ec, std::size_t sz)
        {
            if (!ec)
            {
                //ser_ptr_->ui.text_log->insertPlainText(u8"�ϴ�list.json�ļ�\n");
               // ser_ptr_->ui.text_log->append(u8"�ϴ�list.hson�ļ�");
                OutputDebugString(L"����list.json");

                cout << list_name << "���ͳɹ�\n";
                do_send_id();//����id.json����
            }
        });
}

void upload_json::do_send_id()//����id.json���ֺ�����
{

    std::string id_buf = get_file_context(id_name);//��ȡ�ļ�������
    std::string id_name_text ="1"+id_name + "*" + id_buf; //�����ֺ�����ƴ����һ��
    size_t id_name_text_size = id_name_text.size();     //�����ƴ�Ӻ���ַ�������
    send_id_file.resize(id_name_text_size + sizeof(size_t));
    //std::memcpy(send_id_file.data(), &id_name_text_size, sizeof(size_t));
    sprintf(&send_id_file[0], "%zd", id_name_text_size);

    sprintf(&send_id_file[sizeof(size_t)], "%s", id_name_text.c_str());

    char buffer[4096] = { 0 };
    std::memcpy(buffer, &id_name_text_size, 8);
    std::memcpy(buffer + 8, id_name_text.data(), id_name_text.size());


    asio::async_write(socket_, asio::buffer(/*send_id_file.data(), send_id_file.size()*/buffer,8+ id_name_text_size),
        [this](std::error_code ec, std::size_t sz)
        {
            if (!ec)
            {
                //ser_ptr_->ui.text_log->insertPlainText(u8"�ϴ�id.json�ļ�\n");

                cout << id_name << "���ͳɹ�!\n";
                cout << "���� >: " << send_id_file << endl;
               recive_id_port();//���ձ�ɷ���˵�id ip port
            }
        });
}

void upload_json::recive_id_port()//���ձ�ɷ���˵�id port
{
    socket_.async_read_some(asio::buffer(id_port, 1024),
        [this](std::error_code ec, std::size_t)
        {
            if (!ec)
            {
                size_t buf_len = 0;
                std::memcpy(&buf_len, id_port, sizeof(size_t));//�� id_port����sizeof(size_t)���ֽڵ��洢�� buf_len
                std::string id_port_text(id_port + sizeof(size_t));//�����ݴ浽ip_port_text                 

                string request(id_port_text);
                auto pos = request.find_first_of(',');
                if (pos == string::npos)
                    return;
                auto id1 = request.substr(0, pos);
                auto port1 = request.substr(pos + 1);
                cout << "id1====   " << id1 << "   port1====   " << port1 << endl;
                id = atoi(id1.c_str());
                save_ip_port.ip = socket_.local_endpoint().address().to_string();
                save_ip_port.port = port1;
                files_id.blocks[id].server.insert(save_ip_port);
                save_json_file(files_id, id_name);//����id.json �ļ�
                recive_id_port();
            }
            else {
                cout << "ip port : " << save_ip_port.ip << "  " << save_ip_port.port << "�Ͽ�����\n";//���������ߣ���ip portɾ����                      
                delete_ip_port(save_ip_port);
            }
        });
}

void upload_json::delete_ip_port(filestruct::ip_and_port delete_ip_port)//ɾ����������ߵ�ip port  
{
    for (auto& iter : files_id.blocks)
    {
        auto tempIter = std::find_if(iter.second.server.begin(), iter.second.server.end(),
            //  auto tempIter = std::find_if(iter.server.begin(), iter.server.end(),
            [&delete_ip_port](const filestruct::ip_and_port& val)//�������Χ���� ip port
            {
                return val.ip == delete_ip_port.ip && val.port == delete_ip_port.port;
            });
        // if (tempIter == iter.server.end())//û���ҵ�
        if (tempIter == iter.second.server.end())//û���ҵ�
            continue;
        iter.second.server.erase(tempIter);//�ҵ���ɾ��
        //iter.server.erase(tempIter);//�ҵ���ɾ��
    }
    save_json_file(files_id, id_name); //����id.json         
}

void upload_json::save_json_file(filestruct::files_Server files_id, const string& name)  //����json�ļ�
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

void upload_json::gsh(std::string& strtxt)//���ո�ʽд��id.json �ļ�
{
    unsigned int dzkh = 0; //���ŵļ�����
    bool isy = false; //�ǲ�������
    for (int i = 0; i < strtxt.length(); ++i) {
        if (isy || strtxt[i] == '"') // "ǰ���� "������
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
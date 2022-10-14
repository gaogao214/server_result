#pragma once

#include "asio.hpp"
#include "basic_server.h"
#include "file_struct.h"
#include "basic_session.hpp"

extern filestruct::profile profile_;

class server_page;

class upload_json_session
	: public basic_session
	, public std::enable_shared_from_this<upload_json_session>
{
	Q_OBJECT
public:

	upload_json_session(asio::ip::tcp::socket socket)
		:basic_session(std::move(socket))
	{
		delete_all_ip_port();

		do_opendir();

		parse_id_json();

		save_file_server_ip_port_id_json();
	}
	
	template <typename _TSTRUCT>
	void save_json_file(/*filestruct::files_Server*/_TSTRUCT files_id, const std::string& name);

	void delete_all_ip_port();

protected:
	virtual int read_handle(uint32_t id)  override;
	virtual int read_error() override;

private:

	void do_send_list();

	void do_send_id();

	void delete_ip_port(filestruct::ip_and_port delete_ip_port);

	void save_file_server_ip_port_id_json();

	void save_file(const char* name, const char* file_buf);


private:
	filestruct::ip_and_port save_ip_port;

};


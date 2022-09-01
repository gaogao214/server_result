#pragma once
#include "asio.hpp"
#include "basic_session.hpp"

using namespace std;
using namespace asio::ip;

class upload_file_session :public basic_session
{
public:
	upload_file_session(tcp::socket socket)
		:basic_session(std::move(socket))
	{
	
	}

private:
	void do_send_file(char id, const string& filename);

protected:
	virtual int read_handle(uint32_t id)  override;
};

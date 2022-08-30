#pragma once
#include <array>
#include "asio.hpp"
#include <QObject>
#include <sstream>



class basic_server :public QObject
{
	//Q_OBJECT
public:
	explicit basic_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint/*,server_page* sep_*/)
		:io_context_(io_context)
		, acceptor_(io_context, endpoint)
		, socket_(io_context)
	{
		do_accept();
	}

public:
	template<std::size_t I, typename _Handle>
	void async_write(std::array<char, I> buffer, _Handle&& handle)
	{
		asio::async_write(socket_, asio::buffer(buffer), std::forward<_Handle>(handle));
	}

	template<typename _Handle>
	void async_write(const std::string& buffer, _Handle&& handle)
	{
		asio::async_write(socket_, asio::buffer(buffer), std::forward<_Handle>(handle));
	}


	void close()
	{
		socket_.close();
	}

protected:
	virtual int read_handle() = 0;


private:
	void do_read_header()
	{

		asio::async_read(socket_, asio::buffer(buffer_, sizeof(size_t)),
			[this](std::error_code ec, std::size_t)
			{
				if (ec)
				{
					return;
				}

				std::size_t receive_length{};

				std::memcpy(&receive_length, &buffer_, sizeof(std::size_t));


				do_read_body(receive_length);

			});

	}

	void do_read_body(std::size_t length)
	{

		asio::async_read(socket_, asio::buffer(buffer_, length),
			[this](std::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					read_handle();

					do_read_header();
				}

				buffer_.fill(0);

				do_read_header();
			});
	}

	void do_accept()
	{
		acceptor_.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					do_read_header();
				}
					

				//emit signal_connect();
				do_accept();
			});
	}


protected:
	std::array<char, 8192 + 1024> buffer_;


private:
	asio::ip::tcp::socket socket_;
	asio::ip::tcp::acceptor acceptor_;
	asio::io_context& io_context_;

};


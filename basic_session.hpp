#pragma once
#include "asio.hpp"
#include <QObject>

static constexpr std::size_t size_ = 8192 + 1024 + sizeof(uint32_t);

class basic_session: public QObject
{
public:
	basic_session(asio::ip::tcp::socket socket)
		: socket_(std::move(socket))
	{
		
	}

public:
	void start()
	{
		do_read_header();
	}

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

	template<typename _Response, typename _Handle>
	void async_write(_Response&& resp, _Handle&& handle)
	{
		constexpr auto id = _Response::Number;

		std::array<char, size_> arr{};

		std::memcpy(arr.data(), &id, sizeof(uint32_t));

		resp.to_bytes(arr.data() + sizeof(uint32_t));

		async_write(arr, std::forward<_Handle>(handle));

		arr.fill(0);
	}

	template<typename _Response>
	void async_write(_Response&& resp)
	{
		async_write(std::move(resp), [](std::error_code ec, std::size_t sz) {});
	}


protected:
	virtual int read_handle(uint32_t) = 0;

	virtual int read_error() = 0 ;

private:
	void do_read_header()
	{
		asio::async_read(socket_, asio::buffer(buffer_, sizeof(uint32_t)),
			[this](std::error_code ec, std::size_t)
			{
				if (ec)
				{
					OutputDebugStringA(ec.message().data());

					read_error();

					return;
				}
				std:memcpy(&proto_id, buffer_.data(), sizeof(uint32_t));

				do_read_body(proto_id);
			});
	}

	void do_read_body(uint32_t id)
	{
		asio::async_read(socket_, asio::buffer(buffer_, size_-sizeof(uint32_t)),
			[&,this](std::error_code ec, std::size_t bytes_transferred)
			{
				if (ec)
					return;
				read_handle(id);

				buffer_.fill(0);

				do_read_header();
			});
	}


protected:
	std::array<char, size_> buffer_;

	asio::ip::tcp::socket socket_;
	
	uint32_t proto_id{};
};
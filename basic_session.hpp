#pragma once
#include "asio.hpp"

class basic_session
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

		std::array<char, 1024> arr{};
		std::memcpy(arr.data(), &id, sizeof(uint32_t));

		resp.to_bytes(arr.data() + sizeof(uint32_t));

		async_write(arr, std::forward<_Handle>(handle));
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

private:
	asio::ip::tcp::socket socket_;
protected:
	std::array<char, 4096> buffer_;
};
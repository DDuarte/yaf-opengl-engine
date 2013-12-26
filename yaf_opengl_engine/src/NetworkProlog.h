#ifndef NETWORKPROLOG_H
#define NETWORKPROLOG_H

#include <iostream>
#include <string>
#include <asio.hpp>
#include <thread>
#include <array>

#include "ConcurrentQueue.h"

#define MESSAGE_MAX_SIZE 256

using asio::ip::tcp;

class PrologPredicate
{
public:
    template<typename... Args>
    static std::string Build(const std::string& name, const Args&... args)
    {
        std::string str = name;
        std::array<std::string, sizeof...(args)> list = { stringify(args)... };
        if (sizeof...(args) > 0)
        {
            str += "(";
            for (auto i = 0u; i < sizeof...(args) - 1; ++i)
                str += list[i] + ", ";
            str += list[sizeof...(args) - 1] + ")";
        }

        str += ".\n";
        return str;
    }

private:
    template<typename T>
    static std::string stringify(const T& val) { return std::to_string(val); }

    template<> // std::string overload
    static std::string stringify<>(const std::string& val) { return val; }

    template<> // const char* overload
    static std::string stringify<>(const char* const& val) { return std::string(val); }
};

class NetworkProlog
{
public:
    NetworkProlog() : _ioService(), _socket(_ioService) { }

    bool Startup()
    {
        try
        {
            tcp::resolver resolver(_ioService);
            tcp::resolver::query query("127.0.0.1", "5555");
            auto endpointIterator = resolver.resolve(query);

            asio::connect(_socket, endpointIterator);
        }
        catch (asio::system_error& se)
        {
            std::cerr << "Unable to connect: " << se.what() << std::endl;
            return false;
        }

        _sendThread = std::thread(&NetworkProlog::SendMessages, this);
        _receiveThread = std::thread(&NetworkProlog::ReceiveMessages, this);

        // _sendThread.detach();
        // _receiveThread.detach();

        return true;
    }

    void EnqueueMessage(const std::string& msg)
    {
        _sendQueue.push(msg);
    }

    std::string GetMessage()
    {
        return _receiveQueue.pop();
    }

    bool IsReceiveEmpty() const
    {
        return _receiveQueue.empty();
    }

    void CloseConnection()
    {
        _socket.close();
    }

private:
    ConcurrentQueue<std::string> _sendQueue;
    ConcurrentQueue<std::string> _receiveQueue;
    asio::io_service _ioService;
    tcp::socket _socket;
    std::thread _sendThread;
    std::thread _receiveThread;

    void ReceiveMessages()
    {
        while (_socket.is_open())
        {
            std::string msg;
            asio::error_code error;

            std::array<char, MESSAGE_MAX_SIZE> buf;

            auto len = _socket.read_some(asio::buffer(buf), error);
            if (error == asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error);

            msg = from_array<char, MESSAGE_MAX_SIZE, std::string>(buf, len);

            _receiveQueue.push(msg);
            std::cout << "NETDEBUG: received: " << msg << std::endl;
        }

        std::cout << "Exited loop of NetworkProlog::ReceiveMessages()" << std::endl;
    }

    void SendMessages()
    {
        while (_socket.is_open())
        {
            std::string msg;
            asio::error_code error;
            _sendQueue.pop(msg);

            std::array<char, MESSAGE_MAX_SIZE> buf = to_array<char, MESSAGE_MAX_SIZE, std::string>(msg);

            auto len = _socket.write_some(asio::buffer(buf), error);
            if (error == asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error);

            std::cout << "NETDEBUG: sent: " << msg << std::endl;
        }

        std::cout << "Exited loop of NetworkProlog::SendMessages()" << std::endl;
    }

    template<class T, size_t N, class V>
    static V from_array(const std::array<T, N>& arr, size_t count)
    {
        V v;
        std::copy_n(arr.begin(), count, std::back_inserter(v));
        return v;
    }

    template<class T, size_t N, class V>
    static std::array<T, N> to_array(const V& v)
    {
        std::array<T, N> d;
        d.fill(static_cast<T>(0));
        std::copy_n(v.begin(), v.size() > N ? N : v.size(), d.data());
        return d;
    }
};

#endif // NETWORKPROLOG_H

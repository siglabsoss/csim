/*
    Copyright (c) 2016 VOCA AS / Harald N?kland

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#ifndef __ZMQ_ADDON_HPP_INCLUDED__
#define __ZMQ_ADDON_HPP_INCLUDED__

#include <zmq.hpp>
#include <deque>
#include <iomanip>
#include <sstream>

namespace zmq {

#ifdef ZMQ_HAS_RVALUE_REFS

/*
    This class handles multipart messaging. It is the C++ equivalent of zmsg.h,
    which is part of CZMQ (the high-level C binding). Furthermore, it is a major
    improvement compared to zmsg.hpp, which is part of the examples in the ?MQ
    Guide. Unnecessary copying is avoided by using move semantics to efficiently
    add/remove parts.
*/
class multipart_t
{
private:
    std::deque<message_t> m_parts;

public:
    multipart_t()
    {}

    multipart_t(socket_t& socket)
    {
        recv(socket);
    }

    multipart_t(const void *src, size_t size)
    {
        addmem(src, size);
    }

    multipart_t(const std::string& string)
    {
        addstr(string);
    }

    multipart_t(message_t&& message)
    {
        add(std::move(message));
    }

    multipart_t(multipart_t&& other)
    {
        std::swap(m_parts, other.m_parts);
    }

    multipart_t& operator=(multipart_t&& other)
    {
        std::swap(m_parts, other.m_parts);
        return *this;
    }

    virtual ~multipart_t()
    {
        clear();
    }

    void clear()
    {
        m_parts.clear();
    }

    size_t size() const
    {
        return m_parts.size();
    }

    bool empty() const
    {
        return m_parts.empty();
    }

    bool recv(socket_t& socket)
    {
        clear();
        bool more = true;
        while (more)
        {
            message_t message;
            if (!socket.recv(&message))
                return false;
            more = message.more();
            add(std::move(message));
        }
        return true;
    }

    bool send(socket_t& socket)
    {
        bool more = size() > 0;
        while (more)
        {
            message_t message = pop();
            more = size() > 0;
            if (!socket.send(message, more ? ZMQ_SNDMORE : 0))
                return false;
        }
        clear();
        return true;
    }

    void prepend(multipart_t&& other)
    {
        while (!other.empty())
            push(other.remove());
    }

    void append(multipart_t&& other)
    {
        while (!other.empty())
            add(other.pop());
    }

    void pushmem(const void *src, size_t size)
    {
        m_parts.push_front(message_t(src, size));
    }

    void addmem(const void *src, size_t size)
    {
        m_parts.push_back(message_t(src, size));
    }

    void pushstr(const std::string& string)
    {
        m_parts.push_front(message_t(string.data(), string.size()));
    }

    void addstr(const std::string& string)
    {
        m_parts.push_back(message_t(string.data(), string.size()));
    }

    template<typename T>
    void pushtyp(const T& type)
    {
        m_parts.push_front(message_t(&type, sizeof(type)));
    }

    template<typename T>
    void addtyp(const T& type)
    {
        m_parts.push_back(message_t(&type, sizeof(type)));
    }

    void push(message_t&& message)
    {
        m_parts.push_front(std::move(message));
    }

    void add(message_t&& message)
    {
        m_parts.push_back(std::move(message));
    }

    std::string popstr()
    {
        if (m_parts.empty())
            return "";
        std::string string(m_parts.front().data<char>(), m_parts.front().size());
        m_parts.pop_front();
        return string;
    }

    template<typename T>
    T poptyp()
    {
        if (m_parts.empty())
            return T();
        T type = *m_parts.front().data<T>();
        m_parts.pop_front();
        return type;
    }

    message_t pop()
    {
        if (m_parts.empty())
            return message_t(0);
        message_t message = std::move(m_parts.front());
        m_parts.pop_front();
        return message;
    }

    message_t remove()
    {
        if (m_parts.empty())
            return message_t(0);
        message_t message = std::move(m_parts.back());
        m_parts.pop_back();
        return message;
    }

    const message_t* peek(size_t index) const
    {
        if (index >= size())
            return nullptr;
        return &m_parts[index];
    }

    template<typename T>
    static multipart_t create(const T& type)
    {
        multipart_t multipart;
        multipart.addtyp(type);
        return multipart;
    }

    multipart_t clone() const
    {
        multipart_t multipart;
        for (size_t i = 0; i < size(); i++)
            multipart.addmem(m_parts[i].data(), m_parts[i].size());
        return multipart;
    }

    std::string str() const
    {
        std::stringstream ss;
        for (size_t i = 0; i < m_parts.size(); i++)
        {
            const unsigned char* data = m_parts[i].data<unsigned char>();
            size_t size = m_parts[i].size();

            // Dump the message as text or binary
            bool isText = true;
            for (size_t j = 0; j < size; j++)
            {
                if (data[j] < 32 || data[j] > 127)
                {
                    isText = false;
                    break;
                }
            }
            ss << "\n[" << std::dec << std::setw(3) << std::setfill('0') << size << "] ";
            if (size >= 1000)
            {
                ss << "... (to big to print)";
                continue;
            }
            for (size_t j = 0; j < size; j++)
            {
                if (isText)
                    ss << static_cast<char>(data[j]);
                else
                    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<short>(data[j]);
            }
        }
        return ss.str();
    }

    bool equal(const multipart_t* other) const
    {
        if (size() != other->size())
            return false;
        for (size_t i = 0; i < size(); i++)
            if (!peek(i)->equal(other->peek(i)))
                return false;
        return true;
    }

    static int test()
    {
        bool ok = true;
        float num = 0;
        std::string str = "";
        message_t msg;

        // Create two PAIR sockets and connect over inproc
        context_t context(1);
        socket_t output(context, ZMQ_PAIR);
        socket_t input(context, ZMQ_PAIR);
        output.bind("inproc://multipart.test");
        input.connect("inproc://multipart.test");

        // Test send and receive of single-frame message
        multipart_t multipart;
        assert(multipart.empty());

        multipart.push(message_t("Hello", 5));
        assert(multipart.size() == 1);

        ok = multipart.send(output);
        assert(multipart.empty());
        assert(ok);

        ok = multipart.recv(input);
        assert(multipart.size() == 1);
        assert(ok);

        msg = multipart.pop();
        assert(multipart.empty());
        assert(std::string(msg.data<char>(), msg.size()) == "Hello");

        // Test send and receive of multi-frame message
        multipart.addstr("A");
        multipart.addstr("BB");
        multipart.addstr("CCC");
        assert(multipart.size() == 3);

        multipart_t copy = multipart.clone();
        assert(copy.size() == 3);

        ok = copy.send(output);
        assert(copy.empty());
        assert(ok);

        ok = copy.recv(input);
        assert(copy.size() == 3);
        assert(ok);
        assert(copy.equal(&multipart));

        multipart.clear();
        assert(multipart.empty());

        // Test message frame manipulation
        multipart.add(message_t("Frame5", 6));
        multipart.addstr("Frame6");
        multipart.addstr("Frame7");
        multipart.addtyp(8.0f);
        multipart.addmem("Frame9", 6);
        multipart.push(message_t("Frame4", 6));
        multipart.pushstr("Frame3");
        multipart.pushstr("Frame2");
        multipart.pushtyp(1.0f);
        multipart.pushmem("Frame0", 6);
        assert(multipart.size() == 10);

        msg = multipart.remove();
        assert(multipart.size() == 9);
        assert(std::string(msg.data<char>(), msg.size()) == "Frame9");

        msg = multipart.pop();
        assert(multipart.size() == 8);
        assert(std::string(msg.data<char>(), msg.size()) == "Frame0");

        num = multipart.poptyp<float>();
        assert(multipart.size() == 7);
        assert(num == 1.0f);

        str = multipart.popstr();
        assert(multipart.size() == 6);
        assert(str == "Frame2");

        str = multipart.popstr();
        assert(multipart.size() == 5);
        assert(str == "Frame3");

        str = multipart.popstr();
        assert(multipart.size() == 4);
        assert(str == "Frame4");

        str = multipart.popstr();
        assert(multipart.size() == 3);
        assert(str == "Frame5");

        str = multipart.popstr();
        assert(multipart.size() == 2);
        assert(str == "Frame6");

        str = multipart.popstr();
        assert(multipart.size() == 1);
        assert(str == "Frame7");

        num = multipart.poptyp<float>();
        assert(multipart.empty());
        assert(num == 8.0f);

        // Test message constructors and concatenation
        multipart_t head("One", 3);
        head.addstr("Two");
        assert(head.size() == 2);

        multipart_t tail("One-hundred");
        tail.pushstr("Ninety-nine");
        assert(tail.size() == 2);

        multipart_t tmp(message_t("Fifty", 5));
        assert(tmp.size() == 1);

        multipart_t mid = multipart_t::create(49.0f);
        mid.append(std::move(tmp));
        assert(mid.size() == 2);
        assert(tmp.empty());

        multipart_t merged(std::move(mid));
        merged.prepend(std::move(head));
        merged.append(std::move(tail));
        assert(merged.size() == 6);
        assert(head.empty());
        assert(tail.empty());

        ok = merged.send(output);
        assert(merged.empty());
        assert(ok);

        multipart_t received(input);
        assert(received.size() == 6);

        str = received.popstr();
        assert(received.size() == 5);
        assert(str == "One");

        str = received.popstr();
        assert(received.size() == 4);
        assert(str == "Two");

        num = received.poptyp<float>();
        assert(received.size() == 3);
        assert(num == 49.0f);

        str = received.popstr();
        assert(received.size() == 2);
        assert(str == "Fifty");

        str = received.popstr();
        assert(received.size() == 1);
        assert(str == "Ninety-nine");

        str = received.popstr();
        assert(received.empty());
        assert(str == "One-hundred");

        return 0;
    }

private:
    // Disable implicit copying (moving is more efficient)
    multipart_t(const multipart_t& other) ZMQ_DELETED_FUNCTION;
    void operator=(const multipart_t& other) ZMQ_DELETED_FUNCTION;
};

#endif

}

#endif

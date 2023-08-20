

#include <iostream>
#include "./Tools/httplib.h"
#include "./Tools/Log.hpp"
#include "./proto/add_contact.pb.h"
#include "./Tools/ContactsException.h"
#include "./Tools/utils.hpp"

#define HTTPLIB_LEARN 0

void printContact(add_contact::AddContactRequest &req)
{
    std::cout << "name: " << req.name() << std::endl;
    std::cout << "age: " << req.age() << std::endl;
    for (int j = 0; j < req.phone_size(); j++)
    {
        const add_contact::AddContactRequest_Phone &phone = req.phone(j);
        std::cout << "phone number [" << j + 1 << "]: " << phone.number()
                  << " (" << phone.PhoneType_Name(phone.type()) << ")" << std::endl;
    }
}

void call_back_in_Post(const httplib::Request &req, httplib::Response &res)
{
    // 反序列化 request，注意：这个request不是req，而是req.body
    // req是httplib的封装，实际的请求在req.body中
    add_contact::AddContactRequest request;
    add_contact::AddContactRespones respones;
    try
    {
        if (!request.ParseFromString(req.body))
        {
            throw ContactsException("AddContactRequest Deserialize fail");
        }
        // 新增联系人，持久化存储通讯录
        // 在这个项目中就直接在服务端打印出来就算了，后续可以完善存储到文件中，然后完善其他选项，不只是新增
        printContact(request);
        // 构造 respone，同样，和参数的res也不是同一个东西，即res.body
        respones.set_success(true);
        respones.set_uid(Utils::generate_hex(10));
        // res.body的设置（即序列化respones，放到res.body中去）
        std::string respones_str;
        if (!respones.SerializeToString(&respones_str))
        {
            throw ContactsException("AddContactRequest Serialize fail");
        }
        // 序列化成功
        res.status = 200; // 成功
        res.body = respones_str;
        res.set_header("Content-Type", "application/protobuf");
    }
    catch (const ContactsException &e)
    {
        res.status = 500; // 表示服务端错误
        respones.set_success(false);
        respones.set_error_desc(e.what());
        std::string respones_str;
        if (respones.SerializeToString(&respones_str))
        {
            res.body = respones_str;
            res.set_header("Content-Type", "application/protobuf");
        }
        std::string err_reason_str = "/contacts/add error: " + e.what();
        LogMessage(MESSAGE_MODE::ERROR, err_reason_str);
    }
}

void start_server()
{
    LogMessage(MESSAGE_MODE::NORMAL, "server start");
    httplib::Server server;
    server.Post("/contacts/add", [](const httplib::Request &req, httplib::Response &res)
                {
                    LogMessage(MESSAGE_MODE::NORMAL, "receive a post request");
                    call_back_in_Post(req, res); });
    server.listen("0.0.0.0", 8080);
}

int main()
{
    start_server();
    return 0;
}

#if HTTPLIB_LEARN
void start()
{
    std::cout << " ---------------- server start ---------------- " << std::endl;
    httplib::Server server;
    server.Post("/test-post", [](const httplib::Request &req, httplib::Response &res)
                {
        std::cout << "receive post request!" << std::endl;
        res.status = 200; });

    server.Get("/test-get", [](const httplib::Request &req, httplib::Response &res)
               {
        std::cout << "receive get request!" << std::endl;
        res.status = 200; });

    // 绑定 8080 端口，并且将端口号对外开放
    server.listen("0.0.0.0", 8080);
}

int main()
{
    start();
    return 0;
}
#endif
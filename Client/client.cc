


#include <iostream>
#include "./Tools/httplib.h"
#include "./Tools/ContactsException.h"
#include "./Tools/Log.hpp"
#include "./proto/add_contact.pb.h"

#define CONTACTS_HOST "127.0.0.1"
#define CONTACTS_PORT 8080

#define HTTPLIB_LEARN 0

enum client_option
{
    QUIT = 0,
    CONTACT_ADD = 1,
    CONTACT_DEL = 2,
    CONTACT_FIND_ALL = 3,
    CONTACT_FIND = 4
};

void menu()
{
    std::cout << " ------------------ MENU ------------------- " << std::endl;
    std::cout << " -              1. add contact             - " << std::endl;
    std::cout << " -              2. del contact             - " << std::endl;
    std::cout << " -              3. check list              - " << std::endl;
    std::cout << " -              4. check detail info       - " << std::endl;
    std::cout << " -              0. exit                    - " << std::endl;
    std::cout << " ------------------------------------------- " << std::endl;
}

void AddName(add_contact::AddContactRequest *req)
{
    std::cout << "please input the name: ";
    std::string name;
    std::getline(std::cin, name);
    req->set_name(name);
}
void AddAge(add_contact::AddContactRequest *req)
{
    std::cout << "please input the age: ";
    int age;
    std::cin >> age;
    req->set_age(age);
    std::cin.ignore(256, '\n');
}
void AddPhoneNumber(add_contact::AddContactRequest *req)
{
    // 输入电话号码
    for (int i = 0;; i++)
    {
        std::cout << "please input the [" << i + 1 << "] phone number(type the enter only to finish the input): ";
        std::string number;
        std::getline(std::cin, number);
        if (number.empty()) // 如果联系人只输入了一个 \n 就直接break
            break;

        add_contact::AddContactRequest_Phone *phone = req->add_phone();
        phone->set_number(number);

        std::cout << "please input the phone type: [1. MobilePhone 2. Telephone]: ";
        int type = 0;
        std::cin >> type;
        std::cin.ignore(256, '\n');
        switch (type)
        {
        case 1:
            phone->set_type(add_contact::AddContactRequest_Phone_PhoneType::AddContactRequest_Phone_PhoneType_MP);
            break;
        case 2:
            phone->set_type(add_contact::AddContactRequest_Phone_PhoneType::AddContactRequest_Phone_PhoneType_TEL);
            break;
        default:
            std::cout << "error, break" << std::endl;
            break;
        }
    }
}

void buildAddContactRequest(add_contact::AddContactRequest *req)
{
    std::cout << "------------------ add people info ------------------" << std::endl;
    AddName(req);
    AddAge(req);
    AddPhoneNumber(req);
    std::cout << "-------------- add people info success --------------" << std::endl;
}

void addContact()
{
    // 构造一个客户端
    httplib::Client cli(CONTACTS_HOST, CONTACTS_PORT);
    // 构造一个req
    add_contact::AddContactRequest req;
    buildAddContactRequest(&req);
    // 序列化 req
    std::string req_str;
    bool serialize_res = req.SerializeToString(&req_str);
    if (!serialize_res)
    {
        throw ContactsException("addContact(): SerializeToString failed!");
    }
    // 发起post调用
    auto res = cli.Post("/contacts/add", req_str, "application/protobuf");
    /* res是一个httplib:Result类型 */
    if (!res)
    {
        std::string err_desc;
        err_desc.append("addContact(): /contacts/add post link error! ").append(httplib::to_string(res.error()));
        /* httplib::to_string 可以将错误的枚举专程字符串 */
        /* err_desc 是一个错误信息的字符串 */
        throw ContactsException(err_desc);
    }
    // 反序列化 resp
    add_contact::AddContactRespones resp;
    bool parse_res = resp.ParseFromString(res->body);
    if (res->status != 200 && !parse_res)
    {
        // 因为我们想打印一下反序列化之后的信息
        // 两个前提：反序列化成功+网络调用是成功的（网络状态码是200）
        std::string err_desc;
        err_desc.append("addContact(): /contacts/add post failed")
            .append(std::to_string(res->status))
            .append("[")
            .append(res->reason)
            .append("]");
        throw ContactsException(err_desc);
    }
    else if (res->status != 200)
    {
        // 在这里表示，反序列化是成功的但是状态码不是200
        std::string err_desc;
        err_desc.append("addContact(): /contacts/add post failed")
            .append(std::to_string(res->status))
            .append("[")
            .append(res->reason)
            .append("]")
            .append("\n  error reason: ")
            .append(resp.error_desc());
        throw ContactsException(err_desc);
    }
    else if (!resp.success()) // 我们设置的码不是1，也要错误提示
    {
        std::string err_desc;
        err_desc.append("addContact(): /contacts/add resp error")
            .append("error reason: ")
            .append(resp.error_desc());
        throw ContactsException(err_desc);
    }
    // 结果的打印
    LogMessage(MESSAGE_MODE::NORMAL, "add success");
    std::cout << "add contact success, uid: " << resp.uid() << std::endl;
}

void start_client()
{
    while (true)
    {
        menu();
        std::cout << "please choose:> ";
        ssize_t choose = 0;
        std::cin >> choose;
        std::cin.ignore(256, '\n');
        try
        {
            switch (choose)
            {
            case client_option::QUIT:
                LogMessage(MESSAGE_MODE::NORMAL, "end the client");
                return;
                break;
            case client_option::CONTACT_ADD: // 在这个项目中不做处理
                addContact();
                break;
            case client_option::CONTACT_DEL:
                break;
            case client_option::CONTACT_FIND_ALL:
                break;
            case client_option::CONTACT_FIND:
                break;
            default:
                LogMessage(MESSAGE_MODE::ERROR, "select error");
                break;
            }
        }
        catch (const ContactsException &e)
        {
            LogMessage(MESSAGE_MODE::ERROR, e.what());
        }
        std::cout << std::endl;
    }
}

int main()
{
    start_client();
    return 0;
}

#if HTTPLIB_LEARN
void start()
{
    httplib::Client cli(CONTACTS_HOST, CONTACTS_POST);

    httplib::Result res1 = cli.Post("/test-post");
    if (res1->status == 200)
    {
        std::cout << "call post successed!" << std::endl;
    }

    httplib::Result res2 = cli.Get("/test-get");
    if (res2->status == 200)
    {
        std::cout << "call get successed!" << std::endl;
    }
}

int main()
{
    start();
    return 0;
}
#endif
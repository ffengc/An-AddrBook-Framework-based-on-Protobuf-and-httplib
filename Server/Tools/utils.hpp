#include <sstream>
#include <random>
#include <google/protobuf/map.h>

class Utils
{
public:
    static unsigned int random_char() {
        // 用于随机数引擎获得随机种子
        std::random_device rd;       
        // mt19937是c++11新特性，它是一种随机数算法，用法与rand()函数类似，但是mt19937具有速度快，周期长的特点
        // 作用是生成伪随机数
        std::mt19937 gen(rd()); 
        // 随机生成一个整数i 范围[0, 255]
        std::uniform_int_distribution<> dis(0, 255);
        return dis(gen);
    }

    // 生成 UUID （通用唯一标识符）
    static std::string generate_hex(const unsigned int len) {
        std::stringstream ss;
        // 生成 len 个16进制随机数，将其拼接而成
        for (auto i = 0; i < len; i++) {
            const auto rc = random_char();
            std::stringstream hexstream;
            hexstream << std::hex << rc;
            auto hex = hexstream.str();
            ss << (hex.length() < 2 ? '0' + hex : hex);
        }
        return ss.str();
    }

    static void map_copy(google::protobuf::Map<std::string, std::string>* target, 
                         const google::protobuf::Map<std::string, std::string>& source) {
        if (nullptr == target) {
            std::cout << "map_copy warning, target is nullptr!" << std::endl;
            return;
        }
        for (auto it = source.cbegin(); it != source.cend(); ++it) {    
            target->insert({it->first, it->second});
        }  
    }
};

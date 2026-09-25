#include<iostream>
#include<string>
#include<fstream>
#include<vector>

struct LogItem{
    std::string timestamp;      //时间
    std::string level;          //级别：INFO / WARE / ERROR
    std::string user_id;        //用户ID
    std::string action;         //动作：login / pay 等
    int latency_ms;             //耗时（毫秒，整数）
    int error_code;             //错误码（整数）
};

void data_getline(std::string fileName, std::vector<LogItem>& logs)
{
    //读取数据
    std::ifstream file(fileName);
    //检查文件是否打开
    if(file.is_open())
    {
        size_t loc_head = 0;
        size_t loc_hind = 0;
        int count = 0;
        //日志暂时存储
        std::string line;

        //循环解析每一条日志
        while(getline(file, line))
        {   
            LogItem item;
            count = 0;
            loc_head = 0;
            loc_hind = 0; 
            loc_hind = line.find("|", loc_hind);
            
            //日志时间单独处理
            if(loc_hind != std::string::npos)
            {
                //时间
                item.timestamp = line.substr(0, loc_hind-1);
            }
            loc_hind++;

            /*2026-09-20 22:43:24 | ERROR | user_955 | pay | 52ms | 502
            时间与其他字符处理进行单独处理*/
            while(count < 4)
            {
                loc_head = loc_hind;          
                loc_hind = line.find("|", loc_hind); 
                switch (count)
                {
                    case 0:     //级别
                        item.level = line.substr(loc_head+1, loc_hind-2-loc_head);
                        break;
                    case 1:     //ID
                        item.user_id = line.substr(loc_head+1, loc_hind-2-loc_head);
                        break;
                    case 2:     //动作
                        item.action = line.substr(loc_head+1, loc_hind-2-loc_head);
                        break;
                    case 3:     //耗时
                        item.latency_ms = std::stoi(line.substr(loc_head+1, loc_hind-4-loc_head));
                        break;
                    default:
                        std::cout << "出现错误！！！" << std::endl;
                        break;
                }
                loc_hind++;
                count++;
            }
            //日志错误码单独处理
            item.error_code = std::stoi(line.substr(++loc_hind));
            logs.push_back(item);
        }
        file.close();
    }
    else
    {
        std::cout << "无法打开文件" << std::endl;
    }
}

//vector验证
void vecprint(const std::vector<LogItem>& logs)
{
    for(const auto& item : logs)
    {
        std::cout << item.timestamp << " " 
              << item.level << " " 
              << item.user_id << " " 
              << item.action << " " 
              << item.latency_ms << "ms " 
              << item.error_code << std::endl;
    }
}

int main()
{
    std::vector<LogItem> logs;

    std::string fileName = "../Data/app.log";
    data_getline(fileName, logs);

    //vecprint(logs);

    return 0;
}
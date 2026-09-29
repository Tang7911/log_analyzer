#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<SQLiteCpp/SQLiteCpp.h>

struct LogItem{
    std::string timestamp;      //时间
    std::string level;          //级别：INFO / WARE / ERROR
    std::string user_id;        //用户ID
    std::string action;         //动作：login / pay 等
    int latency_ms;             //耗时（毫秒，整数）
    int error_code;             //错误码（整数）
};

//日志处理并存入vector容器
void data_getline(std::string fileName, std::vector<LogItem>& logs)
{
    //打开读取文件
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
            //记录寻找的“|”个数
            count = 0;
            //前位置
            loc_head = 0;
            //后位置
            loc_hind = 0; 

            //2026-09-20 22:43:24 | ERROR | user_955 | pay | 52ms | 502
            while(count < 5)
            {
                //此时loc_hind位于下一个截取字段前的空格处，loc_head也是
                loc_head = loc_hind; 
                //寻找“|”的位置     
                loc_hind = line.find("|", loc_hind); 
                switch (count)
                {
                    case 0:
                        //时间解析：从0号位开始读取，“|”位置在loc_hind下标处，
                        //时间的最后一个字符和“|”中间还有一个空格，算上“|”一共两个多余字符，
                        //因为下标本来就少1，所以再减1，即得正确截取长度
                        item.timestamp = line.substr(0, loc_hind-1);
                        break;
                    case 1:     //级别
                        //loc_head+1，使得substr的开始读取位置变为下一个字段的开头
                        //此时loc_hind寻找到“|”的位置，位于“|”处，减去（loc_head+1）和 1 后得到要截取的字符串长度
                        item.level = line.substr(loc_head+1, loc_hind-2-loc_head);
                        break;
                    case 2:     //ID
                        //同上
                        item.user_id = line.substr(loc_head+1, loc_hind-2-loc_head);
                        break;
                    case 3:     //动作
                        //同上
                        item.action = line.substr(loc_head+1, loc_hind-2-loc_head);
                        break;
                    case 4:     //耗时
                        //因为耗时的单位为 ms ，而接收变量类型为 int 所以要减 2 去掉 ms
                        //stoi可以将字符串转换成数字
                        //其余同上
                        item.latency_ms = std::stoi(line.substr(loc_head+1, loc_hind-4-loc_head));
                        break;
                    default:
                        std::cout << "出现错误！！！" << std::endl;
                        break;
                }
                //让loc_hind往后一位，寻找下一个“|”
                loc_hind++;
                count++;
            }
            //日志错误码单独处理
            //因为在处理完耗时字段后，又loc_hind++, 所以此时loc_hind 位于错误码前的空格
            //substr(++loc_hind)即可让loc_hind位置变为错误码的第一位，不加截取长度，默认截取到字符串末尾
            item.error_code = std::stoi(line.substr(++loc_hind));
            //压入vector容器
            logs.push_back(item);
        }
        file.close();
    }
    else
    {
        std::cout << "无法打开文件" << std::endl;
    }
}

//写入csv
void write_to_csv(const std::vector<LogItem>& logs, const std::string& fileName)
{
    //打开写入文件
    std::ofstream out_file(fileName);
    if(out_file.is_open())
    {
        //创建csv表头
        out_file << "timestamp,level,user_id,action,latency_ms,error_code" << "\n";
        //循环写入csv
        for(const auto& item : logs)
        {
            out_file << item.timestamp << ","
                     << item.level << ","
                     << item.user_id << ","
                     << item.action << ","
                     << item.latency_ms << ","
                     << item.error_code << "\n";
        }
        //关闭文件
        out_file.close();
    }
    else
    {
        std::cout << "无法打开文件" << std::endl;
    }
}

//写入SQLite
void write_sql(const std::vector<LogItem>& logs, const std::string& fileName)
{
    try
    {
        SQLite::Database db(fileName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        db.exec("CREATE TABLE IF NOT EXISTS log_record (timestamp TEXT, level TEXT, user_id TEXT, action TEXT, latency_ms INTEGER, error_code INTEGER)");
        
        SQLite::Transaction transaction(db);
        SQLite::Statement insert(db, "INSERT INTO log_record (timestamp, level, user_id, action, latency_ms, error_code) VALUES(?, ?, ?, ?, ?, ?)");

        for(const auto& item : logs)
        {
            insert.bind(1, item.timestamp);
            insert.bind(2, item.level);
            insert.bind(3, item.user_id);
            insert.bind(4, item.action);
            insert.bind(5, item.latency_ms);
            insert.bind(6, item.error_code);

            insert.exec();
            insert.reset();
        }
        transaction.commit();
        std::cout << "数据已存入数据库" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "异常：" << e.what() << std::endl;
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

    write_sql(logs, "../Data/logs_output.db");

    //vecprint(logs);

    return 0;
}
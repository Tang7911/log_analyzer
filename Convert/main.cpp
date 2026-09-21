#include<iostream>
#include<string>
#include<fstream>

void data_getline(std::string fileName)
{
    std::ifstream file(fileName);
    if(file.is_open())
    {
        size_t loc = 0;
        int count = 0;
        std::string line;
        while(getline(file, line))
        {
            loc = line.find("|", loc);
            loc++;
        }
        file.close();
        std::cout << count << std::endl;
    }
    else
    {
        std::cout << "无法打开文件" << std::endl;
    }
}

int main()
{
    std::string fileName = "../Data/app.log";
    data_getline(fileName);

    return 0;
}
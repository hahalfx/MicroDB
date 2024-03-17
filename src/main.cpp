#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <map>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// Column definition
struct Column
{
    std::string name;
    std::string type;
    bool is_primary;

    Column(std::string name = "", std::string type = "", bool is_primary = false)
        : name(name), type(type), is_primary(is_primary) {}
};

char globaldbname;

// Table definition
class Table
{
public:
    std::string name;
    std::vector<Column> columns;
    std::vector<std::map<std::string, std::string>> rows;
    std::map<std::string, std::map<std::string, std::string>> index; // 主键索引

    Table() {} // 默认构造函数
    Table(std::string name) : name(name) {}

    void addColumn(const Column &column)
    {
        columns.push_back(column);
    }

    void insertRow(const std::map<std::string, std::string> &row)
    {
        rows.push_back(row);

        // 将数据写入文件
        std::ofstream file("data/" + name + "/" + name + ".dat", std::ios::app);
        for (const auto &[key, value] : row)
        {
            file << key << ", " << value << ";";
        }
        file << "\n";
    }

    void selectRows(const std::string &columnName)
    {
        for (const auto &row : rows)
        {
            auto it = row.find(columnName);
            if (it != row.end())
            {
                std::cout << it->second << std::endl;
            }
        }
    }

    void selectdata(const std::string &columnName, const std::string &columnvalue)
    {
        for (const auto &row : rows)
        {
            auto it = row.find(columnName); // 查找指定列名的条目

            if (it != row.end())
            {
                std::string value = it->second; // 获取列的值

                // 检查值是否与指定的数据名匹配
                if (value == columnvalue)
                {
                    // 如果匹配，可以在这里处理或存储该行数据
                    // 例如，将其打印到控制台
                    std::cout << "Matching row found:" << std::endl;
                    for (const auto &entry : row)
                    {
                        std::cout << entry.first << ": " << entry.second << std::endl;
                    }
                }
            }
        }
    }

    void createIndex()
    {
        for (auto &column : columns)
        {
            if (column.is_primary)
            {
                for (auto &row : rows)
                {
                    index[row[column.name]] = row;
                }
                break;
            }
        }
    }

    void deleteRowsWhere(const std::string &command)
    {
        std::string id;
        size_t pos = command.find("id = ");
        if (pos != std::string::npos)
        {
            id = command.substr(pos + 5); // 提取id值
            std::cout << id << std::endl;
        }
        else
        {
            std::cerr << "Error: Invalid command format." << std::endl;
            return;
        }

        // 删除满足条件的行（id 匹配）
        rows.erase(
            std::remove_if(rows.begin(), rows.end(), [&](const std::map<std::string, std::string> &row)
                           {
                               auto it = row.find("id");
                               return it != row.end() && it->second == id; }),
            rows.end());
        // 重新写入文件
        std::ofstream file("data/" + name + "/" + name + ".dat", std::ios::trunc);
        if (!file)
        {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
            return;
        }

        // 将剩余的行写入文件
        for (const auto &row : rows)
        {
            for (const auto &[key, val] : row)
            {
                file << key << ", " << val << ";";
            }
            file << "\n";
        }
    }

    void deleteAllRows()
    {
        // 首先清空内存中的行数据
        rows.clear();

        // 然后打开对应的数据文件，用空内容覆盖它
        // 注意：这里不使用 std::ios::app 模式，因为我们想覆盖文件，而不是追加内容
        std::ofstream file("data/" + name + "/" + name + ".dat", std::ios::trunc);

        if (!file)
        {
            std::cerr << "Error: Unable to open file for clearing." << std::endl;
            return;
        }

        // 文件被自动清空，因为以 std::ios::trunc 模式打开
        file.close(); // 关闭文件
    }
};

// Database definition
class Database
{
public:
    std::string name;
    std::unordered_map<std::string, Table> tables;

    Database(std::string name) : name(name)
    {
        fs::create_directories("data/" + name); // 创建数据库目录
    }

    void addTable(const Table &table)
    {
        tables[table.name] = table;

        // 创建表文件和索引文件
        std::ofstream dataFile("data/" + name + "/" + table.name + ".dat");
        std::ofstream indexFile("data/" + name + "/" + table.name + ".idx");

        saveTableToFile(table); // 将表保存到文件
    }

    void saveTableToFile(const Table &table)
    {
        std::ofstream file("data/" + name + "/" + table.name + ".dat");
        if (file.is_open())
        {
            for (const auto &column : table.columns)
            {
                file << column.name << " (" << column.type << "), ";
            }
            file << "\n";
            for (const auto &row : table.rows)
            {
                for (const auto &column : table.columns)
                {
                    file << row.at(column.name) << ", ";
                }
                file << "\n";
            }
            file.close();
        }
        else
        {
            std::cerr << "Unable to open file for writing." << std::endl;
        }
    }

    void loadTableFromFile(const std::string &tableName, const std::string &dbName)
    {

        std::ifstream file("data/" + dbName + "/" + tableName + ".dat");
        if (file.is_open())
        {
            std::cout << "数据库加载table中" << std::endl;
            std::string line;
            while (std::getline(file, line))
            {
                // 解析每一行并加载到 Table 对象中
                // 这需要您根据数据文件格式进行实现
            }
            file.close();
        }
        else
        {
            std::cerr << "Unable to open file for reading." << std::endl;
        }
    }

    void loadTables(const std::string &dbName)
    {
        for (const auto &entry : fs::directory_iterator("data/" + dbName))
        {
            std::cout << "数据库加载中" << std::endl;

            if (entry.path().extension() == ".dat")
            {
                std::string tableName = entry.path().stem().string();
                Table table(tableName);
                loadTableFromFile(tableName, dbName);
                table.createIndex(); // 创建索引
                tables[tableName] = table;
                std::cout << "加载数据库成功" << std::endl;
            }
        }
    }
};

// Database Management System
class DBMS
{
private:
    std::unordered_map<std::string, Database> databases;
    Database *currentDatabase = nullptr;

public:
    // DDL methods
    void createDatabase(const std::string &dbName)
    {
        if (databases.find(dbName) != databases.end())
        {
            std::cout << "Error: Database already exists." << std::endl;
            return;
        }
        databases.emplace(dbName, Database(dbName));
        std::cout << "Database created successfully." << std::endl;
    }

    void useDatabase(const std::string &dbName)
    {
        auto it = databases.find(dbName);
        if (it == databases.end())
        {
            std::cout << "Warning: Database does not exist in current system, searching in files." << std::endl;
        }

        // 检查 data/ 目录下是否存在与 dbName 相对应的子目录
        fs::path dbPath = fs::path("data") / dbName;
        if (fs::exists(dbPath) && fs::is_directory(dbPath))
        {
            // 如果目录存在，调用 loadTables 函数加载表
            it->second.loadTables(dbName); // 假设 loadTables 是 Database 类的成员函数
        }
        else
        {
            std::cout << "Error: Database directory does not exist." << std::endl;
            return;
        }

        // 设置当前数据库
        currentDatabase = &it->second;
        std::cout << "Using database: " << dbName << std::endl;
    }

    void createTable(const std::string &tableName, const std::vector<Column> &columns)
    {
        if (!currentDatabase)
        {
            std::cout << "Error: No database selected." << std::endl;
            return;
        }
        if (currentDatabase->tables.find(tableName) != currentDatabase->tables.end())
        {
            std::cout << "Error: Table already exists in the current database." << std::endl;
            return;
        }
        Table newTable(tableName);
        for (const auto &column : columns)
        {
            newTable.addColumn(column);
        }
        currentDatabase->tables[tableName] = newTable;
        currentDatabase->addTable(newTable);
        std::cout << "Table created successfully." << std::endl;
    }

    // DML methods
    void insertIntoTable(const std::string &tableName, const std::map<std::string, std::string> &row)
    {
        if (!currentDatabase)
        {
            std::cout << "Error: No database selected." << std::endl;
            return;
        }
        auto it = currentDatabase->tables.find(tableName);
        if (it == currentDatabase->tables.end())
        {
            std::cout << "Error: Table does not exist." << std::endl;
            return;
        }
        it->second.insertRow(row);
        std::cout << "Row inserted successfully into " << tableName << "." << std::endl;
    }

    void selectFromTable(const std::string &tableName, const std::string &columnName)
    {
        if (!currentDatabase)
        {
            std::cout << "Error: No database selected." << std::endl;
            return;
        }
        auto it = currentDatabase->tables.find(tableName);
        if (it == currentDatabase->tables.end())
        {
            std::cout << "Error: Table does not exist." << std::endl;
            return;
        }
        it->second.selectRows(columnName);
    }

    void selectFromTable(const std::string &tableName, const std::string &columnName, const std::string &columnvalue)
    {
        if (!currentDatabase)
        {
            std::cout << "Error: No database selected." << std::endl;
            return;
        }
        auto it = currentDatabase->tables.find(tableName);
        if (it == currentDatabase->tables.end())
        {
            std::cout << "Error: Table does not exist." << std::endl;
            return;
        }
        it->second.selectdata(columnName, columnvalue);
    }

    void deleteFromTable(const std::string &tableName, const std::string &condition)
    {
        if (!currentDatabase)
        {
            std::cout << "Error: No database selected." << std::endl;
            return;
        }

        auto it = currentDatabase->tables.find(tableName);
        if (it == currentDatabase->tables.end())
        {
            std::cout << "Error: Table does not exist." << std::endl;
            return;
        }

        // 如果有条件，应用条件来删除特定行
        // 否则，删除所有行
        if (!condition.empty())
        {
            it->second.deleteRowsWhere(condition);
        }
        else
        {
            it->second.deleteAllRows();
        }
    }

    bool droptable(const std::string &tablename)
    {
        std::string filePath1 = "data/" + tablename + "/" + tablename + ".dat";
        std::string filePath2 = "data/" + tablename + "/" + tablename + ".idx";
        try
        {
            if (fs::remove(filePath1) && fs::remove(filePath2))
            {
                std::cout << "table deleted successfully." << std::endl;
                return true;
            }
            else
            {
                std::cout << "table not found." << std::endl;
                return false;
            }
        }
        catch (const fs::filesystem_error &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    bool dropdatabase(const std::string &databasename)
    {
        std::string directoryPath = "data/" + databasename;
        try
        {
            // 删除文件夹及其所有内容
            std::uintmax_t numRemoved = fs::remove_all(directoryPath);
            std::cout << "Total files/directories removed: " << numRemoved << std::endl;
            return true;
        }
        catch (const fs::filesystem_error &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }
};

// Command parser
void parseCommand(DBMS &dbms, const std::string &command)
{
    std::istringstream iss(command);
    std::string token;
    iss >> token;

    // Handle DDL commands
    if (token == "create" && iss >> token)
    {
        if (token == "database")
        {
            std::string dbName;
            iss >> dbName;
            dbms.createDatabase(dbName);
        }
        else if (token == "table")
        {

            std::string token, tableName, columnDefinitions;
            std::vector<Column> columns;

            iss >> tableName; // tableName = "person"

            // 获取括号内的内容
            getline(iss, columnDefinitions, '('); // 跳过到 '(' 的内容
            getline(iss, columnDefinitions, ')'); // 读取 '(' 和 ')' 之间的内容

            // 进一步解析列定义，例如："id int primary, name string"
            std::istringstream columnStream(columnDefinitions);
            std::string columnDetails;
            while (getline(columnStream, columnDetails, ','))
            {
                std::istringstream detailStream(columnDetails);
                std::string columnName, columnType, primaryw;
                detailStream >> columnName >> columnType;

                bool isPrimary = false;
                if (detailStream >> primaryw && primaryw == "primary")
                {
                    isPrimary = true;
                }

                columns.emplace_back(columnName, columnType, isPrimary);
            }
            dbms.createTable(tableName, columns);
        }
    }
    else if (token == "use")
    {
        std::string dbName;
        iss >> dbName;
        dbms.useDatabase(dbName);
    }

    // Handle DML commands
    else if (token == "insert")
    {
        std::string tableName, temp, value;
        char discard;                         // 用于丢弃不需要的字符，如括号和逗号
        std::vector<std::string> valuesArray; // 字符串数组用于存储值

        iss >> tableName >> temp; // 读取表名和 "values" 关键字

        if (temp == "values")
        {
            std::string values, name;

            // 获取括号内的内容
            getline(iss, values, '('); // 跳过到 '(' 的内容
            getline(iss, values, ')'); // 读取 '(' 和 ')' 之间的内容

            // 进一步解析列定义，例如："id int primary, name string"
            std::istringstream valueStream(values);
            std::string valueDetails;
            while (getline(valueStream, valueDetails, ','))
            {
                std::istringstream detailStream(valueDetails);
                getline(detailStream, value, '"'); // 跳过到 '(' 的内容
                getline(detailStream, value, '"'); // 读取 '(' 和 ')' 之间的内容

                valuesArray.push_back(value); // 将值添加到字符串数组
                // 此时，valuesArray 中包含了所有的值，您可以在这里对其进行进一步处理或使用
            }
            dbms.insertIntoTable(tableName, {{valuesArray[0], valuesArray[1]}}); // 将值传递给数据库操作
        }
    }

    else if (token == "select")
    {
        std::string columnName, from, tableName, wherePart, columnvalue, equal;
        std::string wantname;

        iss >> wantname; // 获取想要查询的列名

        // 解析 from 关键字和表名
        iss >> from >> tableName;

        // 检查是否有 where 子句
        std::string word;
        iss >> word;

        if (word == "where")
        {
            iss >> columnName;
            iss >> equal;
            iss >> columnvalue;
            dbms.selectFromTable(tableName, columnName, columnvalue);
            // std::getline(iss, wherePart); // 读取 where 子句的其余部分，包括条件
            //  在这里，您可以处理 where 子句，解析条件并执行查询
            //  示例：解析条件并执行查询
            //  wherePart 的格式可以是 "column op value"，例如 "id = 1001"
            //  您需要编写代码来解析这个条件，并使用它执行查询
            std::cout << "Performing SELECT query with WHERE clause: " << std::endl;
        }
        else
        {
            // 执行无条件查询，只选择指定列名的所有行
            dbms.selectFromTable(tableName, wantname);
            std::cout << "Performing SELECT query without WHERE clause. Selected column: " << columnName << std::endl;
        }
    }
    else if (token == "delete")
    {
        std::string condition, tableName;
        iss >> tableName;
        iss >> token;
        if (token == "where")
        {
            // 从命令中读取条件
            std::getline(iss, condition);
        }
        dbms.deleteFromTable(tableName, condition);
    }
    else if (token == "drop" && iss >> token)
    {
        if (token == "database")
        {
            std::string databasename;
            iss >> databasename;
            dbms.dropdatabase(databasename);
        }
        else if (token == "table")
        {
            std::string tablename;
            iss >> tablename;
            dbms.droptable(tablename);
        }
    }
}

// Main function
int main()
{
    DBMS myDBMS;
    std::string command;

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, command);
        if (command == "exit")
            break;
        parseCommand(myDBMS, command);
    }

    return 0;
}

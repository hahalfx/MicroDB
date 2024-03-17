# MicroDB

模仿MySQL，开发的一个微型数据库管理系统

## 具体功能

### **1.** 存储功能

目录结构和文件命名方式示例：

```
data //目录

|---- person //目录，库名

| |---- person.dat //表文件

| +---- person.idx //同名表的索引文件

+---- other
```

### **2.** DDL

1) `create database`

功能：创建数据库

语法：`create database <dbname>`

注：尖括号 <>标记括起了参数。在使用时，须给出具体的名字。例如：

`create database person`

以下同此。

2) `drop database`

功能：删除数据库

语法：`drop database <dbname>`

3) `use`

功能：切换数据库

语法：`use <dbname>3`

4) `create table`

功能：创建表

语法：`create table <table-name> (<column> <type> [primary], …)`

表名和列名：全英文小写，不含_和特殊字符。

数据类型type。作为简化，type只考虑以下两种：

* int：采用 C++的默认长度，不考虑多种 int。
* string：最长256字符的定长串。编码类型：UTF-8。

如指定列是primary（主键） ，则须为表建立索引。索引数据结构自定（例如二叉平衡

树等） 。

5) `drop table`

功能：删除表

语法：`drop table <table-name>`

注：删除表的同时，删除对应的索引（如果有的话） 。

### **3.** DML

1) `select`

功能：根据条件（如果有）查询表，显示查询结果。

语法：

`select <column> from <table> [ where <cond> ]`

* `<column>： <column-name> | *`。只需一个列名。*表示所有列。
* where子句：可选。如无，表示无条件查询。
  * `<cond> ：<column> <op> <const-value>`
  * `<op>`：=、<、>三者之一

2) `delete`

功能：根据条件（如果有）删除表中的记录。

语法：`delete <table> [ where <cond> ]`

* where子句：语法同select的。

3) `insert`

功能：在表中插入数据。

语法：`insert <table> values (<const-value>[, <const-value>…])`

注：字符串数据用双引号括起来

4) `update`

功能：根据条件（如果有）更新表中的记录。如无条件，则更新整张表。

语法：`update <table> set <column> = <const-value> [ where <cond> ]`

* where子句：语法同select的。

注：字符串数据用双引号括起来

### 界面

模仿 MySQL的交互式界面。例如

```
\> create table person (id int primary, name string)

反馈信息

\> use person

\> insert person values(1001, “peter”)

反馈信息

\> select name from person where id = 1001

反馈信息

\> exit
```


import sqlite3
import pandas as pd

# 连接到 SQLite 数据库（如果没有则创建）
connection = sqlite3.connect('./simulations/results/sca.db')

if connection.row_factory == sqlite3.Row:
    print("row_factory 已设置为 sqlite3.Row")
else:
    print("row_factory 未设置为 sqlite3.Row")

# 创建一个游标对象
cursor = connection.cursor()
# 查询所有表名
cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
tables = cursor.fetchall()

# 打印所有表名
print("表名列表:")
for table in tables:
    print(table[0])

'''
# 关闭连接
cursor.close()
connection.close()

'''
# 查看表中所有数据
cursor.execute('SELECT * FROM scalar')  # 替换 'users' 为你的表名
# 使用 Pandas 直接执行 SQL 查询
df = pd.read_sql_query("SELECT * FROM scalar", connection)

# 打印 DataFrame
print(df)

# 关闭游标和连接
cursor.close()
connection.close()

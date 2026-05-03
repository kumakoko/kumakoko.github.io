# new_type_sample.py

from typing import NewType  # 显式地从typing模块中导入NewType辅助类

UserId = NewType('UserId', int)  # 定义一个新类型UserId，且该类型是继承自int类型

 # 声明函数的参数类型是UserId，返回值类型是字符串
def get_user_name(user_id: UserId) -> str:
    return str(user_id)

#  执行类型检查，显式地声明整数42351是一个UserId
user_a = get_user_name(UserId(42351)) 
print("user_a id is "+user_a)

#  执行类型检查，整数-1不是一个UserId，但执行get_user_name函数也并不会出错
user_b = get_user_name(-1)
print("user_b id is "+user_b)
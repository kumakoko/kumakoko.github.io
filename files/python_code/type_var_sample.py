#type_var_sample.py

from typing import TypeVar  # 显式地从typing模块中导入TypeVar辅助类

T = TypeVar('T')            # 定义一个“泛型类型”T，这个类型可以指代一切
A = TypeVar('A',str,tuple)  # 定义一个“泛型类型”A，这个类型可以只可以指代str类型和tuple类型

# 返回一个带n个重复元素x的列表，列表中元素的类型是可以任意类型
def repeat(x: T, n: int) -> list[T]:
    return [x]*n

list_1 = repeat("abc",5)
print("list_1 is ",list_1)   # 打印结果 list_1 is ['abc','abc','abc','abc','abc']
list_2 = repeat((1,2,3),4)   # 打印结果 list_2 is [(1,2,3),(1,2,3),(1,2,3),(1,2,3)]
print("list_2 is ",list_2)
list_3 = repeat({1:"one",2:2,"three":3},3)
print("list_3 is ",list_3)   # 打印结果 list_3 is [{1:'one',2:2,'three',3},{1:'one',2:2,'three',3},{1:'one',2:2,'three',3} ]

# 比较两者中元素个数，返回元素个数较多的那个输入参数
def longest(x:A,y:A) -> A:
    if len(x) >= len(y):
        return x
    else:
        return y

print(longest("abc",('a','b')))         # 打印结果abc
print(longest("abc",('a','b','c','d'))) # 打印结果('a','b','c','d')
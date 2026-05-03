# callable_sample.py

from typing import Callable  # 显式地从typing模块中导入Callable辅助类
from typing import Tuple     # 显式地从typing模块中导入Tuple辅助类

def print_info(name:str, cid:int) -> Tuple[str, int]:
    print("my name is {0},my cid is {1}".format(name,cid))
    return name,cid


class CallbackDemo:
    callback_func = None
    
    # Callable
    # 声明参数cb是一个回调函数，该回调函数的两个参数分别是字符串和整数类型
    # 返回的两个参数，按Python规则，会组成一个元组。两个元素分别是字符串和整数类型
    def set_callback_func(cb:Callable[[str,int],Tuple[str, int]])->None:
        CallbackDemo.callback_func = cb
    
    def do_callback_func(name:str, cid:int)->tuple:
        if CallbackDemo.callback_func is None:
            return None
        else:
            return CallbackDemo.callback_func(name, cid)

# 判断是不是一个可回调的函数
print(isinstance(print_info, Callable))                 # 打印True
CallbackDemo.set_callback_func(print_info)
CallbackDemo.do_callback_func(name="Jack",cid=12345)    # 执行结果打印 my name is Jack,my cid is 12345
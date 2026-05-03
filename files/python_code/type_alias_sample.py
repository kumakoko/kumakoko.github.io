# type_alias_sample.py

Vector = list[float] # 这标明，Vector是一个列表，且里面的元素都是float类型

# scalar:float 表示参数是参数应是float类型
# vector:Vector 表示参数是参数应是列表类型，且里面的元素float都是float
def scale(scalar:float,vector:Vector) -> Vector:
    return [scalar * num for num in vector] # 将列表vector中的每个元素取出，和scalar相乘，然后再存进一个列表中，最后返回

v = [1.0, -4.2, 5.4]
print(v)
new_vector = scale(2.0, v)  # 类型检查器会敲定这个浮点数列表就是Vector类型
print(new_vector)
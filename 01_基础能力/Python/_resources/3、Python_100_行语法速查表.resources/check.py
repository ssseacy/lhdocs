# -*- coding: utf-8 -*-

# ===== 1. 基本类型与变量 =====

# 动态类型 — 不用声明类型
a = 10
b = 3.14
s = "hello"
flag = True
none_val = None

# 多重赋值
x, y, z = 1, 2, 3
x = y = z = 0

# 类型检查
print(type(a), isinstance(s, str))


# ===== 2. 数字 / 运算 =====

# 基本运算
c = a + b
d = a * b
mod = a % 3
exp = a ** 2  # 幂运算

# 比较运算
print(a > b, a == 10, a != 5)

# 布尔运算
print((a > 5) and flag, (a < 5) or (b > 2), not flag)


# ===== 3. 字符串 =====

# f-string（格式化字符串）
name = "Alice"
greeting = f"Hello, {name}, a + b = {a + b}"
print(greeting)

# 字符串方法
print(s.upper(), s.lower(), s.replace("l", "L"))
print(s.split("l"))


# ===== 4. 容器（列表、元组、字典、集合） =====

# 列表（类似 C++ 的 vector / std::vector）
lst = [1, 2, 3, 4]
lst2 = list(range(5))  # [0,1,2,3,4]

# 元组（不可修改）
tpl = (1, "a", 3.14)

# 字典（类似 C++ map）
d = {"one": 1, "two": 2}
d["three"] = 3
print(d["one"])
print(d.get("four", 0))

# 集合（无序、不重复）
sset = {1, 2, 3, 2}


# ===== 5. 条件判断 =====

if a > b:
    print("a 大于 b")
elif a == b:
    print("相等")
else:
    print("a 小于 b")

# 三元表达式（类似 C++ ?:）
min_val = a if a < b else b
print("min:", min_val)


# ===== 6. 循环 =====

# for 遍历
for i in range(5):  # 0 到 4
    print(i)

for item in lst:
    print(item)

# while 循环
n = 5
while n > 0:
    print(n)
    n -= 1
else:
    print("while 结束，没有被 break")

# break / continue
for i in range(10):
    if i == 5:
        break
    if i % 2 == 0:
        continue
    print("奇数:", i)


# ===== 7. 推导式（Python 的强项） =====

# 列表推导式
squares = [i * i for i in range(10)]
even = [i for i in range(10) if i % 2 == 0]

# 嵌套
pairs = [(i, j) for i in range(3) for j in range(2)]

# 字典 / 集合 推导
d2 = {i: i * i for i in range(5)}
s2 = {i * i for i in range(5)}

print(squares, even, pairs, d2, s2)


# ===== 8. 函数 =====

def add(x, y):
    return x + y

# 多返回值（实际返回的是 tuple）
def div_mod(x, y):
    return x // y, x % y

q, r = div_mod(7, 3)
print("quot:", q, "rem:", r)

# 默认参数 & 可变参数
def func(a, b=10, *args, **kwargs):
    print("a:", a, "b:", b)
    print("args:", args)
    print("kwargs:", kwargs)

func(1, 2, 3, 4, x=100, y=200)


# 匿名函数（lambda）
f = lambda x, y: x + y
print(f(3, 4))

# 高阶函数（map / filter / sorted）
nums = [1, 2, 3, 4, 5]
doubles = list(map(lambda x: x * 2, nums))
evens = list(filter(lambda x: x % 2 == 0, nums))
sorted_desc = sorted(nums, reverse=True)
print(doubles, evens, sorted_desc)


# ===== 9. 生成器 / 迭代器 =====

# 生成器表达式
gen = (i * i for i in range(5))
for v in gen:
    print("gen:", v)

# yield 定义生成器函数
def fib(n):
    a, b = 0, 1
    for _ in range(n):
        yield b
        a, b = b, a + b

for v in fib(10):
    print("fib:", v)


# ===== 10. 异常处理 =====

try:
    x = 1 / 0
except ZeroDivisionError as e:
    print("除 0 错误:", e)
except Exception as e:
    print("其他错误:", e)
else:
    print("没有异常")
finally:
    print("总是会执行")

# 自定义异常
class MyError(Exception):
    pass

def f_raise():
    raise MyError("出错了！")

try:
    f_raise()
except MyError as e:
    print("捕获到 MyError:", e)


# ===== 11. 类和面向对象 =====

class Person:
    # 类变量（所有实例共享）
    count = 0

    def __init__(self, name, age):
        self.name = name
        self.age = age
        Person.count += 1

    def say(self):
        print(f"My name is {self.name}, age {self.age}")

    @classmethod
    def how_many(cls):
        print("当前人数：", cls.count)

    @staticmethod
    def info():
        print("Person 类，用于演示 OOP")

    def __repr__(self):
        return f"Person(name={self.name}, age={self.age})"

# 使用
p1 = Person("Alice", 30)
p2 = Person("Bob", 25)
p1.say()
Person.how_many()
Person.info()
print(p2)


# ===== 12. 模块与包 =====

# 引入模块（像 C++ 的 include / import header）
import math
from math import sqrt, pi

print(sqrt(16), pi)

# 自己的模块（假设有 hello.py）
# import hello
# from hello import greet

# 如果自己写包结构：
# mypkg/
#   __init__.py
#   mod1.py
#   mod2.py


# ===== 13. 文件 I/O =====

# 打开文件 — 用 with 更安全
with open("test.txt", "w", encoding="utf-8") as f:
    f.write("hello python\n")

with open("test.txt", "r", encoding="utf-8") as f:
    for line in f:
        print("line:", line.strip())


# ===== 14. 常用内置函数 =====

print(len(lst), sum(nums), min(nums), max(nums))
print(sorted(nums), reversed(nums))

# 类型转换
print(int(3.14), float(2), str(123), list("abc"))

# 枚举 / zip
for idx, val in enumerate(nums):
    print(idx, val)

for a, b in zip([1, 2, 3], ["a", "b", "c"]):
    print(a, b)


# ===== 15. 切片（Slicing） =====

L = list(range(10))
print(L[1:5], L[:5], L[::2], L[::-1])  # 截取、步长、反转等

# 字符串 / tuple 也支持切片
t = (10, 20, 30, 40)
print(t[1:3])
s2 = "abcdef"
print(s2[::2])


# ===== 16. Lambda + 装饰器（简单） =====

# 装饰器示例
def my_decorator(func):
    def wrapper(*args, **kwargs):
        print("调用前")
        result = func(*args, **kwargs)
        print("调用后")
        return result
    return wrapper

@my_decorator
def say_hi(name):
    print(f"Hi, {name}!")

say_hi("World")


# ===== 17. 类型提示 (Type Hints) =====

def greeting(name: str) -> str:
    return f"Hello, {name}"

print(greeting("Tom"))


# ===== 18. if __name__ == "__main__" 用法 =====

def main():
    print("这是主函数")

if __name__ == "__main__":
    main()


# ===== 19. 异步 (简要) =====

import asyncio

async def hello_async():
    print("Hello async")
    await asyncio.sleep(1)
    print("Goodbye async")

async def main_async():
    await hello_async()

# 运行异步
asyncio.run(main_async())


# ===== 20. 注释 =====

# 单行注释使用 #
"""
多行注释（字符串常量形式）
可以写在函数/类/模块开头，形成 docstring
"""

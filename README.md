**qm算法**

将逻辑表达式的所有取值打表输出，并且使用不完全的quine mccluskey算法找出一个稍简单的等价表达式
不完全的意思是它缺少素项表部分，因此得到的表达式通常都不是最简表达式

一时兴起的为了实现出来玩一玩
一天闲暇时间写出来的垃圾东西，算法性能也不好（主要是常数不好），可读性也烂的一批，维护性完全没有的小辣鸡代码

$\neg$  !

$\wedge$ &

$\vee$ |

$\rightarrow$ >

$\leftrightarrow$ =

按照上面的符号对应关系输入，括号不变，例如

$\neg q\rightarrow(a\wedge (b\leftrightarrow c))$

就是

```
!q>(a&(b=c))
```



**waring**

程序不会检测非法的式子

可能存在bug

时间复杂度至少在O(2^n)所以变量过多会跑相当久

**程序内容**

输入合法表达式后

首先会打印真值表

然后给出一个较简单的等价式子
# Vanillang 表达式

## 概述

Vanillang 有多种表达式类别，首先可以分为有类型表达式和无类型表达式，有类型表达式在类型不为 `void` 时有值，有值时还要区分是否产生新值。

注：

- 关于表达式能否位于赋值语句的左边，参考 [statement.md](./statement.md)

## 基本表达式

### 括号表达式

一对小括号中间可以是一个表达式，加上这对小括号整体构成括号表达式，它是一种基本表达式，它的值和类型与括号内的表达式保持一致。

示例：

```vanillang
(a + b)
(-1)
("Vanillang")
```

### 字面量表达式

指由单个字面量构成的表达式，字面量的语法规范及其类型参考 [literal.md](./literal.md)。字面量表达式一定有类型和值。

示例：

```vanillang
42
3.14
2.71828d
0b10100
0x3f3f3f3f
0o777
"Hello, world!"
f"Hello, $(player.getName())"
r"This is a raw string"
[1, 2, 3, 4, 5]
[I; 1, 0, foo, bar, baz]
[@a, currEntity, "Steve", "Alex"]
{id: "minecraft:oak_log", state: {axis: "y"}}
@r[team="red", gamemode="creative"]
true
false
```

### 关键字表达式

指单个关键字构成的表达式，参考下表：

| 关键字   | 作用                                                                                | 是否产生新值 |
| -------- | ----------------------------------------------------------------------------------- | ------------ |
| `this`   | 表示当前类的实例，只能在实例方法的方法体中使用                                      | 否           |
| `super`  | 表示当前类的基类实例，相当于 `this` 的向上类型转换                                  | 否           |
| `none`   | 表示一个空值，由于 `Optional<T>` 是泛型类型，因此 `none` 的具体类型要在上下文中确定 | 是           |
| `callee` | 表示当前函数本身，不能在构造器中使用                                                | 否           |
| 基本类型 | 参考标识符表达式中声明类别为类型部分的描述                                          | -            |

### 标识符表达式

指单个标识符构成的表达式，它是否有类型，以及有类型是具体是什么，取决于标识符的声明，但可以确定的是有值时不会产生新值：

| 标识符声明类别 | 是否有类型 | 是否有值 | 备注                               |
| -------------- | ---------- | -------- | ---------------------------------- |
| 变量           | 是         | 是       | -                                  |
| 函数           | 是         | 是       | -                                  |
| 类型           | 否         | 否       | 可以携带泛型参数，遵循类型注解语法 |

## 一元表达式

特指运算符是操作数的前缀的表达式，其中操作数本身是一个表达式。
在没有重载的情况下，一元运算符及其适用类型，以及整个表达式的值和类型见下表：

| 运算符 | 接收类型                                         | 表达式类型         | 表达式的值       | 是否产生新值         |
| ------ | ------------------------------------------------ | ------------------ | ---------------- | -------------------- |
| `+`    | 数值类基本类型                                   | 与操作数类型一致   | 与操作数的值一致 | 是（但可能会被优化） |
| `-`    | 数值类基本类型                                   | 与操作数的类型一致 | 操作数的相反数   | 是                   |
| `~`    | 除 `long` 和 `ulong` 外所有整数基本类型和 `bool` | 与操作数的类型一致 | 操作数的按位非   | 是                   |
| `!`    | `bool`                                           | `bool`             | 操作数的逻辑非   | 是                   |

示例：

```vanillang
+1
+0xff
+num
-5
-num
~0xffffffff
!(a == b)
```

## 二元表达式

表示同时具有左右两个操作数，运算符作为中缀的表达式，除 `instanceof` 的右操作数是类型注解以外，其余操作数都应该是表达式。
在没有重载的情况下，二元运算符及其适用类型，以及整个表达式的值和类型见下表：

### 算术类

| 运算符 | 左操作数类型     | 右操作数类型       | 表达式类型                                | 表达式的值                 | 是否产生新值 |
| ------ | ---------------- | ------------------ | ----------------------------------------- | -------------------------- | ------------ |
| `+`    | `int` 或 `float` | 必须与左操作数一致 | 与左操作数一致                            | 两操作数的和               | 是           |
| `-`    | `int` 或 `float` | 必须与左操作数一致 | 与左操作数一致                            | 两操作数的差               | 是           |
| `*`    | `int` 或 `float` | 必须与左操作数一致 | 与左操作数一致                            | 两操作数的乘积             | 是           |
| `/`    | `int` 或 `float` | `int` 或 `float`   | `float`                                   | 两操作数的商               | 是           |
| `//`   | `int`            | `int`              | `int`                                     | 两操作数整除，结果向下取整 | 是           |
| `%`    | `int`            | `int`              | `int`                                     | 左操作数取余右操作数的值   | 是           |
| `**`   | `int` 或 `float` | `int` 或 `float`   | 左右均为 `int` 时为 `int`，否则为 `float` | 左操作数的右操作数次方     | 是           |

注：

- 接收 `int` 的地方可以接收 `byte`、`ubyte`、`short` 和 `ushort`，它们会被提升为 `int`，但这个过程不会产生新值，相当于当作 `int` 解释
- 由于 Mojang 不提供 64 位原生算子，因此 Vanillang 暂时无法实现 64 位数值类型的运算，上述所有类型均不含 `long`、`ulong` 和 `double`
- 由于记分板并不支持原生的无符号 32 位整数，因此 `uint` 类型也不能参与运算

### 比较类

| 运算符 | 左操作数类型 | 右操作数类型   | 表达式类型  | 表达式的值                                          | 是否产生新值 |
| ------ | ------------ | -------------- | ----------- | --------------------------------------------------- | ------------ |
| `==`   | 非浮点类型   | 与左操作数相同 | `bool`      | `true` 或 `false`                                   | 是           |
| `!=`   | 非浮点类型   | 与左操作数相同 | `bool`      | `false` 或 `true`                                   | 是           |
| `===`  | 引用类型     | 与左操作数相同 | `bool`      | `true` 或 `false`                                   | 是           |
| `!==`  | 引用类型     | 与左操作数相同 | `bool`      | `false` 或 `true`                                   | 是           |
| `>`    | 可比较类型   | 与左操作数相同 | `bool`      | `true` 或 `false`                                   | 是           |
| `<`    | 可比较类型   | 与左操作数相同 | `bool`      | `true` 或 `false`                                   | 是           |
| `>=`   | 可比较类型   | 与左操作数相同 | `bool`      | `true` 或 `false`                                   | 是           |
| `<=`   | 可比较类型   | 与左操作数相同 | `bool`      | `true` 或 `false`                                   | 是           |
| `<=>`  | 可比较类型   | 与左操作数相同 | `Ordering?` | 可比较时为 `Ordering` 枚举实例，无法比较时为 `none` | 是           |

注：

- 可比较的基本类型有：`byte`、`ubyte`、`short`、`ushort`、`int`、`float`，没有 `uint`、`long`、`ulong` 和 `double` 的原因见上一备注

### 逻辑运算类

| 运算符 | 左操作数类型 | 右操作数类型 | 表达式类型 | 表达式的值       | 是否产生新值 |
| ------ | ------------ | ------------ | ---------- | ---------------- | ------------ |
| `&&`   | `bool`       | `bool`       | `bool`     | 两操作数的逻辑与 | 是           |
| `\|\|` | `bool`       | `bool`       | `bool`     | 两操作数的逻辑或 | 是           |

注：

- 逻辑运算支持短路，当左操作数能够直接决定整个表达式的值时，右操作数不再进行求值

### 位运算类

| 运算符 | 左操作数类型 | 右操作数类型 | 表达式类型 | 表达式的值                              | 是否产生新值 |
| ------ | ------------ | ------------ | ---------- | --------------------------------------- | ------------ |
| `&`    | `int`        | `int`        | `int`      | 两操作数的按位与                        | 是           |
| `\|`   | `int`        | `int`        | `int`      | 两操作数的按位或                        | 是           |
| `^`    | `int`        | `int`        | `int`      | 两操作数的按位异或                      | 是           |
| `<<`   | `int`        | `int`        | `int`      | 左操作数算术左移 n 位，n 是右操作数的值 | 是           |
| `>>`   | `int`        | `int`        | `int`      | 左操作数算术右移 n 位，n 是右操作数的值 | 是           |
| `>>>`  | `int`        | `int`        | `int`      | 左操作数逻辑右移 n 位，n 是右操作数的值 | 是           |

注：

- 操作数可以接收 `byte`、`ubyte`、`short` 和 `ushort`，和算术运算一样具有类型提升
- 若位移运算的右操作数为负数，则会向反方向位移，但实际应用中强烈不建议这样编写

### 空值合并

| 运算符 | 左操作数类型  | 右操作数类型 | 表达式类型   | 表达式的值                                     | 是否产生新值 |
| ------ | ------------- | ------------ | ------------ | ---------------------------------------------- | ------------ |
| `??`   | `Optional<T>` | 非空类型 `T` | 非空类型 `T` | 左操作数非空时为左操作数中的值，否则为右操作数 | 是           |

## 后缀表达式

包含函数调用、下标操作和成员访问：

| 运算符 | 操作数类型                                   | 表达式类型                       | 表达式的值                       | 是否产生新值             |
| ------ | -------------------------------------------- | -------------------------------- | -------------------------------- | ------------------------ |
| `()`   | 可调用类型                                   | 函数的返回类型                   | 函数的返回值（为 `void` 时无值） | 是（返回 `void` 时除外） |
| `[]`   | 实现了相应重载的类型                         | 以重载返回类型为准               | 以重载的返回值为准               | 是（赋值语句左边时除外） |
| `.`    | 适用于任意能访问成员的标识符，不一定要有类型 | 取决于被访问的成员，不一定有类型 | 取决于被访问的成员，不一定有值   | 否                       |

注：

- 可调用类型指的是函数类型和实现了调用运算符重载的自定义类型。特别地，如果操作数是一个类型名，那么整个表达式对应的操作为调用相应类型的构造器，返回一个新构造的对象
- 下标操作不适用于基本类型，下标操作的重载决议参考 [operator.md](./operator.md)
- 特别地，`super()` 表示调用基类的构造器，只能在构造器的函数体中使用，括号内应该传入构造器参数

## 范围表达式

用于创建一个 `Range<T>` 类型的对象，在没有重载的情况下，操作数的类型以及整个表达式的值和类型见下表：

| 运算符 | 左操作数类型 | 右操作数类型 | 表达式类型   | 表达式的值   | 是否产生新值 |
| ------ | ------------ | ------------ | ------------ | ------------ | ------------ |
| `..`   | `int`        | `int`        | `Range<int>` | 表示一个范围 | 是           |

注：

- 操作数类型为 `int` 时，左右操作数可以至多省略一个，当左操作数被省略时，结果所表示的最小值即为 `int` 类型的最小值；当右操作数被省略时，结果所表示的最大值即为 `int` 类型的最大值

## 条件表达式

用于按条件返回一个表达式的值，参考下表：

| 运算符 | 条件类型 | `true` 分支表达式类型 | `false` 分支表达式类型 | 表达式的值         | 是否产生新值 |
| ------ | -------- | --------------------- | ---------------------- | ------------------ | ------------ |
| `?:`   | `bool`   | 不限                  | 与 `true` 分支相同     | 与 `true` 分支相同 | 否           |

---

**附：Vanillang 表达式文法产生式**

```ebnf
Expression ::= ConditionalExpression;
ConditionalExpression ::= NullishCoalescingExpression ['?' Expression ':' Expression];
NullishCoalescingExpression ::= LogicalOrExpression ('??' LogicalOrExpression)*;
LogicalOrExpression ::= LogicalAndExpression ('||' LogicalAndExpression)*;
LogicalAndExpression ::= EqualityExpression ('&&' EqualityExpression)*;
EqualityExpression ::= RelationalExpression (('==' | '!=' | '===' | '!==') RelationalExpression)*;
RelationalExpression ::= RangeExpression ((('<' | '<=' | '>' | '>=' ) RangeExpression) | ('instanceof' Type))*;
RangeExpression ::= (BitwiseOrExpression ['..' [BitwiseOrExpression]]) | ('..' BitwiseOrExpression);
BitwiseOrExpression ::= BitwiseXorExpression ('|' BitwiseXorExpression)*;
BitwiseXorExpression ::= BitwiseAndExpression ('^' BitwiseAndExpression)*;
BitwiseAndExpression ::= ShiftExpression ('&' ShiftExpression)*;
ShiftExpression ::= AdditiveExpression (('<<' | '>>' | '>>>') AdditiveExpression)*;
AdditiveExpression ::= MultiplicativeExpression (('+' | '-') MultiplicativeExpression)*;
MultiplicativeExpression ::= UnaryExpression (('*' | '/' | '//' | '%') UnaryExpression)*;
UnaryExpression ::= ['!' | '~' | '-' | '+'] ExponentialExpression;
ExponentialExpression ::= PostfixExpression ['**' UnaryExpression];
PostfixExpression ::= PrimaryExpression (('.' (GeneralizedIdentifier | IdentifierLike)) | ('(' [ArgumentList] ')') | ('[' Expression ']'))*;
PrimaryExpression ::= '(' Expression ')' | Literal | IdentifierLike | PrimitiveType | ExpressionKeyword;

IdentifierLike ::= Identifier {if peek() is not a type declaration else} Identifier ['<' GenericArgumentList '>'];
Literal ::= Number | String | Boolean | ListLikeLiteral | DictLiteral | Selector;
PrimitiveType ::= 'byte' | 'ubyte' | 'short' | 'ushort' | 'int' | 'uint' | 'long' | 'ulong' | 'float' | 'double' | 'bool';
ExpressionKeyword ::= 'this' | 'super' | 'none' | 'callee';

String ::= StringLiteral [(StringLiteral | Interpolation)*];
Boolean ::= 'true' | 'false';
ListLikeLiteral ::= '[' [('B' | 'I' | 'L') ';'] [Expression (',' Expression)*] ']';
DictLiteral ::= '{' [DictEntry (',' DictEntry)*] '}';
Selector ::= ('@p' | '@r' | '@a' | '@e' | '@s' | '@n') ['[' SelectorArgumentList ']'];
ArgumentList ::= (Expression (',' Expression)* ['context' '=' DictLiteral]) | ('context' '=' DictLiteral);

Interpolation ::= '$(' Expression ')';
DictEntry ::= (GeneralizedIdentifier | StringLiteral) ':' Expression;
SelectorArgumentList ::= SelectorArgument (',' SelectorArgument)*;

SelectorArgument ::= GeneralizedIdentifier ('=' | '!=') Expression;
```
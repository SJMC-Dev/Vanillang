# Vanillang 字面量

Vanillang 的字面量分为简单字面量和复合字面量，其中简单字面量是表示特定值的字符串，复合字面量是通过特定语法组合而成的特殊表达式。

这些是简单字面量：
- 整型字面量
- 浮点字面量
- 布尔字面量
- 字符字面量
- 字符串字面量（普通字符串、原始字符串）

这些是复合字面量：
- 字符串字面量（格式化字符串）
- 数组字面量
- 列表字面量
- 复合标签字面量
- 选择器字面量

## 整型字面量

表示一个整数，进制取决于前缀，具体类型取决于后缀。
这些例子都是合法的整型字面量：

```vanillang
5
9i
16si
10000ui
1234
1b
127b
255ub
256s
65535us
2147483648sl
```

整型字面量支持十进制、二进制、八进制和十六进制四种进制：

| 进制     | 前缀 | 合法数字    |
| -------- | ---- | ----------- |
| 十进制   | 无   | [0-9]       |
| 二进制   | `0b` | [01]        |
| 八进制   | `0o` | [0-7]       |
| 十六进制 | `0x` | [0-9a-fA-F] |

整型字面量支持的后缀有下面这些，如果未指定后缀，则默认认为后缀为 `i`：

| 后缀 | 表示类型                                      |
| ---- | --------------------------------------------- |
| `b`  | 十进制中表示 `byte`，非十进制中表示 `ubyte`   |
| `sb` | `byte`                                        |
| `ub` | `ubyte`                                       |
| `s`  | 十进制中表示 `short`，非十进制中表示 `ushort` |
| `ss` | `short`                                       |
| `us` | `ushort`                                      |
| `i`  | 十进制中表示 `int`，非十进制中表示 `uint`     |
| `si` | `int`                                         |
| `ui` | `uint`                                        |
| `l`  | 十进制中表示 `long`，非十进制中表示 `ulong`   |
| `sl` | `long`                                        |
| `ul` | `ulong`                                       |

注：
- 所有前缀与后缀中的字母均不区分大小写
- 后缀 `b` 不能在十六进制中使用，因为它是一个合法的十六进制数字
- 单独的 `0b` 不是进制前缀，它表示值为 0，类型为 `byte` 的字面量

## 浮点字面量

表示一个浮点数，类型取决于后缀，支持十进制小数与科学记数法两种写法。
浮点字面量必须包含小数点或指数符号 `e`，否则编译器会认定为整型字面量。
这些例子都是合法的浮点字面量：

```vanillang
3.141596f
1.414213562373d
1e3f
3e-4
.1f
2.e1d
```

十进制小数写法中，整数部分和小数部分任一部分为 0 时，可以省略不写，但必须保留小数点。
科学记数法由尾数部分、指数符号 `e` 和指数部分构成，其中尾数部分可以是一个十进制小数或整数，指数部分必须是一个十进制整数，指数部分可以有正负号。

浮点字面量支持的后缀有下面这些，如果未指定后缀，则默认认为后缀为 `f`：

| 后缀 | 表示类型 |
| ---- | -------- |
| `f`  | `float`  |
| `d`  | `double` |

注：
- 所有后缀与指数符号中的字母均不区分大小写。

## 布尔字面量

表示一个 `bool` 类型的值，只能为 `true` 或 `false`。

## 字符字面量

注意，字符字面量虽然使用字符表示，但它本质上是一个 `ubyte` 类型的值。
字符字面量需要使用单引号包围，例如 `'a'`、`'b'`、`'c'`。

## 字符串字面量

使用双引号包围一个字符串，即可构成一个字符串字面量。
这些都是合法的字符串字面量：

```vanillang
"Hello, Vanillang!"
f"the value is $(expr)"
r"This is a raw string"
```

字符串字面量可以加上前缀，以区分不同表示法：

| 前缀 | 表示法       | 备注                                       |
| ---- | ------------ | ------------------------------------------ |
| 无   | 普通字符串   | 支持转义符                                 |
| `f`  | 格式化字符串 | 支持插值语法；表达式必须可以被转换为字符串 |
| `r`  | 原始字符串   | 不支持转义符，不能包含双引号               |

注：
- 前缀不区分大小写。

## 数组字面量

用于表示一个类型为 `vanillang.types.Array<T>` 的字面常量。
这些都是合法的数组字面量：

```vanillang
[expr1, expr2, expr3]
[I; expr1, expr2]
[B; expr1, expr2]
[L; expr1, expr2]
```

具体表示法如下：

| 表示法              | 备注                                                                              |
| ------------------- | --------------------------------------------------------------------------------- |
| 数组通用表示        | 所有表达式必须具有相同类型                                                        |
| SNBT 整型数组表示   | 匹配 SNBT 整型数组；所有表达式必须能被转换成 `int` 或 `uint` 类型，且类型唯一     |
| SNBT 字节型数组表示 | 匹配 SNBT 字节数组；所有表达式必须能被转换成 `byte` 或 `ubyte` 类型，且类型唯一   |
| SNBT 长整型数组表示 | 匹配 SNBT 长整型数组；所有表达式必须能被转换成 `long` 或 `ulong` 类型，且类型唯一 |

注：
- SNBT 表示法中，分号前的字母必须大写。

## 列表字面量

用于表示一个类型为 `vanillang.types.List` 的字面常量，每个元素可以具有不同类型。
表示形式与数组通用表示一致，例如：

```vanillang
[expr1, expr2, expr3]
```

## 复合标签字面量

用于表示一个类型为 `vanillang.types.Dict` 的字面常量，值不支持无引号字符串，键满足广义标识符命名规则时，引号可以省略。
典型的写法如下：

```vanillang
{ key1: value1, key2: value2, key3: value3 }
{ "key1": value1, "key2": value2, "key3": value3 }
```


## 选择器字面量

表示一个类型为 `vanillang.types.Selector` 的字面常量，语法类似于原版选择器，但有细微的差异，不能简单地认为是原版指令中的目标选择器语法。
这些都是合法的选择器字面量的写法：

```vanillang
@p
@r
@a
@e
@s
@n
@e[sort="nearest", limit=2]
@p[tag="candidate"]
@a[team!="hider"]
@r[team!=none]
```

注：
- 选择器参数的等号或不等号后面接收的是 Vanillang 表达式，如果需要接收字符串则要加双引号
- 不等于在原版选择器中使用的是 `=!`，在 Vanillang 中，我们使用 `!=`，以确保语法与其他地方保持一致
- 原版选择器部分等号或不等号右边允许留空，在 Vanillang 中，我们使用关键字 `none` 来代替这一语法

---

**附：Vanillang 字面量文法产生式**

```ebnf
Literal ::= Number | Char | String | Boolean | ListLikeLiteral | DictLiteral | Selector;
String ::= StringLiteral [ (StringLiteral | Interpolation)* ];
Interpolation ::= '$(' Expression ')';
Boolean ::= 'true' | 'false';
ListLikeLiteral ::= '[' [ ('B' | 'I' | 'L') ';' ] [ Expression (',' Expression)* ] ']';
DictLiteral ::= '{' [ DictEntry (',' DictEntry)* ] '}';
Selector ::= ('@p' | '@r' | '@a' | '@e' | '@s' | '@n') [ '[' SelectorArgumentList ']' ];
DictEntry ::= (GeneralizedIdentifier | StringLiteral) ':' Expression;
SelectorArgumentList ::= SelectorArgument (',' SelectorArgument)*;
SelectorArgument ::= GeneralizedIdentifier ('=' | '!=') Expression;
```
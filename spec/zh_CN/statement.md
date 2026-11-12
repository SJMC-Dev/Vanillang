# Vanillang 语句

语句是 Vanillang 函数体中的执行单元，Vanillang 支持多种语句。
语句不以分号结尾，我们使用的是语句完整性规则，一个完整语句后面只要有换行或其他分隔符（比如 `}`）即可。

## 块语句

块语句是使用大括号包围的语句列表，其本身也视为一个语句，块语句拥有自己的作用域，可以带有元数据。
例外：块语句位于控制流语句后面时，不能携带元数据。 

示例：

```vanillang
{
    let c = a + b
    return c
}

metadata(experimental) {
    world.freezeTick()
}
```

## 赋值语句

在 Vanillang 中，赋值是一种语句，基本语法如下：

```text
表达式 赋值符号 [try] 表达式
```

相关特性：
- 除 `??=` 先计算左边再计算右边以外，其余赋值语句都是先计算右边再计算左边
- `try` 不能用于 `??=`
- 赋值语句左边的表达式不能产生新值，唯一的例外是产生新引用后访问其成员，因为成员访问不会产生新值

其中，赋值符号有这些，除了 `=` 是普通赋值以外，其余均为复合赋值：

| 符号   | 作用                                                                 |
| ------ | -------------------------------------------------------------------- |
| `=`    | 普通赋值，将右边赋值给左边                                           |
| `??=`  | 空值合并赋值，左边必须是一个可选类型，仅当左边为空时将右边赋值给左边 |
| `+=`   | `a += b` 等价于 `a = a + b`                                          |
| `-=`   | `a -= b` 等价于 `a = a - b`                                          |
| `*=`   | `a *= b` 等价于 `a = a * b`                                          |
| `/=`   | `a /= b` 等价于 `a = a / b`                                          |
| `//=`  | `a //= b` 等价于 `a = a // b`                                        |
| `%=`   | `a %= b` 等价于 `a = a % b`                                          |
| `**=`  | `a **= b` 等价于 `a = a ** b`                                        |
| `&=`   | `a &= b` 等价于 `a = a & b`                                          |
| `^=`   | `a ^= b` 等价于 `a = a ^ b`                                          |
| `\|=`  | `a \|= b` 等价于 `a = a \| b`                                        |
| `<<=`  | `a <<= b` 等价于 `a = a << b`                                        |
| `>>=`  | `a >>= b` 等价于 `a = a >> b`                                        |
| `>>>=` | `a >>>= b` 等价于 `a = a >>> b`                                      |

示例：

```vanillang
a = b
a ??= b
a += b
a -= b
a *= try foo(b)
a /= b
a //= b
a %= b
a **= b
a &= b
a ^= b
a |= b
a <<= 2
a >>= 2
a >>>= 2
```

## 表达式语句

表达式自身可以单独作为语句，一个独立的表达式语句前面也可以选择加上 `try` 来进行错误或空值传播。

更多信息参考[表达式部分](./expression.md)

示例：

```vanillang
add(1, 2)
try mayFail()
```

## 变量声明语句

变量声明可以单独作为语句，用于声明当前块级作用域内的局部变量，在语句中，右边的初始化表达式可以加上 `try`。

示例：

```vanillang
let entity = try Entity.fromSelector(@n[type="minecraft:creeper"])
let time = world.queryCurrentTime()
```

## 常量声明语句

常量声明也可以单独作为语句，用于声明当前块级作用域内的常量，初始化表达式必须符合常量声明的要求。

示例：

```vanillang
const LIMIT = 10
const EPSILON = 1e-3
```

## if 语句

if 语句有两种写法：普通 if 语句和 if-let 语句。

### 普通 if 语句

基本语法：

```text
if (表达式) 语句 [else 语句]
```

写法和大多数语言一样，需要注意的是条件表达式的类型只能为 `bool`。
如果条件表达式是 `insteaceof` 表达式并且左操作数是局部变量，则 then 分支的语句内变量收窄到右边的类型注解所表示的类型。

示例：

```vanillang
if (a % 2 == 0) {
    world.print("even")
} else {
    world.print("odd")
}
```

### if-let 语句

基本语法：

```text
if (变量声明) 语句 [else 语句]
```

if-let 语句用于解包一个可选类型的值，变量声明的初始化表达式必须是一个可选类型，其值非空时变量被初始化为被包装的值，并执行 then 分支，否则执行 else 分支。声明的变量仅在 then 分支可用。

示例：

```vanillang
if (let entity = Entity.fromSelector(@e[limit=1, type="minecraft:creeper"])) {
    world.print(entity.getData()["id"])
}
```

## switch 语句

switch 语句有两种类别：字面量匹配和类型匹配，类型匹配额外支持 switch-let 语法。

### 字面量匹配

基本语法：

```text
switch (表达式) {
    (case 字面量 -> 语句)*
    [default -> 语句]
}
```

用法类似于其他语言，每个语句是独立的，不会隐式贯穿。

示例：

```vanillang
switch (value) {
    case 1 -> world.print("one")
    case 2 -> world.print("two")
    default -> {
        world.print("other")
        return
    }
}
```

### 类型匹配

基本语法：

```text
switch (局部变量或变量声明) {
    (case 类型注解 [when 守卫条件表达式] -> 语句)*
    [default -> 语句]
}
```

用于向下类型匹配，对应条件下的守卫表达式和语句中变量收窄到该 case 对应的类型。

示例：

```vanillang
switch (entity) {
    case Player when entity.getHealth() == 10 -> {
        world.print("HP is greater than or equals 10", entity)
    }
    default -> {
        world.print("I don't care", @a)
    }
}

switch (let gamemode = player.getGamemode().valueOr(Gamemode.Survival)) {
    case Gamemode.Creative -> {
        player.setGamemode(Gamemode.Survival)
        world.showTitle(player, "Creative mode is not allowed!")
    }
    case Gamemode.Spectator -> {
        world.showActionbar(player, "You are in spectator mode.")
    }
}
```

## for 语句

for 语句用于循环遍历一个可迭代对象，基本语法如下：

```text
[label 标签] for (let 变量名 in 可迭代对象) 语句
```

可迭代对象是指实现了 `vanillang.types.Iterable<T>` 接口的对象。

for 语句可以拥有一个标签，标签是一种标识符，用于指定 `break` 和 `continue` 的跳出点，因此不能和其他作用域内的标识符重名。

示例：

```vanillang
for (let i in 0..9) {
    world.print(i)
}
```

## while 语句

while 语句有两种写法：普通 while 语句和 while-let 语句。

### 普通 while 语句

基本语法：

```text
[label 标签] while (表达式) 语句
```

和大多数语言一样，while 语句用于在表达式为真时循环执行，需要注意条件表达式的类型必须是 `bool`。

示例：

```vanillang
while (player.getGamemode().valueOr(Gamemode.Survival) != Gamemode.Survival) {
    player.setGamemode(Gamemode.Survival)
}
```

### while-let 语句

基本语法：

```text
[label 标签] while (变量声明) 语句
```

while-let 语句用于解包一个可选类型的值，用法与 if-let 类似，在值非空时循环执行。

示例：

```vanillang
while (let player = Player.fromSelector(@r)) {
    world.kill(player)
}
```

## break 语句

用于跳出当前循环，如果有标签则跳出标签标注的循环，基本语法如下：

```text
break [标签]
```

示例：

```vanillang
break
break outer
```

注：如果 `break` 后面有换行符，则视为无标签，无论下一行是不是标识符。

## continue 语句

用于跳过本轮循环，如果有标签则跳过标签标注的本轮循环，基本语法如下：

```text
continue [标签]
```

示例：

```vanillang
continue
continue outer
```

注：如果 `continue` 后面有换行符，则视为无标签，无论下一行是不是标识符。

## return 语句

用于结束函数，可以返回一个值，基本语法如下：

```text
return [表达式]
```

示例：

```vanillang
return
return 0
```

注：如果 `return` 后面有换行符，则视为无返回值，无论下一行是不是表达式。

## reload 语句

用于重载加载数据包，用法：

```vanillang
reload
```

---

**附：Vanillang 语句文法产生式**

```ebnf
Statement ::= ExpressionStatement | AssignmentStatement | VariableDeclarationStatement | ConstantDeclarationStatement | BlockStatement | ControlFlowStatement;

ExpressionStatement ::= ['try'] Expression;

AssignmentStatement ::= Expression AssignmentOperator ['try'] Expression;
AssignmentOperator ::= '=' | '??=' | '+=' | '-=' | '*=' | '/=' | '//=' | '%=' | '**=' | '&=' | '^=' | '|=' | '<<=' | '>>=' | '>>>=';

VariableDeclarationStatement ::= (LetPrimary '=' ['try'] Expression);

ConstantDeclarationStatement ::= ConstantDeclaration;

BlockStatement ::= [Metadata] '{' Statement* '}';

ControlFlowStatement ::= IfStatement | SwitchStatement | [ 'label' Identifier ] WhileStatement | [ 'label' Identifier ] ForStatement | ReturnStatement | BreakStatement | ContinueStatement | ReloadStatement;

IfStatement ::= 'if' '(' (Expression | VariableDeclaration) ')' Statement [ 'else' Statement ];

SwitchStatement ::= 'switch' '(' (Expression | VariableDeclaration) ')' '{' (SwitchCase '->' Statement)* [ 'default' '->' Statement ] '}';
SwitchCase ::= ('case' Literal) | ('case' Type [ 'when' Expression ]);

WhileStatement ::= 'while' '(' Expression | VariableDeclaration ')' Statement;

ForStatement ::= 'for' '(' LetPrimary 'in' Expression ')' Statement;

ReturnStatement ::= 'return' [ Expression ];

BreakStatement ::= 'break' [ Identifier ];

ContinueStatement ::= 'continue' [ Identifier ];

ReloadStatement ::= 'reload';
```
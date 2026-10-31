# Vanillang 语法

## 概述

Vanillang 的语法设计旨在提供一种简洁、易读且功能强大的编程语言，适用于 Minecraft 原版开发。Vanillang 的语法借鉴了多种现代编程语言的特点，同时结合了 Minecraft 数据包的特定需求，形成了一套独特的语法体系。

Vanillang 的语法主要包括以下几个方面：

- 模块与包
- 声明
- 表达式
- 语句
- 注释

Vanillang 不使用分号作为语句或声明的结束符，而是使用换行符来分隔语句或声明，且支持语句完整性规则：如果换行符前面不是一个完整的语句或声明，则忽略该换行符。

## 模块与包

Vanillang 定义一个模块为一个后缀名为 `.vnl` 的源代码文件，一个包为一个目录，包内可以包含多个模块。模块和包的命名规则与标识符相同，但不能使用关键字和保留字。其中，根包定义为向编译器提供的指定目录。

可以使用 `import` 声明导入一个包、模块或标识符：

- 导入的标识符必须是使用 `export` 声明导出的。导入时可以使用 `as` 关键字为导入的标识符指定一个别名，或者使用 `*` 导入一个包或模块内的所有导出标识符。
- 导入模块时，可以使用模块名访问模块内的导出标识符，例如 `import vanillang.math` 后可以使用 `math.PI` 访问 `math` 模块内的 `PI` 常量；也可以使用 `as` 关键字为模块指定一个别名，例如 `import vanillang.math as m` 后可以使用 `m.PI` 访问 `math` 模块内的 `PI` 常量。
- 导入包时，可以使用包名访问包内所有模块的导出标识符，例如 `import vanillang` 后可以使用 `vanillang.ui.TextDisplayUi` 访问 `vanillang` 包内的 `ui` 模块内的 `TextDisplayUi` 类；也可以使用 `as` 关键字为包指定一个别名，例如 `import vanillang as vnl` 后可以使用 `vnl.ui.TextDisplayUi` 访问 `vanillang` 包内的 `ui` 模块内的 `TextDisplayUi` 类。需要注意的是，导入包时不能使用 `*` 导入包内的所有模块，`*` 仅适用于导入标识符。

可以使用 `export` 声明导出一个标识符，导出的标识符可以被其他模块导入使用。导出时可以使用 `as` 关键字为导出的标识符指定一个别名，指定别名后，在 `import` 声明中只能使用别名导入该标识符，不能使用原名导入该标识符。

导入声明只能出现在模块顶层，不能出现在其他任何地方。导入声明的作用域为整个模块，即在模块内的任何位置都可以使用导入的标识符。 导入声明的执行顺序为从上到下，导入声明会按照它们在模块中的出现顺序依次执行。
导出声明只能出现在模块结尾，不能出现在其他任何地方。

下面给出模块的产生式：

```ebnf
Module ::= (ImportDeclaration*) (TopIdentifierDeclaration*) (ExportDeclaration*);
```

## 声明

Vanillang 顶层标识符声明是模块中的基本元素，用于告知编译器这个模块有哪些标识符以及它们的属性；成员声明是类、接口或枚举中的基本元素，用于告知编译器这个类、接口或枚举有哪些成员标识符以及它们的属性；模块导入声明用于导入一个包、模块或标识符；模块导出声明用于导出一个标识符。

Vanillang 支持以下几种顶层标识符声明：

- 变量声明
- 函数声明
- 类型声明

Vanillang 支持以下几种成员声明：

- 属性声明
- 方法声明
- 枚举成员声明

顶层标识符和成员声明可以使用元数据进行修饰，元数据是一些供编译器识别并影响编译器行为的标记，可以用于标记已弃用的元素、实验性的元素或禁用警告等。元数据如果出现则必须出现在一个声明的前面。

下面给出声明的产生式：

```ebnf
TopIdentifierDeclaration ::= [ Metadata ] (VariableDeclaration | FunctionDeclaration | TypeDeclaration);
ImportDeclaration ::= 'import' ImportPath;
ExportDeclaration ::= 'export' ExportList;

VariableDeclaration ::= VariableDeclarationPrimary '=' Expression;
FunctionDeclaration ::= RegularFunctionDeclaration | NativeFunctionDeclaration;
TypeDeclaration ::= ClassDeclaration | InterfaceDeclaration | EnumDeclaration | TypeAliasDeclaration;
PropertyDeclaration ::= Identifier ':' Type ['=' Expression];
InterfaceMethodDeclaration ::= [ Metadata ] (FunctionSignature | OperatorOverloadingSignature);
Metadata ::= 'metadata' '(' MetadataTerm (',' MetadataTerm)* ')';

VariableDeclarationPrimary ::= 'let' Identifier [ ':' Type ];
RegularFunctionDeclaration ::= FunctionSignature FunctionBody;
NativeFunctionDeclaration ::= 'native' FunctionSignature;
ParameterList ::= Parameter (',' Parameter)*;
ClassDeclaration ::= [ 'final' ] 'class' Identifier [ '<' GenericParameterList '>' ] [ 'extends' Type ] [ 'implements' Type (',' Type)* ] ClassBody;
InterfaceDeclaration ::= 'interface' Identifier [ '<' GenericParameterList '>' ] InterfaceBody;
EnumDeclaration ::= 'enum' Identifier [ '<' GenericParameterList '>' ] EnumBody;
TypeAliasDeclaration ::= 'type' Identifier [ '<' GenericParameterList '>' ] '=' Type;
ImportPath ::= Identifier ('.' Identifier)* [ '.' '*' | 'as' Identifier | '{' ImportPathList '}' ];
ExportList ::= Identifier (',' Identifier)*;
MetadataTerm ::= GeneralizedIdentifier [ SimpleStringLiteral ];
FunctionSignature ::= 'func' Identifier '(' [ ParameterList ] ')' ['->' (Type | 'void')];

RelativeImportPath ::= (Identifier | 'parent') ('.' (Identifier | 'parent'))* [ '.' '*' | 'as' Identifier | '{' RelativeImportPathList '}' ];
Type ::= (PrimitiveType | Identifier ('.' Identifier)* [ '<' GenericArgumentList '>' ]) [ '?' ];
Parameter ::= Identifier ':' Type;

GenericParameterList ::= Identifier (',' Identifier)*;
GenericArgumentList ::= Type (',' Type)*;
ImportPathList ::= ImportPathItem (',' ImportPathItem)*;

FunctionBody ::= BlockStatement;
ClassBody ::= '{' ClassMember* '}';
InterfaceBody ::= '{' InterfaceMethodDeclaration* '}';
EnumBody ::= '{' EnumMemberDeclaration* '}';
ImportPathItem ::= 'self' [ 'as' Identifier ] | ImportPath | '*';

ClassMember ::= [ Metadata ] [ 'private' | `protected` | 'public' ] (([ 'static' ] PropertyDeclaration) | ([ 'static' | 'override' ] FunctionDeclaration) | Constructor | ([ 'override' ] OperatorOverloading));
Constructor ::= 'init' '(' [ ParameterList ] ')' FunctionBody;
OperatorOverloading ::= RegularOperatorOverloading | NativeOperatorOverloading;
RegularOperatorOverloading ::= OperatorOverloadingSignature FunctionBody;
NativeOperatorOverloading ::= 'native' OperatorOverloadingSignature;
OperatorOverloadingSignature ::= `op` OverloadableOperator '(' [ ParameterList ] ')' [ '->' (Type | 'void')];
EnumMemberDeclaration ::= [ Metadata ] Identifier [ '(' [ EnumAssociatedValueList ] ')' ];

EnumAssociatedValueList ::= EnumAssociatedValue (',' EnumAssociatedValue)*;
EnumAssociatedValue ::= Identifier ':' Type;

OverloadableOperator ::= '+' | '-' | '*' | '/' | '//' | '%' | '**' | '==' | '<=>' | '&' | '\|' | '^' | '~' | '<<' | '>>' | '>>>' | ('(' ')') | ('[' ']') | '..';
```

### 变量声明

变量声明使用 `let` 关键字，除 `for` 语句循环变量以外，必须包含一个初始化表达式，变量的类型可以通过类型注解指定，如果没有指定类型，则由编译器根据初始化表达式推断类型。

下面是一些变量声明的示例：

```vanillang
let x: int = 10
let y: int = 0xFF
let name: string = "Vanillang"
let PI: float = 3.14159f
```

### 函数声明

函数声明用于声明一个函数，必须包含一个参数列表，参数列表中的每个参数都必须包含一个类型注解，参数列表可以是空的。函数声明可以选择性地包含一个返回类型注解，如果没有指定返回类型，则由编译器根据函数体中的 `return` 语句推断返回类型，如果函数体中没有 `return` 语句，则返回类型为 `void`。

下面是一些函数声明的示例：

```vanillang
func add(a: int, b: int) -> int {
    return a + b
}

func greet(name: string) {
    print("Hello, " + name + "!")
}
```

### 类型声明

类型声明用于声明一个类型，Vanillang 支持以下几种类型声明：

- 类声明：用于声明一个类，可以包含成员属性和成员方法，可以使用 `extends` 关键字指定一个父类，使用 `implements` 关键字指定一个或多个接口。
- 接口声明：用于声明一个接口，只能包含实例成员方法声明，不能包含实现，Vanillang 不支持接口继承。
- 枚举声明：用于声明一个枚举模式，可以包含一个或多个枚举成员，枚举模式的每个成员都可以有一个或多个关联值，可以使用 `switch` 来匹配枚举模式，就像匹配子类一样。
- 类型别名声明：用于声明一个类型别名，可以将一个类型标识符赋值给另一个类型标识符，表示它们是同一个类型。

下面是一些类型声明的示例：

```vanillang
class Player {
    name: string
    health: int
}

interface Damageable {
    func takeDamage(amount: int) -> void
}

enum Result {
    Success(value: string)
    Failure(error: string)
}

type NamespaceId = string
```

### 属性声明

类成员属性声明用于声明一个类的成员属性，可以指定访问修饰符，使用 `static` 关键字指定静态属性。类成员属性必须包含一个类型注解，对于静态属性，必须包含一个初始化表达式；对于实例属性，不应该包含初始化表达式，必须在构造函数中进行初始化。

下面是一些类成员属性声明的示例：

```vanillang
class Player {
    public name: string
    private health: int = 20
    static maxHealth: int = 20
}
```

### 方法声明

类成员方法声明类似于函数声明，用于声明一个类的成员方法，可以指定访问修饰符，使用 `static` 关键字指定静态方法。类成员方法必须包含一个参数列表，参数列表中的每个参数都必须包含一个类型注解，参数列表可以是空的。类成员方法可以选择性地包含一个返回类型注解，如果没有指定返回类型，则由编译器根据方法体中的 `return` 语句推断返回类型，如果方法体中没有 `return` 语句，则返回类型为 `void`。对于实例方法，方法体内可以使用 `this` 关键字访问当前实例的成员属性和成员方法；对于静态方法，方法体内不能使用 `this` 关键字访问当前实例的成员属性和成员方法，但可以使用类名访问当前类的静态属性和静态方法。

### 模块导入声明

模块导入声明用于导入一个包、模块或标识符，可以使用 `as` 关键字为导入的标识符指定一个别名，或者使用 `*` 导入一个包或模块内的所有导出标识符。同时支持大括号导入，可以在导入时指定要导入的标识符列表，并且可以为每个导入的标识符指定一个别名。

下面是一些模块导入声明的示例：

```vanillang
import vanillang.math
import vanillang.math as m
import vanillang.math.*
import vanillang as vnl
import vanillang.ui.TextDisplayUi
import vanillang.ui.{ TextDisplayUi, Button }
import vanillang.ui.{ self, TextDisplayUi as TDU, Button as Btn }
```

### 模块导出声明

模块导出声明用于导出一个标识符，不能使用别名导出：

```vanillang
export Player, add
```

## 表达式

Vanillang 表达式是由一个或多个操作数和一个或多个运算符组成的代码片段，用于计算一个值，表达式分为 17 个不同的优先级，各个优先级的运算符及结合方式如下表所示：

| 优先级 | 运算符                                                                                          | 结合方式 |
| ------ | ----------------------------------------------------------------------------------------------- | -------- |
| 1      | `()`、`[]`、`.`、`?.`                                                                           | 左结合   |
| 2      | `**`                                                                                            | 右结合   |
| 3      | `!`、`~`、`-`（单目）、`+`（单目）                                                              | 右结合   |
| 4      | `*`、`/`、`//`、`%`                                                                             | 左结合   |
| 5      | `+`、`-`                                                                                        | 左结合   |
| 6      | `<<`、`>>`、`>>>`                                                                               | 左结合   |
| 7      | `&`                                                                                             | 左结合   |
| 8      | `^`                                                                                             | 左结合   |
| 9      | `\|`                                                                                            | 左结合   |
| 10     | `..`                                                                                            | 左结合   |
| 11     | `<`、`<=`、`>`、`>=`、`instanceof`                                                              | 左结合   |
| 12     | `==`、`!=`、`===`、`!==`                                                                        | 左结合   |
| 13     | `&&`                                                                                            | 左结合   |
| 14     | `\|\|`                                                                                          | 左结合   |
| 15     | `??`                                                                                            | 左结合   |
| 16     | `?:`                                                                                            | 右结合   |
| 17     | `=`、`??=`、`+=`、`-=`、`*=`、`/=`、`//=`、`%=`、`**=`、`&=`、`^=`、`\|=`、`<<=`、`>>=`、`>>>=` | 右结合   |

下面给出表达式的产生式：

```ebnf
Expression ::= AssignmentExpression;
AssignmentExpression ::= ConditionalExpression [ ('=' | '??=' | '+=' | '-=' | '*=' | '/=' | '//=' | '%=' | '**=' | '&=' | '^=' | '|=' | '<<=' | '>>=' | '>>>=') AssignmentExpression ];
ConditionalExpression ::= NullishCoalescingExpression [ '?' AssignmentExpression ':' ConditionalExpression ];
NullishCoalescingExpression ::= LogicalOrExpression ('??' LogicalOrExpression)*;
LogicalOrExpression ::= LogicalAndExpression ('||' LogicalAndExpression)*;
LogicalAndExpression ::= EqualityExpression ('&&' EqualityExpression)*;
EqualityExpression ::= RelationalExpression (('==' | '!=' | '===' | '!==') RelationalExpression)*;
RelationalExpression ::= RangeExpression (('<' | '<=' | '>' | '>=' | 'instanceof') RangeExpression)*;
RangeExpression ::= (BitwiseOrExpression [ '..' [ BitwiseOrExpression ] ]) | ('..' BitwiseOrExpression);
BitwiseOrExpression ::= BitwiseXorExpression ('|' BitwiseXorExpression)*;
BitwiseXorExpression ::= BitwiseAndExpression ('^' BitwiseAndExpression)*;
BitwiseAndExpression ::= ShiftExpression ('&' ShiftExpression)*;
ShiftExpression ::= AdditiveExpression (('<<' | '>>' | '>>>') AdditiveExpression)*;
AdditiveExpression ::= MultiplicativeExpression (('+' | '-') MultiplicativeExpression)*;
MultiplicativeExpression ::= UnaryExpression (('*' | '/' | '//' | '%') UnaryExpression)*;
UnaryExpression ::= [ '!' | '~' | '-' | '+' ] ExponentialExpression;
ExponentialExpression ::= PostfixExpression [ '**' UnaryExpression ];
PostfixExpression ::= PrimaryExpression (('.' (GeneralizedIdentifier | IdentifierLike)) | ('?.' GeneralizedIdentifier) | ('(' [ ArgumentList ] ')') | ('[' Expression ']'))*;
PrimaryExpression ::= '(' Expression ')' | Literal | IdentifierLike | PrimitiveType | 'this' | 'super' | 'none';

IdentifierLike ::= Identifier { if peek() is not a type declaration else } Identifier [ '<' GenericArgumentList '>' ];
Literal ::= Number | Char | String | Boolean | ListLikeLiteral | DictLiteral | Selector;
PrimitiveType ::= 'byte' | 'short' | 'int' | 'long' | 'float' | 'double' | 'bool' | 'string'

String ::= StringLiteral [ (StringLiteral | Interpolation)* ];
Boolean ::= 'true' | 'false';
ListLikeLiteral ::= '[' [ ('B' | 'I' | 'L') ';' ] [ Expression (',' Expression)* ] ']';
DictLiteral ::= '{' [ DictEntry (',' DictEntry)* ] '}';
Selector ::= ('@p' | '@r' | '@a' | '@e' | '@s' | '@n') [ '[' SelectorArgumentList ']' ];
ArgumentList ::= (Expression (',' Expression)* [ 'context' '=' Expression ]) | ('context' '=' Expression);

Interpolation ::= '$(' Expression ')';
DictEntry ::= (GeneralizedIdentifier | StringLiteral) ':' Expression;
SelectorArgumentList ::= SelectorArgument (',' SelectorArgument)*;

SelectorArgument ::= GeneralizedIdentifier '=' Expression;
```

### 基础表达式

基础表达式由字面量、标识符或括号包围的表达式组成：

```vanillang
42
"Hello, Vanillang!"
true
player
(x + y)
```

### 后缀表达式

后缀表达式由一个表达式后跟一个或多个可选的后缀组成，后缀可以是成员访问、函数调用或索引访问：

```vanillang
player.name
player?.health
greet("Vanillang")
array[0]
```

### 指数表达式

指数表达式由一个表达式后跟一个可选的指数运算符 `**` 和另一个表达式组成：

```vanillang
2 ** 3
```

### 一元表达式

一元表达式由一个或多个一元运算符 `!`、`~`、`-`（单目）或 `+`（单目）后跟一个表达式组成：

```vanillang
!isOnline
-health
+score
```

### 乘除表达式

乘除表达式由一个表达式后跟零个或多个乘除运算符 `*`、`/`、`//` 或 `%` 和另一个表达式组成：

```vanillang
a * b
a / b
a // b
a % b
```

### 加减表达式

加减表达式由一个表达式后跟零个或多个加减运算符 `+` 或 `-` 和另一个表达式组成：

```vanillang
a + b
a - b
```

### 范围表达式

范围表达式由一个表达式后跟一个可选的范围运算符 `..` 和另一个表达式组成：

```vanillang
1..10
a..b
```

### 位移表达式

位移表达式由一个表达式后跟零个或多个位移运算符 `<<`、`>>` 或 `>>>` 和另一个表达式组成：

```vanillang
a << 2
a >> 2
a >>> 2
```

### 位运算表达式

位运算表达式由一个表达式后跟零个或多个位运算符 `&`、`^` 或 `\|` 和另一个表达式组成：

```vanillang
a & b
a ^ b
a | b
```

### 比较表达式

比较表达式由一个表达式后跟零个或多个比较运算符 `<`、`<=`、`>`、`>=` 或 `instanceof` 和另一个表达式组成：

```vanillang
a < b
a <= b
a > b
a >= b
player instanceof Entity
```

### 相等表达式

相等表达式由一个表达式后跟零个或多个相等运算符 `==` 或 `!=` 和另一个表达式组成：

```vanillang
a == b
a != b
```

### 逻辑表达式

逻辑表达式由一个表达式后跟零个或多个逻辑运算符 `&&` 或 `\|\|` 和另一个表达式组成：

```vanillang
a && b
a || b
```

### 空值合并表达式

空值合并表达式由一个表达式后跟零个或多个空值合并运算符 `??` 和另一个表达式组成：

```vanillang
a ?? b
```

### 条件表达式

条件表达式由一个表达式后跟一个问号 `?`、一个表达式、一个冒号 `:` 和另一个表达式组成：

```vanillang
condition ? expr1 : expr2
```

### 赋值表达式

赋值表达式由一个表达式后跟一个赋值运算符 `=`、`??=`、`+=`、`-=`、`*=`、`/=`、`//=`、`%=`、`**=`、`&=`、`^=`、`\|=`、`<<=`、`>>=` 或 `>>>=` 和另一个表达式组成：

```vanillang
a = b
a ??= b
a += b
a -= b
a *= b
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

## 语句

Vanillang 语句是用于执行操作的代码片段。

Vanillang 支持以下几种语句：

- 表达式语句
- 变量声明语句
- 块语句
- 控制流语句

下面给出语句的产生式：

```ebnf
Statement ::= ExpressionStatement | VariableDeclarationStatement | BlockStatement | ControlFlowStatement;

ExpressionStatement ::= Expression;
VariableDeclarationStatement ::= VariableDeclaration;
BlockStatement ::= '{' Statement* '}';
ControlFlowStatement ::= IfStatement | SwitchStatement | [ 'label' Identifier ] WhileStatement | [ 'label' Identifier ] ForStatement | ReturnStatement | BreakStatement | ContinueStatement | ReloadStatement;

IfStatement ::= 'if' '(' Expression ')' Statement [ 'else' Statement ];
SwitchStatement ::= 'switch' '(' Expression ')' '{' (SwitchCase '->' Statement)* [ 'default' '->' Statement ] '}';
WhileStatement ::= 'while' '(' Expression ')' Statement;
ForStatement ::= 'for' '(' VariableDeclarationPrimary 'in' Expression ')' Statement;
ReturnStatement ::= 'return' [ Expression ];
BreakStatement ::= 'break' [ Identifier ];
ContinueStatement ::= 'continue' [ Identifier ];
ReloadStatement ::= 'reload';

SwitchCase ::= ('case' Literal) | ('case' Type [ 'when' Expression ]);
```

### 表达式语句

表达式语句由一个表达式组成，执行该表达式并丢弃其结果：

```vanillang
player.name
greet("Vanillang")
```

### 变量声明语句

变量声明语句由一个变量声明组成，用于声明一个变量：

```vanillang
let x: int = 10
let name: string = "Vanillang"
let PI: float = 3.14159f
```

### 块语句

块语句由一对花括号 `{}` 包围的一系列语句组成，用于创建一个新的作用域：

```vanillang
{
    let x: int = 10
    world.print(x)
}
```

### 控制流语句

控制流语句用于控制程序的执行流程，包括条件语句、循环语句和跳转语句等：

```vanillang
if (player.health <= 0) {
    world.print("Game Over")
} else {
    world.print("Keep Fighting!")
}

switch (result) {
    case Success when result.value == "Victory" -> world.print("You Win!")
    case Success -> world.print("Success: " + result.value)
    case Failure -> world.print("Failure: " + result.error)
    default -> world.print("Unknown Result")
}

while (player.health > 0) {
    player.takeDamage(1)

    if (player.health == 10) {
        break
    }
}

label outer
for (let enemy in nearbyEnemies) {
    enemy.takeDamage(5)

    while (enemy.health > 0) {
        enemy.takeDamage(1)

        if (enemy.health == 1) {
            continue outer
        }
    }
}

return player.health

reload
```

## 注释

Vanillang 支持两种注释：单行注释和多行注释。单行注释以 `#` 开头，直到行末结束；多行注释以 `#*` 开头，以 `*#` 结尾，可以跨越多行。

```vanillang
# 这是一个单行注释

#* 
    这是一个多行注释
    可以跨越多行
*#
```

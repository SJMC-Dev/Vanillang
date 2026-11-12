# Vanillang 声明

Vanillang 支持多种声明，声明可以拥有元数据。
声明结尾使用换行符，无需使用分号。

## 导入声明

使用 `import` 关键字编写一个导入声明，后跟导入路径，基本语法如下：

```text
import 导入路径
```

导入声明有以下特性：

- 只能出现在模块最上方，不能位于其他声明的后面
- 具有公共前缀的导入路径可以使用花括号合并表示
- 花括号内可以使用 `self` 关键字来表示花括号前面的一个标识符
- `self` 只能位于花括号的首项
- 可以使用 `as` 关键字为导入的标识符指定别名，仅在当前模块生效
- 可以对一个模块使用 `*` 来导入这个模块下的所有标识符
- `as` 与 `*` 不能混用，`self` 与 `*` 也不能混用

这些都是合法的导入声明：

```vanillang
import vanillang.types.*
import vanillang.world
import vanillang.entity.Entity
import foo.bar.{
    self as b,
    Baz,
    Qux as Q
}
import a.b.{
    c.d.{
        self as dd,
        *,
        e,
        F as Foo
    },
    g as h
}
```

## 变量声明

使用 `let` 关键字声明一个变量，基本语法如下：

```text
let 变量名 [: 类型注解] [= [try] 初始化表达式]
```

变量声明有以下特性：

- 可以位于模块顶层和函数体中
- 可以位于 `for`、`while`、`if` 和 `switch` 语句的括号中
- 除位于 `for` 语句的括号中时，其余情况必须包含初始化表达式
- 顶层变量的作用域是模块级的，其他变量声明的作用域是块级的
- 在表达式的类型可以推断时，类型注解可以省略
- 模块顶层的变量初始化仅在数据包加载时，`onLoad` 函数调用之前发生
- 函数内的局部变量初始化表达式可以使用 `try` 关键字简化错误传播

这些都是合法的变量声明：

```vanillang
let x: int = 1
let result = call(expr)
let value = try call(expr)

# for 语句括号内
let i in 0..9
```

## 常量声明

使用 `const` 关键字声明一个编译时常量，基本语法如下：

```text
const 常量名 [: 类型注解] = 编译时可求值的字面量
```

常量声明有以下特性：

- 可以位于模块顶层和函数体中
- 不能位于控制流语句的括号中
- 类型只能为基本类型和 `vanillang.types.String`
- 必须包含初始化表达式，表达式必须是一个编译时可求值的字面量
- 类型注解可以省略

这些都是合法的常量声明：

```vanillang
const PI: float = 3.14
const MAX_PLAYERS_ONLINE = 10
```

## 函数声明

使用 `func` 关键字声明一个函数，基本语法如下：

```text
[native] func 函数名[<泛型参数列表>]([参数: 参数类型注解]*) [-> 返回类型注解] [where 泛型约束] [函数体]
```

函数声明有以下特性：

- 可以位于模块顶层、类、接口、枚举和不透明类型中
- 位于类型上下文中时，也叫方法声明
- 返回值能被推断时，可以省略
- 支持泛型参数与泛型约束
- 可以使用 `native` 修饰符将一个函数的实现声明为原生的，此时不允许包含函数体
- 接口中的函数声明不能带有实现，也无法使用 `native` 关键字
- 实例方法带有隐含的 `this` 参数，用户无需也不能显式声明，编译器会为它分配第一个参数位
- 重写基类方法或实现接口方法后，该方法自动成为多态方法，子类或实现类型中需要显式使用 `override` 标注
- 作为成员方法时，可以单独为所在类型的泛型参数添加泛型约束
- 函数签名由函数名、泛型参数个数和参数列表唯一确定，生成签名时，泛型参数会被替换为位置占位符
- 函数声明支持重载，具有相同函数名、不同函数签名的函数可以共存

这些都是合法的函数声明：

```vanillang
func add(a: int, b: int) -> int { return a + b }
native func setTime(time: int) -> void
```

## 类声明

使用 `class` 关键字声明一个类，基本语法如下：

```text
[final] class 类名[<泛型参数列表>] [extends 基类] [implements 接口列表] [where 泛型约束] {成员声明}
```

类声明有以下特性：

- 只能位于模块顶层
- 支持泛型参数与泛型约束
- 可以继承至多 1 个基类，不支持多继承
- 不能继承不透明类型
- 可以实现任意多个接口
- 使用 `final` 修饰的类不能被继承
- 支持实例成员与静态成员，包括属性和方法
- 支持声明构造器和运算符重载
- 用户未显式声明构造器时，由编译器生成一个无参默认构造
- 成员有 `public`、`protected` 和 `private` 三种访问修饰符，访问修饰符应该始终位于成员声明的最前面

这些都是合法的类声明：

```vanillang
class Foo extends Bar implements Baz, Qux {
    private a: int
    private b: float

    func getA() -> int {
        return a
    }

    func getB() -> float {
        return b
    }

    func setA(a: int) -> void {
        this.a = a
    }

    func setB(b: int) -> void {
        this.b = b
    }
}

final class Box<T> where T notnone {
    value: T

    op ==(other: T) -> bool {
        return value == other.value
    }
}
```

## 接口声明

使用 `interface` 关键字声明一个接口，基本语法如下：

```text
interface 接口名[<泛型参数列表>] [where 泛型约束] {成员声明}
```

接口声明有以下特性：

- 只能位于模块顶层
- 支持泛型参数与泛型约束
- 不支持接口继承
- 成员只能是接口方法和运算符重载，必须是公有的，前面不能有访问修饰符
- 成员不能拥有实现
  
这些都是合法的接口声明：

```vanillang
interface Cloneable<T> {
    func clone() -> T
}

interface Stringable {
    func toString() -> String
}
```

## 枚举声明

使用 `enum` 声明一个枚举类型，基本语法如下：

```text
enum 枚举名[<泛型参数列表>] [implements 接口列表] [where 泛型约束] {成员声明}
```

枚举声明有以下特性：

- 只能位于模块顶层
- 支持泛型参数与泛型约束
- 不支持继承
- 可以实现一个或多个接口
- 支持声明枚举成员，它与所在的枚举存在事实上的继承关系，始终是公有的，前面不能有访问修饰符
- 支持声明实例方法、静态属性、静态方法和运算符重载
- 实例方法、静态属性和静态方法可以使用和类一样的访问修饰符，访问修饰符应该始终位于声明的最前面
- 可以把枚举理解为没有实例属性和构造器的类

这些都是合法的枚举声明：

```vanillang
enum Optional<T> {
    Some(value: T)
    None

    func hasValue() -> bool {
        return this instanceof Some
    }

    func valueOr(defaultValue: T) -> T {
        if (let value = this) {
            return value
        }

        return defaultValue
    }
}

enum Result<T, E> {
    Ok(value: T)
    Error(message: E)

    func isOk() -> bool {
        return this instanceof Ok
    }
}
```

## 枚举成员声明

使用以下基本语法在枚举声明中声明一个枚举成员：

```text
枚举成员名[(关联值: 类型注解)+]
```

枚举成员声明有以下特性：

- 只能位于枚举声明中
- 可以带有任意多个关联值，需要注明关联值的类型

这些都是合法的枚举成员声明：

```vanillang
Some(value: T)
None
```

## 不透明类型声明

使用 `opaque` 声明一个不透明类型，基本语法如下：

```text
opaque 类型名[<泛型参数列表>] [extends 基类] [implements 接口列表] [where 泛型约束] {成员声明}
```

不透明类型声明有以下特性：

- 只能位于模块顶层
- 只能位于标准库中
- 支持泛型参数与泛型约束
- 可以继承至多 1 个基类，不支持多继承
- 只能继承不透明类型
- 可以实现任意多个接口
- 支持实例方法和静态方法
- 不支持成员属性
- 支持声明运算符重载
- 不允许声明构造器
- 成员有 `public`、`protected` 和 `private` 三种访问修饰符，访问修饰符应该始终位于声明的最前面

这些都是合法的不透明类型声明：

```vanillang
opaque Entity {
    static native func fromSelector(selector: Selector) -> Entity?

    native func isAlive() -> bool
}

opaque Player extends Entity {
    static native func fromSelector(selector: Selector) -> Player?

    native func getHealth() -> int
}
```

## 类型别名声明

使用 `type` 关键字声明一个类型别名，基本语法如下：

```text
type 类型别名[<泛型参数列表>] [where 泛型约束] = 原类型注解
```

类型别名声明有以下特性：

- 只能位于模块顶层
- 支持泛型参数与泛型约束

这些都是合法的类型别名声明：

```vanillang
type NamespaceId = String
type Box<T> = T
```

## 成员属性声明

使用以下基本语法在类中声明一个成员属性：

```text
[static [const]] 属性名[: 类型注解] [= 初始化表达式]
```

成员属性声明有以下特性：

- 只能位于类声明和枚举声明（仅限静态属性）中
- 实例属性不能拥有初始化表达式，必须在构造器中使用赋值语句显式初始化
- 静态属性必须拥有初始化表达式，类型已知时可以省略类型注解
- 静态属性可以是常量，此时要使用 `const` 修饰，初始化表达式只能是编译时可求值的字面量
- 常量静态成员的类型和使用常量声明语法定义的常量一样，只能是基本类型和 `vanillang.types.String`

这些都是合法的成员属性声明：

```vanillang
id: int
name: String

static foo: Foo = Foo(expr)
static const MAX_ENTITIES = 20
```

## 运算符重载声明

使用关键字 `op` 声明一个运算符重载，基本语法如下：

```text
[native] op 可重载运算符符号([参数: 参数类型注解]*) [-> 返回类型注解] [函数体]
```

运算符重载声明有以下特性：

- 可以位于类、接口、枚举和不透明类型中
- 返回值能被推断时，可以省略
- 不支持泛型参数与泛型约束
- 可以使用 `native` 修饰符将实现声明为原生的，此时不允许包含函数体
- 接口中的函数声明不能带有实现，也无法使用 `native` 关键字
- 带有隐含的 `this` 参数，用户无需也不能显式声明，编译器会为它分配第一个参数位
- 重写基类运算符或实现接口运算符后，该运算符重载自动多态，子类或实现类型中需要显式使用 `override` 标注
- 运算符签名由运算符名和参数列表唯一确定
- 支持声明多个同名运算符重载，具有相同运算符名、不同签名的运算符重载可以共存

这些都是合法的运算符重载：

```vanillang
op +(other: String) -> String
native op [](index: int) -> T?
```

## 构造器声明

使用 `init` 关键字声明一个构造器，基本语法如下：

```text
init([参数: 参数类型注解]*) 函数体
```

构造器声明有以下特性：

- 只能位于类中
- 没有返回类型
- 不支持泛型参数与泛型约束
- 带有隐含的 `this` 参数，用户无需也不能显式声明，编译器会为它分配第一个参数位
- 构造器签名由参数列表唯一确定
- 支持声明多个构造器，具有不同签名的构造器可以共存
- 基类、实例成员属性必须在构造器的函数体开头显式初始化
- 在函数体中使用 `super` 关键字初始化基类，使用赋值语法初始化实例成员属性

这些都是合法的构造器声明

```vanillang
init(id: int, name: String) {
    super()
    this.id = id
    this.name = name
}

init() {}
```

---

**附：Vanillang 声明文法产生式**

```ebnf
TopIdentifierDeclaration ::= [Metadata] ['private'] (VariableDeclaration | ConstantDeclaration | FunctionDeclaration | TypeDeclaration);

ImportDeclaration ::= 'import' ImportPath;
ImportPath ::= Identifier ('.' Identifier)* ['.' '*' | 'as' Identifier | '{' ImportPathList '}'];
ImportPathList ::= ImportPathItem (',' ImportPathItem)*;
ImportPathItem ::= 'self' ['as' Identifier] | ImportPath | '*';

VariableDeclaration ::= LetPrimary '=' Expression;
LetPrimary ::= 'let' Identifier [':' Type];

ConstantDeclaration ::= ConstPrimary '=' Literal;
ConstPrimary ::= 'const' Identifier [':' Type];

FunctionDeclaration ::= RegularFunctionDeclaration | NativeFunctionDeclaration;
RegularFunctionDeclaration ::= FunctionHeader [GenericConstraint] FunctionBody;
NativeFunctionDeclaration ::= 'native' FunctionHeader [GenericConstraint];
FunctionHeader ::= 'func' Identifier ['<' GenericParameterList '>'] '(' [ParameterList] ')' ['->' (Type | 'void')];
FunctionBody ::= '{' Statement* '};
ParameterList ::= Parameter (',' Parameter)*;
Parameter ::= Identifier ':' Type;

TypeDeclaration ::= ClassDeclaration | InterfaceDeclaration | EnumDeclaration | OpaqueDeclaration | TypeAliasDeclaration;

ClassDeclaration ::= ['final'] 'class' Identifier ['<' GenericParameterList '>'] ['extends' Type] ['implements' Type (',' Type)*] [GenericConstraint] ClassBody;
ClassBody ::= '{' ClassMember* '}';
ClassMember ::= [Metadata] ['public' | `protected` | 'private'] (Property | Method | Constructor | Operator);
Property ::= ['static' ['const']] Identifier [':' Type] ['=' Expression];
Method ::=  ['static' | 'override'] FunctionDeclaration;
Constructor ::= 'init' '(' [ParameterList] ')' FunctionBody;
Operator ::=  ['override'] OperatorOverloading;
OperatorOverloading ::= RegularOperatorOverloading | NativeOperatorOverloading;
RegularOperatorOverloading ::= OperatorOverloadingHeader FunctionBody;
NativeOperatorOverloading ::= 'native' OperatorOverloadingHeader;
OperatorOverloadingHeader ::= `op` OverloadableOperator '(' [ParameterList] ')' ['->' (Type | 'void')];

InterfaceDeclaration ::= 'interface' Identifier ['<' GenericParameterList '>'] [GenericConstraint] InterfaceBody;
InterfaceBody ::= '{' InterfaceMember* '}';
InterfaceMember ::= [Metadata] (FunctionHeader | OperatorOverloadingHeader)

EnumDeclaration ::= 'enum' Identifier ['<' GenericParameterList '>'] [GenericConstraint] EnumBody;
EnumBody ::= '{' (EnumMember | EnumProperty | EnumMethod | EnumOperator)* '}';
EnumMember ::= [Metadata] Identifier ['(' [EnumAssociatedValueList] ')'];
EnumAssociatedValueList ::= EnumAssociatedValue (',' EnumAssociatedValue)*;
EnumAssociatedValue ::= Identifier ':' Type;
EnumProperty ::= [Metadata] ['public' | 'private'] 'static' ['const'] Identifier [':' Type] ['=' Expression];
EnumMethod ::= [Metadata] ['public' | 'private'] ['static'] FunctionDeclaration;
EnumOperator ::= [Metadata] ['public' | 'private'] OperatorOverloading

OpaqueDeclaration ::= 'opaque' Identifier ['<' GenericParameterList '>'] ['extends' Type] ['implements' Type (',' Type)*] [GenericConstraint] OpaqueBody;
OpaqueBody ::= '{' OpaqueMember* '}'
OpaqueMember ::= [Metadata] ['public' | `protected` | 'private'] (OpaqueMethod | OpaqueOperator);
OpaqueMethod ::= ['static' | 'override'] FunctionDeclaration;
OpaqueOperator ::= ['override'] OperatorOverloading;

TypeAliasDeclaration ::= 'type' Identifier ['<' GenericParameterList '>'] [GenericConstraint] '=' Type;
```
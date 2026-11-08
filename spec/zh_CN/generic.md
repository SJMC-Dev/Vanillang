# Vanillang 泛型系统

泛型是一种与具体类型无关的类型参数，它用于帮助开发者编写不关心具体类型的代码。Vanillang 支持泛型系统，且通过泛型约束来实现编译时检查。

## 泛型函数与泛型类型

泛型系统可以作用于函数与类型，其中泛型函数中的泛型参数在调用时被实例化；泛型类型中的泛型参数在需要表示一个具体类型的值时被实例化。
泛型参数列表的基本语法如下：

```text
<参数1[，参数2，...，参数n]>
```

开发者可以声明一个或多个泛型参数，每个参数必须是一个合法的标识符。

## 泛型约束

为了实现编译时泛型检查，减少代码在运行时发生的错误，Vanillang 支持泛型约束，可以通过使用 `where` 关键字来定义泛型约束。
泛型约束有 4 个不同的类别，分别为非空、基类继承、接口实现和运算符包含，使用关键字 `notnone`、`extends`、`implements` 和 `hasop` 来表示。

注：

- 一个泛型参数只能有一个 `notnone` 和 `extends` 约束，可以有多个 `implements` 和 `hasop` 约束

这些都是合法的泛型约束：

```vanillang
T notnone
T extends Array<T>
T implements Iterable<T>
T hasop <=>(T) -> Ordering?
```

编译器规定：对未进行约束的泛型类型的值进行运算或调用成员方法，会报编译错误。

---

**附：Vanillang 泛型系统文法产生式**

```ebnf
GenericParameterList ::= Identifier (',' Identifier)*;
GenericArgumentList ::= Type (',' Type)*;
GenericConstraint ::= 'where' ConstraintClauseList;
ConstraintClauseList ::= ConstraintClause (',' ConstraintClause)*;
ConstraintClause ::= Identifier ('notnone' | ('extends' | 'implements') Type | 'hasop' OperatorRequirement);
OperatorRequirement ::= OverloadableOperator '(' [TypeList] ')' '->' ReturnType;
```

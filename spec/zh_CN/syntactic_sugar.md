# Vanillang 语法糖

Vanillang 语法糖是指一些特殊的语法形式，它们在编译时会被转换成更基本的语法结构，以提供更简洁、易读或更强大的表达能力。

## none 关键字

使用 `none` 关键字可以直接用于实例化一个 `vanillang.typesystem.Optional\<T\>` 类型的值，表示一个空值：

```vanillang
let value: Optional<string> = none
```

## 可选类型语法

通过在类型注解后面添加 `?`，可以将一个类型 `T` 标记为可选类型，等价于 `vanillang.typesystem.Optional\<T\>`，实际类型也为 `vanillang.typesystem.Optional\<T\>`：

```vanillang
# 等价于 let name: vanillang.typesystem.Optional<string> = none
let name: string? = none
```
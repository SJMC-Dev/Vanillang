# Vanillang 关键字

Vanillang 关键字具有特殊用途，不可作用标识符，下面的表格列出了已经使用的关键字及其用法示例：

## 变量声明

| 关键字  | 用途               | 示例                                                                           |
| ------- | ------------------ | ------------------------------------------------------------------------------ |
| `let`   | 声明一个变量       | `let playersSelector = @a`、`let marker: Entity = @n[type="minecraft:marker"]` |
| `const` | 声明一个编译时常量 | `const PI = 3.14`                                                              |


## 函数与控制流

| 关键字     | 用途                                             | 示例                                                                                      |
| ---------- | ------------------------------------------------ | ----------------------------------------------------------------------------------------- |
| `func`     | 声明一个函数                                     | `func getPlayerName(player: Player) -> String`                                            |
| `return`   | 从函数中返回值或提前结束函数                     | `return`、`return 0`、`return @a[gamemode="creative"]`                                    |
| `if`       | 条件语句开头                                     | `if (player.inDimension("minecraft:overworld"))`、`if (num == 1)`                         |
| `else`     | 表示上一条件不满足                               | `else return 0`、`else if (world.checkBlockState([-11, 63, 2], "minecraft:grass_block"))` |
| `for`      | 变量循环控制                                     | `for (let i in 0..99)`、`for (let player: Player in players)`                             |
| `while`    | 条件循环控制                                     | `while (n >= 1)`、`while (players.size() != 3)`、`while (let entity = @n[sort="random"])` |
| `label`    | 定义循环控制流的标签                             | `label outerLoop`、`label innerLoop`                                                      |
| `break`    | 跳出当前循环或跳出循环到标签处                   | `break`、`break outerLoop`                                                                |
| `continue` | 跳出本轮循环迭代                                 | `continue`                                                                                |
| `switch`   | 表示选择语句                                     | `switch (num)`、`switch (let a = call(b))`                                                |
| `case`     | 在 `switch` 语句中匹配编译时常量值或子类的实例   | `case 1`、`case "Vanillang"`、`case Ok`、`case Error`                                     |
| `default`  | 在 `switch` 语句中匹配所有未被 `case` 匹配的情况 | `default`                                                                                 |
| `when`     | 匹配子类实例时提供额外条件                       | `case Ok when foo.value == 1`                                                             |
| `context`  | 表示当前函数的命令上下文常量，Dict 类型          | `context={as: @p, at: @s}`                                                                |
| `try`      | 用于得到空值或错误时直接返回                     | `let player: Player = try Player.fromSelector(@r[tag="candidate"])`                       |
| `native`   | 表示函数是原生函数，没有 Vanillang 实现          | `native func getData() -> Dict?`                                                          |
| `callee`   | 表示当前函数本身                                 | `return n * callee(n - 1)`                                                                |
| `in`       | 遍历可迭代对象                                   | `for (let i in 0..9)`                                                                     |
| `reload`   | 重新加载当前数据包，必须在函数体内使用           | `func reloadDataPack() -> void { reload }`                                                |
| `init`     | 声明类的构造函数                                 | `init(value: int) { this.value = value }`                                                 |

## 基本类型

| 关键字   | 用途                                 | 示例                                                                 |
| -------- | ------------------------------------ | -------------------------------------------------------------------- |
| `byte`   | 表示有符号字节型                     | `let flag: byte = 1B`、`let initial: byte = 'V'`                     |
| `ubyte`  | 表示无符号字节型                     | `func setAlpha(alpha: ubyte) -> void`                                |
| `short`  | 表示有符号短整型                     | `let offset: short = 256s`                                           |
| `ushort` | 表示无符号短整型                     | `private durability: ushort`                                         |
| `int`    | 表示有符号整型                       | `let score: int = 0`、`func add(a: int, b: int) -> int`              |
| `uint`   | 表示无符号整型                       | `private elapsedTicks: uint`                                         |
| `long`   | 表示有符号长整型                     | `let worldTime: long = 1000000000000L`                               |
| `ulong`  | 表示无符号长整型                     | `func hash(text: String) -> ulong`                                   |
| `float`  | 表示单精度浮点型                     | `let scale: float = 1.5`                                             |
| `double` | 表示双精度浮点型                     | `let distance: double = 3.5d`、`let y: double = 64.0D`               |
| `bool`   | 表示布尔型，取值为 `true` 或 `false` | `let isNight: bool = false`、`func isOnline(player: Player) -> bool` |

## 类型系统与面向对象

| 关键字       | 用途                                                     | 示例                                                                                    |
| ------------ | -------------------------------------------------------- | --------------------------------------------------------------------------------------- |
| `class`      | 声明一个类                                               | `class GameStatus`、`class EntityDetector<T>`                                           |
| `interface`  | 声明一个接口                                             | `interface Data`                                                                        |
| `type`       | 重命名一个类型                                           | `type NamespaceId = String`                                                             |
| `enum`       | 声明一个枚举类型                                         | `enum Color`、`enum Gamemode`                                                           |
| `extends`    | 继承一个基类，或表示泛型必须是一个类的子类               | `class Player extends Entity`、`where T extends Array<T>`                               |
| `implements` | 实现一个或多个接口，或表示泛型必须实现某个接口           | `class Text implements Data`, `where T implements Iterable<T>`                          |
| `this`       | 指代当前实例对象                                         | `this.id = id`                                                                          |
| `super`      | 指代上一层基类                                           | `super.getName()`                                                                       |
| `private`    | 声明私有成员                                             | `private id: String`、`private func stepOnce() -> void`                                 |
| `protected`  | 声明保护成员                                             | `protected name: String`、`protected func setBaseClassPropertyValue() -> void`          |
| `public`     | 声明公有成员，可省略                                     | `public func getName() -> String`                                                       |
| `static`     | 标识一个静态成员                                         | `static gameStatus: String`                                                             |
| `instanceof` | 判断一个实例是否为某个类或其子类，或为某个接口的实现类   | `if (player instanceof Entity)`                                                         |
| `final`      | 声明一个类不能被继承                                     | `final class World`                                                                     |
| `override`   | 标识一个成员函数覆盖了基类的同名函数                     | `override func getName() -> String`                                                     |
| `none`       | 实例化一个表示空值的 `vanillang.types.Optional<T>`       | `let optionalPlayer: Player? = none`                                                    |
| `op`         | 声明一个运算符重载函数                                   | `op +(other: Vec3) -> Vec3`                                                             |
| `where`      | 表示泛型约束的开始                                       | `where T extends Array<T>`、`where T implements Iterable<T>`、`where T hasop +(T) -> T` |
| `hasop`      | 表示泛型必须对某个运算符可用                             | `where T hasop <=>(T) -> Ordering?`                                                     |
| `notnone`    | 表示泛型类型非空，即不能是 `vanillang.types.Optional<T>` | `where T notnone`                                                                       |
| `void`       | 表示函数没有返回值                                       | `func onLoad() -> void`                                                                 |

## 字面量关键字

| 关键字  | 用途       | 示例                             |
| ------- | ---------- | -------------------------------- |
| `true`  | 字面值为真 | `let areAllPlayersOnline = true` |
| `false` | 字面值为假 | `return false`                   |

## 模块关键字

| 关键字   | 用途                                               | 示例                                                         |
| -------- | -------------------------------------------------- | ------------------------------------------------------------ |
| `import` | 导入一个标识符、模块或包                           | `import vanillang.math`、`import vanillang.ui.TextDisplayUi` |
| `as`     | 重命名一个导入的标识符，不能与 * 搭配使用          | `import vanillang.ui.ItemDisplayUi as Button`                |
| `*`      | 导入一个模块下的所有顶层标识符，不能与 as 搭配使用 | `import vanillang.ui.*`                                      |
| `self`   | 表示当前包或模块                                   | `import vanillang.ui.{self, ItemDisplayUi, TextDisplayUi}`   |

# 元数据关键字

| 关键字     | 用途                                                                                                                                                   | 示例                               |
| ---------- | ------------------------------------------------------------------------------------------------------------------------------------------------------ | ---------------------------------- |
| `metadata` | 定义类、变量、函数等的元数据，供编译器识别并影响编译器行为，目前支持的元数据项为 `deprecated`、`experimental`、`nowarnings` 和 `gameversion <version>` | `metadata(deprecated, nowarnings)` |
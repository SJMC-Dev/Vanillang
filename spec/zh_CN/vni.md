# Vanillang 模块接口文件

Vanillang 模块接口文件以 `.vni` 作为后缀，其本质是一个 JSON 文件，用于存储模块中的标识符相关信息。
- 注意：`private` 修饰的标识符虽然不能导入，但仍然应该出现在模块接口文件中。

## 格式

在 Vanillang 模块接口文件中，标识符是一个键值对，其键名为该标识符在作用域内的唯一名字，值为一个对象，该对象有如下成员属性：

| 属性     | 类型   | 解释                                                                                                                                                                                               |
| -------- | ------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| category | string | 该标识符的类别，取值可以为 `let`、`func`、`class`、`interface`、`enum`、`typealias`、`property`、`method`、`parameter`、`enummember`、`enumvalue` 、`constructor`、`operator`                      |
| metadata | object | 元数据信息，仅当标识符具有元数据且不为 `enummember`、`enumvalue`、`parameter` 时存在，键名为元数据项，值为 string 类型或 null，对于需要提供元数据值的元数据项，其值为 string 类型，否则其值为 null |

根据 `category` 值的不同，其余属性的值也有所不同。                      |

### let 类别

| 属性           | 类型   | 解释                                       |
| -------------- | ------ | ------------------------------------------ |
| type           | string | 该变量的类型，格式与源代码的类型语法一致   |
| accessModifier | string | 访问修饰符，取值可以为 `public`、`private` |

### func 类别

| 属性           | 类型    | 解释                                                 |
| -------------- | ------- | ---------------------------------------------------- |
| name           | string  | 函数名，指源代码中声明的名字，不是内部生成的唯一名称 |
| returnType     | string  | 函数返回值类型，格式与源代码的类型语法一致           |
| parameters     | object  | 函数参数，其属性应该是 `parameter` 类别的对象        |
| native         | boolean | 是否为原生函数                                       |
| accessModifier | string  | 访问修饰符，取值可以为 `public`、`private`           |

### class 类别

| 属性                  | 类型          | 解释                                       |
| --------------------- | ------------- | ------------------------------------------ |
| baseClass             | string        | 基类的名称，若无基类则为 null              |
| implementedInterfaces | array<string> | 实现的接口列表，若无实现接口则为 []        |
| final                 | boolean       | 是否为 final 类                            |
| genericParameters     | array<string> | 泛型参数列表                               |
| properties            | object        | 成员属性，由 `property` 类别的对象组成     |
| methods               | object        | 成员方法，由 `method` 类别的对象组成       |
| constructors          | object        | 构造器，由 `constructor` 类别的对象组成    |
| operators             | object        | 运算符重载，由 `operator` 类别的对象组成   |
| accessModifier        | string        | 访问修饰符，取值可以为 `public`、`private` |

### interface 类别

| 属性              | 类型          | 解释                                       |
| ----------------- | ------------- | ------------------------------------------ |
| genericParameters | array<string> | 泛型参数列表                               |
| methods           | object        | 成员方法，由 `method` 类别的对象组成       |
| operators         | object        | 运算符重载，由 `operator` 类别的对象组成   |
| accessModifier    | string        | 访问修饰符，取值可以为 `public`、`private` |

### enum 类别

| 属性              | 类型          | 解释                                       |
| ----------------- | ------------- | ------------------------------------------ |
| genericParameters | array<string> | 泛型参数列表                               |
| members           | object        | 枚举成员，由 `enummember` 类别的对象组成   |
| accessModifier    | string        | 访问修饰符，取值可以为 `public`、`private` |

### typealias 类别

| 属性              | 类型          | 解释                                       |
| ----------------- | ------------- | ------------------------------------------ |
| genericParameters | array<string> | 泛型参数列表                               |
| originalType      | string        | 原始类型，格式与源代码的类型语法一致       |
| accessModifier    | string        | 访问修饰符，取值可以为 `public`、`private` |

### property 类别

| 属性           | 类型    | 解释                                                    |
| -------------- | ------- | ------------------------------------------------------- |
| type           | string  | 属性类型，格式与源代码的类型语法一致                    |
| static         | boolean | 是否为静态属性                                          |
| accessModifier | string  | 访问修饰符，取值可以为 `public`、`protected`、`private` |

### method 类别

| 属性           | 类型    | 解释                                                    |
| -------------- | ------- | ------------------------------------------------------- |
| name           | string  | 方法名，指源代码中声明的名字，不是内部生成的唯一名称    |
| returnType     | string  | 方法返回值类型，格式与源代码的类型语法一致              |
| parameters     | object  | 方法参数，其属性应该是 `parameter` 类别的对象           |
| static         | boolean | 是否为静态方法                                          |
| native         | boolean | 是否为原生方法                                          |
| accessModifier | string  | 访问修饰符，取值可以为 `public`、`protected`、`private` |

### parameter 类别

| 属性 | 类型   | 解释                                 |
| ---- | ------ | ------------------------------------ |
| type | string | 参数类型，格式与源代码的类型语法一致 |

### enummember 类别

| 属性            | 类型   | 解释                                                  |
| --------------- | ------ | ----------------------------------------------------- |
| assocatedValues | object | 枚举成员的关联值，其属性应该是 `enumvalue` 类别的对象 |

### enumvalue 类别

| 属性 | 类型   | 解释                                         |
| ---- | ------ | -------------------------------------------- |
| type | string | 枚举关联值的类型，格式与源代码的类型语法一致 |

### constructor 类别

| 属性           | 类型   | 解释                                                    |
| -------------- | ------ | ------------------------------------------------------- |
| parameters     | object | 构造器参数，其属性应该是 `parameter` 类别的对象         |
| accessModifier | string | 访问修饰符，取值可以为 `public`、`protected`、`private` |

### operator 类别

| 属性           | 类型    | 解释                                                    |
| -------------- | ------- | ------------------------------------------------------- |
| returnType     | string  | 返回值类型，格式与源代码的类型语法一致                  |
| parameters     | object  | 参数，其属性应该是 `parameter` 类别的对象               |
| accessModifier | string  | 访问修饰符，取值可以为 `public`、`protected`、`private` |
| native         | boolean | 是否为原生重载实现                                      |
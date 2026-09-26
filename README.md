# Windows Console App

> 一个基于Windows API 纯手工实现的控制台UI渲染库。

作者：mistelin37

## 1.简介

本项目是一个基于c++开发的简易控制台UI框架，旨在为控制台应用的UI界面设计提供框架。
项目核心调用了`<windows.h>`库，通过获取标准输入输出句柄
`GetStdHandle(STD_INPUT_HANDLE) `和`GetStdHandle(STD_OUTPUT_HANDLE)`与 Windows 控制台 API 进行交互。

因此本项目依赖Windows控制台。

## 2.代码结构

本项目分为两个主要部分：头文件和示例程序。

在头文件部分，我构造了`Element`基类，并实现了`Window`，`Label`，`Button`，`Picture`四个子类，用于存储窗体，文本，按钮，图片四种控件。

在示例程序部分，我写了一个简单的程序，在控制台上渲染了一块120*30字符的区域（视为一个大窗口对象），包括一个小窗口`App1`和一个底部栏。其中：
- 底部栏包含一个控制小窗口隐藏/显示的按钮
- 小窗口可按住顶部高亮部分拖曳，内部静态展示了`Label`控件和`Picture`控件

此外，示例程序中包含了还未封装的鼠标移动和点击事件处理逻辑。

## 3.使用说明

本项目包括5个头文件：
```
ui.h: Element基类与控制台初始化函数的实现，是其余头文件的前置依赖。
Window.h: Window子类的实现。
Label.h: Label子类的实现。
Button.h: Button子类的实现。
Picture.h: Picture子类的实现。
elements.h: 一个声明了上述四个子类的头文件，旨在方便使用
```
因此在使用时，只需包含头文件`ui.h`和`elements.h`.

### 3.1.类的属性和方法

由于本项目为实验性质，未对类的属性和方法进行严格的可访问性处理，大部分属性与方法均为`public`，使用时请注意数据安全。

#### 3.1.1 Element基类

**属性：**

| 名称 | 类型 | 作用 |
|:----:|:----:|:----|
| col | SHORT | 控件左上角的列坐标（字符数） |
| row | SHORT | 控件左上角的行坐标（字符数） |
| width | SHORT | 控件宽度（字符数） |
| height | SHORT | 控件高度（字符数） |
| render | bool | 是否渲染 |
| father | *Element | 父控件指针 |
| son | std::vector<std::unique_ptr<Element>> | 子控件指针列表 |
| content | std::vector\<CHAR_INFO\> | 控件自身的字符缓冲，尺寸为 `width * height` |
| format | WORD | 默认字符属性（颜色），初始为 `0x07`（黑底白字） |

**成员函数：**

| 名称 | 签名 | 说明 |
|:----:|:----:|:----|
| 构造函数 | `Element(SHORT _col, SHORT _row, SHORT _width, SHORT _height)` | 初始化位置与尺寸，并将 `content` 填充为空格、属性 `0x07` |
| At | `CHAR_INFO* At(SHORT _col, SHORT _row)` | 返回控件内局部坐标 `(_col, _row)` 处字符缓冲的指针；越界时打印提示并返回 `nullptr` |
| Build | `void Build()` | 遍历 `son`，对 `render == true` 的子控件调用其 `Render(this)`，递归地完成一次子控件组合 |
| visible | `bool visible(SHORT _col, SHORT _row)` | 判断局部坐标 `(_col, _row)` 是否落在控件矩形范围内 |
| hittest | `Element* hittest(SHORT _col, SHORT _row)` | 从最上层子控件开始递归查找命中的控件；无子控件命中时返回 `this` |
| Render | `virtual void Render(Element* target) = 0` | 纯虚函数，子类实现具体绘制逻辑，将内容写入 `target` |
| OnHover | `virtual bool OnHover()` | 鼠标悬停时的回调，默认返回 `false`，子类可重写 |
| Leave | `virtual bool Leave()` | 鼠标离开时的回调，默认返回 `false`，子类可重写 |
| 析构 | `virtual ~Element() = default` | 虚析构，保证通过基类指针删除子类对象时正确释放 |

**注：**

- `col` / `row` 是**相对于父控件**的局部坐标，不是屏幕绝对坐标。
- `hittest` 与 `visible` ， `visible` 方法内部在递归调用时进行坐标转换。
- `At` 接收的是**控件自身的局部坐标**，范围是 `[0, width) × [0, height)`。
- `hittest` 从 `son` 的**末尾向前**遍历，效果为同一个子控件列表中顺序靠后的渲染在上层，同时遮挡下层的碰撞检测。

---

#### Mouse类

Mouse是鼠标状态类，用于鼠标点击相关事件的参数传入。

| 名称 | 类型 | 说明 |
|:----:|:----:|:----|
| x | SHORT | 鼠标列坐标 |
| y | SHORT | 鼠标行坐标 |
| button | int | 鼠标按键状态 |
| 构造函数 | `Mouse(SHORT _x, SHORT _y, int _button)` | 初始化鼠标位置与按键 |

---

#### InitConsole函数

`void InitConsole(const wchar_t* title)`

初始化控制台环境：

1. 清屏
2. 获取标准输出/输入句柄
3. 设置窗口标题
4. 隐藏光标
5. 开启鼠标输入（`ENABLE_MOUSE_INPUT`），关闭快速编辑模式（`ENABLE_QUICK_EDIT_MODE`），并启用扩展标志（`ENABLE_EXTENDED_FLAGS`）

---

#### Window子类

`Window` 继承自 `Element`，是带边框、背景和可选标题栏的容器控件。它既能作为普通窗口显示，也能作为其他控件的父容器，支持通过标题栏拖动窗口，并提供子控件的创建与销毁接口。

**属性：**

| 名称 | 类型 | 说明 |
|:----:|:----:|:----|
| border | bool[4] | 四条边框的开关，顺序为**右上左下** |
| border_edge | CHAR_INFO | 边框边线的字符与属性 |
| border_corner | CHAR_INFO | 边框四角的字符与属性 |

**成员函数：**

| 名称 | 签名 | 说明 |
|:----:|:----:|:----|
| 构造函数 | `Window(SHORT _col, SHORT _row, SHORT _width, SHORT _height, std::string _title = "")` | 初始化窗口；若 `_title` 非空，则在顶部创建一个占满宽度的 `Button` 作为标题栏与拖动柄。 |
| SetBorder | `void SetBorder(bool _border0, bool _border1, bool _border2, bool _border3)` | 设置四条边框的开关，顺序为右上左下 |
| SetBorderStyle | `void SetBorderStyle(CHAR_INFO _border_edge, CHAR_INFO _border_corner)` | 设置边线与四角的字符样式 |
| SetBg | `void SetBg(WORD _format)` | 设置窗口背景的字符属性（颜色） |
| FlushAll | `void FlushAll()` | 清空 `content`、调用 `Render(nullptr)` 完成自身绘制，再用 `WriteConsoleOutputW` 将内容写入控制台左上角区域（设计上仅根窗口控件调用，用于刷新屏幕） |
| Render | `void Render(Element* target)` | 绘制背景 → 绘制子控件 → 绘制边框；若 `target` 非空，则将自身内容按 `(col, row)` 偏移拷贝到父控件缓冲 |
| CreateSon | `template <typename T, typename... Args> T* CreateSon(Args&&... args)` | 创建子控件，设置其 `father` 为当前窗口，加入 `son` 并返回裸指针 |
| DestorySon | `void DestorySon(int _id)` | 按下标从 `son` 中移除子控件 |

**私有成员：**

私有属性均为拖动柄实现功能时所需的中间变量，私有方法为渲染背景和边框的具体实现。

| 名称 | 类型 | 说明 |
|:----:|:----:|:----|
| start_col / start_row | SHORT | 拖动开始时窗口的列 / 行坐标 |
| start_width / start_height | SHORT | 拖动开始时的宽 / 高（当前未使用） |
| start_x / start_y | SHORT | 拖动开始时鼠标的列 / 行坐标 |
| FlushBg | `void FlushBg()` | 将 `content` 全部字符的属性设为 `format` |
| FlushBorder | `void FlushBorder()` | 根据 `border[4]` 逐条绘制边线与四角 |

**有关标题栏与拖动：**

构造函数在 `_title` 非空时会创建一个覆盖顶行的 `Button`：

- 格式设为 `0x70`（白底黑字），`label->aligment = 0`，内容为 `"  " + _title`。
- `on_click`：记录窗口起始坐标与鼠标起始坐标。
- `hold`：在按住期间，按鼠标位移量更新窗口的 `col` / `row`，实现拖动。

因此拖动逻辑依赖 `Button` 的 `on_click` 与 `hold` 回调，以及 `Mouse` 类提供的 `x` / `y`。

**关于渲染流程：**

```
Render(target)
 ├─ FlushBg()          设置背景属性
 ├─ Build()            遍历子控件调用其 Render(this)
 ├─ FlushBorder()      绘制边框
 └─ if target != null  将自身内容偏移拷贝到父缓冲
```

拷贝时对每个字符判断 `(col + i, row + j)` 是否落在 `target` 范围内，超出部分被裁剪，从而避免越画出父控件边界。

**关于边框绘制：**

为保证横竖边框在视觉上宽度一致，横边框占用一行字符，竖边框占用两行字符。

---

#### Label

`Label` 继承自 `Element`，是用于显示文本的控件。它将字符串按空格拆分为单词后进行**自动换行**排版，并支持水平对齐、垂直对齐与透明背景。

## 属性

| 名称 | 类型 | 说明 |
|:----:|:----:|:----|
| content | std::string | 要显示的文本内容 |
| aligment | int | 水平对齐：0 左、1 居中、2 右 |
| v_aligment | int | 垂直对齐：0 上、1 居中、2 下 |
| transparent | bool | 是否透明背景，为 `true` 时使用 `target->format` 作为字符属性 |

## 成员函数

| 名称 | 签名 | 说明 |
|:----:|:----:|:----|
| 构造函数 | `Label(SHORT _col, SHORT _row, SHORT _width, SHORT _height)` | 初始化位置与尺寸 |
| Render | `void Render(Element* target) override` | 将 `content` 按单词换行后绘制到 `target` 的对应位置 |

**渲染流程：**

```
Render(target)
 ├─ 按空格拆分 content 为单词列表 words
 ├─ 逐词累加，超过 width 则换行，生成 lines
 ├─ 根据 v_aligment 计算起始 y 偏移
 ├─ 根据 aligment 计算每行起始 x 偏移
 └─ 逐字符写入 target，超出范围则跳过
```

**关于透明背景：**

当 `transparent == true` 时，`format` 被设为 `target->format`，即继承父控件的字符属性；否则使用 `Label` 自身的 `format`。

**关于边界裁剪：**

每次写入前都检查 `(col + x, row + y)` 是否落在 `target` 的宽高范围内，越界则只递增 `x` 不写入，因此文本超出控件或父控件范围时会被安全裁剪，不会越界访问缓冲。

**注：**


- 处理时暂未考虑中文与宽字符，使用这些字符可能产生换行错误或者乱码。
- `i.size() > width` 的单词会强行中断，长单词无法强制断行显示。
- 渲染时最终图像被绘制在父控件缓冲中，因此假设 `target` 非空，因此不应作为根控件直接渲染`Label`对象。

---

#### Button类

`Button` 继承自 `Element`，是可点击的按钮控件。它内置一个 `Label` 用于显示文字，支持四种视觉状态（正常、悬停、按下、禁用），并通过三个回调向外暴露点击、按住与松开事件。

**属性：**

| 名称 | 类型 | 说明 |
|:----:|:----:|:----|
| state | int | 当前状态：0 正常、1 悬停、2 按下、3 禁用 |
| state_format | WORD[4] | 四种状态对应的字符属性，默认 `{0x70, 0xF0, 0x07, 0x80}` |
| enabled | bool | 是否启用 |
| focused | bool | 是否获得焦点 |
| label | std::unique_ptr\<Label\> | 内置标签，负责显示按钮文字 |
| on_click | std::function\<void(Mouse)\> | 点击时的回调 |
| hold | std::function\<void(Mouse)\> | 按住时的回调 |
| off_click | std::function\<void()\> | 松开时的回调 |

**成员函数：**

| 名称 | 签名 | 说明 |
|:----:|:----:|:----|
| 构造函数 | `Button(SHORT _col, SHORT _row, SHORT _width, SHORT _height)` | 初始化位置尺寸，并创建 `label`，默认水平居中、垂直居中 |
| SetFormat | `void SetFormat(WORD _normal, WORD _hover, WORD _pressed, WORD _disabled)` | 设置四种状态的字符属性 |
| FlushBg | `void FlushBg()` | 将 `content` 全部字符的属性设为当前 `format` |
| Render | `void Render(Element* target)` | 按 `state` 取属性 → 刷背景 → 渲染 `label` → 拷贝到 `target` |
| OnHover | `bool OnHover()` | 悬停时把状态置为 1（悬停），禁用时忽略 |
| OnClick | `bool OnClick(Mouse event)` | 触发 `on_click`，状态置为 2（按下），禁用时忽略 |
| Hold | `bool Hold(Mouse event)` | 触发 `hold`，禁用时忽略 |
| OffClick | `bool OffClick()` | 触发 `off_click`，状态置为 1（悬停），禁用时忽略 |
| Leave | `bool Leave()` | 状态置为 0（正常），禁用时忽略 |

**状态机：**

```
         OnHover            OnClick
  Normal ───────► Hover ───────────► Hold
    ▲              ▲                  │
    │   Leave      │    OffClick      │
    └──────────────┴──────────────────┘
                 Disable（state=3，所有事件被忽略）
```


**渲染流程：**

```
Render(target)
 ├─ format = state_format[state]   按状态取属性
 ├─ FlushBg()                      刷背景属性
 ├─ label->Render(this)            把文字画到自身缓冲
 └─ if target != null              偏移拷贝到父控件
```

**默认样式：**

| 状态 | 属性值 | 效果 |
|:----:|:------:|:----|
| Normal | 0x70 | 白底黑字 |
| Hover | 0xF0 | 亮白底黑字 |
| Pressed | 0x07 | 黑底白字 |
| Disabled | 0x80 | 灰底黑字 |

**与Window类的关系：**

`Window` 的标题栏就是一个 `Button`：通过 `SetFormat(0x70,0x70,0x70,0x70)` 让四种状态外观一致，再用 `on_click` 记录起始坐标、`hold` 更新窗口位置，从而实现拖动。

**注意事项：**

- `state` 是按钮内部状态，外部若要禁用按钮，需把 `state` 置为 3（目前没有专门的 `SetEnabled` 接口）。

---

#### Picture

`Picture` 继承自 `Element`，是用于显示字符图形/图像的控件。它本身不做绘制逻辑，仅把自身 `content` 缓冲按坐标偏移拷贝到父控件，因此图像的填充需要外部直接写入 `content`。正因如此，Picture的 `content` 缓冲不会被内部逻辑修改，它是安全的。

**成员函数：**

| 名称 | 签名 | 说明 |
|:----:|:----:|:----|
| 构造函数 | `Picture(SHORT _col, SHORT _row, SHORT _width, SHORT _height, std::string _title = "")` | 初始化位置与尺寸；`_title` 参数被接收但未使用 |
| Render | `void Render(Element* target) override` | 将自身 `content` 按 `(col, row)` 偏移拷贝到 `target`，越界部分裁剪 |

**渲染流程：**

```
Render(target)
 └─ if target != null
     逐字符判断 (col + i, row + j) 是否在 target 范围内
       └─ 在范围内则写入 target->At(col + i, row + j)
```


**写入方式：**

由于 `Picture` 不提供加载图像或填色的接口，使用时应先通过 `Element::At(col, row)` 直接写入 `content`：

```cpp
Picture* pic = window->CreateSon<Picture>(0, 0, 10, 5);
*pic->At(2, 2) = CHAR_INFO{L'#', 0x0E};
```

之后父控件 `Build()` 时会调用 `pic->Render(this)`，把内容画到父控件上。

**注：**

- `content` 在构造时被填为空格、属性 `0x07`，若不手动写入则显示为空。

### 3.2.示例程序

main.cpp是一个例子，同时也实现了获取鼠标事件并分发给控件，由于鼠标事件处理部分尚未封装成库接口，main.cpp 中的这段代码目前是使用该 UI 库的必要模板，不能简单当作可丢弃的示例。

#### 界面结构：

```
scrn（全屏窗口，120×30，背景 0x80）
├─ window1（子窗口，2,2，20×26，标题 "App1"，背景 0x07）
│   ├─ label0（左对齐，垂直居中）
│   ├─ label1（水平居中，垂直居中）
│   ├─ label2（右对齐，垂直居中）
│   └─ pic0（2,16，20×10，绘制四个彩色 '#'）
└─ bottom（底部条，0,29，120×1，背景 0x70）
    └─ btn0（0,0，10×1，文字 "App1"）
```

#### 交互效果：

| 操作 | 效果 |
|:----:|:----|
| 悬停按钮 | 按钮进入悬停状态（颜色变化） |
| 按下按钮 | 按钮进入按下状态 |
| 松开按钮 | 触发 `off_click`，切换 `window1` 的显示与隐藏 |
| 拖动 `window1` 标题栏 | 窗口跟随鼠标移动 |


*如在使用中有任何疑问，建议，欢迎通过邮箱联系作者： mistelin@163.com*
/**
@mainpage Geometry Workbench

@section overview Обзор

Интерактивный стенд по компьютерной геометрии на C++17 и Qt 6.

@section highlights Возможности Doxygen в этом проекте

* `@mainpage` — эта страница становится домашней страницей HTML-документации.
* `@defgroup` и `@ingroup` — API разделён на предметные библиотеки.
* Graphviz (`HAVE_DOT = YES`) — строит графы классов, include-зависимостей и вызовов.
* `@example` — исходный пример появляется в специальном разделе Examples.
* пользовательские aliases `\\complexity`, `\\invariant`, `\\educational` — единый стиль методических заметок.
* XML-вывод (`GENERATE_XML`) можно использовать во внешнем генераторе или IDE.

@section example_section Пример API

Ниже подключён минимальный пример вычисления предиката поворота:

@example turn_example.cpp

@section quickstart Быстрый старт

```bash
cmake -S . -B build
cmake --build build --target docs
```

HTML находится в `build/docs/html/index.html`, XML — в `build/docs/xml/`.
*/

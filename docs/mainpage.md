@mainpage Geometry Workbench

@section overview Обзор

Интерактивный стенд по компьютерной геометрии на C++17 и Qt 6.

@section highlights Возможности Doxygen в этом проекте

* `@mainpage` — эта страница становится домашней страницей HTML-документации.
* `@defgroup` и `@ingroup` — API разделён на предметные библиотеки.
* Graphviz (`HAVE_DOT = YES`) — строит графы классов, зависимостей подключаемых файлов и вызовов.
* `@example` — исходный пример появляется в специальном разделе «Примеры».
* пользовательские псевдонимы `\\complexity`, `\\invariant`, `\\educational` — единый стиль методических заметок.
* `@pre`, `@post`, `@note`, `@warning`, `@see` и `@since` описывают контракт API прямо рядом с функцией.
* `@f$ ... @f$` и MathJax отображают формулы, а `@dot ... @enddot` строит схему конвейера точного предиката.
* `@todo` формирует отдельный список дальнейших улучшений из реальных комментариев к коду.
* XML-вывод (`GENERATE_XML`) можно использовать во внешнем генераторе или среде разработки.

Полный перечень возможностей и точек их использования: @ref doxy_features.

@section example_section Пример API

Ниже подключён минимальный пример вычисления предиката поворота:

@example turn_example.cpp

@section quickstart Быстрый старт

```bash
cmake -S . -B build
cmake --build build --target docs
```

HTML находится в `docs/site/html/index.html`, XML — в `docs/site/xml/`.

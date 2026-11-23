@page doxy_features Конспект возможностей Doxygen в проекте

@section doxy_intro Введение

В Geometry Workbench Doxygen — не отдельный текстовый отчёт, а часть исходного
кода и сборочного процесса. Комментарии в заголовочных файлах описывают API,
`Doxyfile` определяет представление результата, а готовый HTML хранится в
репозитории и публикуется на GitHub Pages.

Ниже перечислены **все возможности Doxygen, которые задействованы в проекте**,
и указано, где именно их искать. Пути даны относительно корня репозитория.

@section doxy_comments Комментарии и контракт API

| Возможность | Что делает | Где найти |
|---|---|---|
| Блочный комментарий `/** ... */` | Делает комментарий документацией Doxygen. | `src/lib/geometry/predicates.h`, `src/lib/bigint/exact_predicate.h` |
| `@brief` | Кратко описывает сущность; попадает в списки классов и функций. | Все публичные заголовки, например `src/apps/GeometryWorkbench/geometrycanvas.h` |
| `@param`, `@return` | Описывают входные параметры и результат. | `src/lib/geometry/segment_intersection.h`, `src/lib/bigint/exact_predicate.h` |
| `@pre`, `@post` | Фиксируют предусловия и гарантии после успешного вызова. | `src/lib/bigint/exact_predicate.h` |
| `@note` | Добавляет важное пояснение без статуса ошибки. | `src/lib/bezier/bezier.h`, `src/lib/geometry/predicates.h` |
| `@warning` | Выделяет численное или ресурсное ограничение. | `src/lib/bigint/exact_predicate.h`, `src/lib/geometry/predicates.h` |
| `@see` | Создаёт перекрёстную ссылку на связанную функцию. | `src/lib/bezier/bezier.h`, `src/lib/geometry/predicates.h` |
| `@since` | Показывает версию появления интерфейса. | `src/lib/bigint/exact_predicate.h` |
| `@par` | Создаёт именованный смысловой блок в описании. | `src/lib/geometry/predicates.h`, `src/lib/bezier/bezier.h` |

@section doxy_structure Структура API и навигация

| Возможность | Что делает | Где найти |
|---|---|---|
| `@mainpage` | Создаёт домашнюю страницу HTML-документации. | `docs/mainpage.md` |
| `@page` и `@section` | Создают отдельные страницы и разделы документации. | `docs/architecture.md`, этот файл |
| `@defgroup`, `@ingroup` | Собирают API в предметные группы. | `src/lib/geometry/convex_hull.h`, `src/lib/geometry/predicates.h`, `src/lib/bigint/bigint.h`, `src/lib/bezier/bezier.h` |
| `@anchor` | Создаёт устойчивую точку перехода на странице. | `src/lib/bigint/exact_predicate.h`: `exact_turn_pipeline` |
| `@example` и `EXAMPLE_PATH` | Добавляют исходник в специальный раздел с примерами. | `docs/examples/turn_example.cpp`, `Doxyfile` |
| `SEARCHENGINE`, `GENERATE_TREEVIEW`, `SOURCE_BROWSER` | Включают поиск, дерево разделов и просмотр исходников в HTML. | `Doxyfile` |
| `EXTRACT_ALL`, `EXTRACT_PRIVATE`, `EXTRACT_STATIC` | Не скрывают недокументированные, приватные и статические сущности. | `Doxyfile` |
| `JAVADOC_AUTOBRIEF`, `MULTILINE_CPP_IS_BRIEF` | Помогают автоматически выделять краткое описание из комментария. | `Doxyfile` |

@section doxy_custom Пользовательские метки проекта

В `Doxyfile` определены три псевдонима. Они превращают повторяющиеся
методические пометки в единый стиль документации.

| Псевдоним | Заголовок в HTML | Пример использования |
|---|---|---|
| `@complexity{...}` | «Сложность» | `src/lib/geometry/convex_hull.h`, `src/lib/geometry/delaunay.h` |
| `@invariant{...}` | «Инвариант» | `src/lib/geometry/convex_hull.h`, `src/lib/bigint/bigint.h` |
| `@educational{...}` | «Учебная заметка» | `src/lib/geometry/predicates.h` |

@section doxy_math Формулы: LaTeX и MathJax

Формулы записываются командой `@f[ ... @f]`, а параметр `USE_MATHJAX = YES`
подключает MathJax к HTML-страницам. Благодаря этому математические выражения
масштабируются и остаются читаемыми в браузере.

| Что документируется | Где найти |
|---|---|
| Формула векторного произведения | `src/lib/geometry/predicates.h` |
| Формула точного предиката поворота | `src/lib/bigint/exact_predicate.h` |
| Рекуррентная формула де Кастельжо | `src/lib/bezier/bezier.h` |
| Включение MathJax | `Doxyfile`: `USE_MATHJAX`, `MATHJAX_VERSION` |

@section doxy_graphs Graphviz: схемы и связи

Команды `@dot ... @enddot` позволяют записать Graphviz-схему прямо в
документирующем комментарии. В проекте есть две вручную созданные схемы:

| Схема | Что показывает | Где найти |
|---|---|---|
| Архитектура | Связи Qt-приложения, холста и библиотек. | `docs/architecture.md` |
| Конвейер точного поворота | Разбор координат, вычисление и получение знака. | `src/lib/bigint/exact_predicate.h` |

Кроме ручных схем, `Doxyfile` включает автоматические графы Graphviz:

| Настройка | Результат |
|---|---|
| `CLASS_GRAPH`, `COLLABORATION_GRAPH`, `GRAPHICAL_HIERARCHY` | Наследование, отношения классов и иерархия. |
| `INCLUDE_GRAPH`, `INCLUDED_BY_GRAPH` | Зависимости подключаемых файлов. |
| `CALL_GRAPH`, `CALLER_GRAPH` | Кого вызывает функция и кем она вызывается. |
| `GROUP_GRAPHS`, `TEMPLATE_RELATIONS`, `UML_LOOK` | Связи групп, шаблонов и UML-представление. |
| `DOT_IMAGE_FORMAT = svg`, `INTERACTIVE_SVG = YES` | Масштабируемые интерактивные SVG-графы. |

@section doxy_tasks Задачи на будущее

`@todo` и `GENERATE_TODOLIST = YES` создают страницу со списком дальнейших
улучшений. Здесь попадают только реальные пункты развития кода:

* деление с остатком для `BigInteger` — `src/lib/bigint/bigint.h`;
* сглаживание стыков составной кривой Безье — `src/lib/bezier/bezier.h`.

Параметр `GENERATE_BUGLIST = YES` также включён в `Doxyfile`, но тег `@bug`
в проекте пока не используется: известных подтверждённых ошибок нет.

@section doxy_formats Форматы и публикация

| Возможность | Назначение | Где найти |
|---|---|---|
| `GENERATE_HTML` | Основной сайт документации. | `docs/site/html/index.html` |
| `GENERATE_XML` | Машиночитаемое представление для IDE и внешних инструментов. | `docs/site/xml/` |
| `GENERATE_TAGFILE` | Файл перекрёстных ссылок для другой документации Doxygen. | `Doxyfile` |
| `INPUT_ENCODING = UTF-8`, `OUTPUT_LANGUAGE = Russian` | Корректная русскоязычная документация. | `Doxyfile` |
| GitHub Pages workflow | Публикует готовый статический HTML без запуска Doxygen на GitHub. | `.github/workflows/deploy-docs.yml` |

LaTeX-вывод сейчас намеренно выключен (`GENERATE_LATEX = NO`), но Doxygen может
включить его для печатного PDF без изменения комментариев. Это полезно для
отчёта или курсовой работы; HTML и XML при этом продолжают генерироваться как
обычно.

@section doxy_workflow Рабочая последовательность

1. Изменить комментарии или Markdown-страницы.
2. Выполнить `cmake --build build --target docs`.
3. Открыть `docs/site/html/index.html` и проверить ссылки, формулы и графы.
4. Добавить обновлённый каталог `docs/site` в коммит.
5. После push GitHub Actions опубликует готовую страницу на GitHub Pages.

@section doxy_conclusion Заключение

В проекте Doxygen соединяет код, объяснение алгоритма и навигацию по
архитектуре. Краткие теги поддерживают контракт API, формулы сохраняют
математическую точность, Graphviz делает связи наглядными, а HTML-сайт
позволяет показать результат без локальной среды разработки. Благодаря этому
документация остаётся проверяемой, воспроизводимой и полезной как для защиты
проекта, так и для его дальнейшей разработки.

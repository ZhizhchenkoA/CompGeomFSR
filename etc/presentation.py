"""Создаёт редактируемую презентацию о Doxygen в Geometry Workbench.

Запуск: ``.env/bin/python etc/presentation.py``.
Все слайды построены из стандартных текстовых полей, таблиц и прямоугольников,
поэтому их удобно изменять в PowerPoint или LibreOffice Impress вручную.
"""

from pathlib import Path
import sys

from pptx import Presentation
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN
from pptx.util import Inches, Pt


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "etc" / "doxygen_presentation.pptx"
ASSETS = ROOT / "etc" / "assets"

NAVY = RGBColor(15, 23, 42)
BLUE = RGBColor(37, 99, 235)
SLATE = RGBColor(71, 85, 105)
MUTED = RGBColor(100, 116, 139)
LIGHT_BLUE = RGBColor(239, 246, 255)
LIGHT_GRAY = RGBColor(248, 250, 252)
GRAY = RGBColor(203, 213, 225)
WHITE = RGBColor(255, 255, 255)


def set_run(run, size: int, color: RGBColor = NAVY, bold: bool = False,
            name: str = "Calibri") -> None:
    """Настраивает один фрагмент текста в совместимом системном шрифте."""
    run.font.name = name
    run.font.size = Pt(size)
    run.font.bold = bold
    run.font.color.rgb = color


def add_text(slide, x, y, w, h, value: str, size: int = 18,
             color: RGBColor = NAVY, bold: bool = False,
             align=PP_ALIGN.LEFT, font_name: str = "Calibri"):
    """Добавляет простое редактируемое текстовое поле."""
    box = slide.shapes.add_textbox(Inches(x), Inches(y), Inches(w), Inches(h))
    frame = box.text_frame
    frame.clear()
    frame.word_wrap = True
    frame.margin_left = Inches(0.03)
    frame.margin_right = Inches(0.03)

    for index, line in enumerate(value.split("\n")):
        paragraph = frame.paragraphs[0] if index == 0 else frame.add_paragraph()
        paragraph.alignment = align
        paragraph.space_after = Pt(5)
        run = paragraph.add_run()
        run.text = line
        set_run(run, size, color, bold, font_name)
    return box


def add_code(slide, x, y, w, h, value: str) -> None:
    """Добавляет нейтральный блок кода без сложного декоративного оформления."""
    shape = slide.shapes.add_shape(1, Inches(x), Inches(y), Inches(w), Inches(h))
    shape.fill.solid()
    shape.fill.fore_color.rgb = LIGHT_GRAY
    shape.line.color.rgb = GRAY
    frame = shape.text_frame
    frame.clear()
    frame.word_wrap = True
    frame.margin_left = Inches(0.16)
    frame.margin_right = Inches(0.16)
    frame.margin_top = Inches(0.12)
    for index, line in enumerate(value.split("\n")):
        paragraph = frame.paragraphs[0] if index == 0 else frame.add_paragraph()
        paragraph.space_after = Pt(0)
        run = paragraph.add_run()
        run.text = line
        set_run(run, 13, NAVY, False, "Consolas")


def add_bullets(slide, x, y, w, h, items: list[str], size: int = 18) -> None:
    """Добавляет обычный список; каждая строка остаётся отдельным абзацем."""
    box = slide.shapes.add_textbox(Inches(x), Inches(y), Inches(w), Inches(h))
    frame = box.text_frame
    frame.clear()
    frame.word_wrap = True
    frame.margin_left = Inches(0.04)
    frame.margin_right = Inches(0.04)
    for index, item in enumerate(items):
        paragraph = frame.paragraphs[0] if index == 0 else frame.add_paragraph()
        paragraph.space_after = Pt(10)
        run = paragraph.add_run()
        run.text = f"• {item}"
        set_run(run, size, NAVY)


def add_header(slide, title: str, subtitle: str, number: int) -> None:
    """Добавляет единый лаконичный заголовок слайда."""
    slide.background.fill.solid()
    slide.background.fill.fore_color.rgb = WHITE
    line = slide.shapes.add_shape(1, 0, 0, Inches(13.333), Inches(0.08))
    line.fill.solid()
    line.fill.fore_color.rgb = BLUE
    line.line.fill.background()
    add_text(slide, 0.7, 0.4, 11.7, 0.48, title, 28, NAVY, True)
    add_text(slide, 0.72, 0.93, 11.5, 0.28, subtitle, 13, MUTED)
    add_text(slide, 12.25, 0.47, 0.35, 0.25, str(number), 12, MUTED,
             False, PP_ALIGN.RIGHT)
    add_text(slide, 0.72, 7.08, 11.7, 0.18,
             "Geometry Workbench · документация Doxygen", 9, MUTED)


def add_table(slide, x, y, w, h, headers: list[str], rows: list[list[str]]) -> None:
    """Добавляет простую таблицу, содержимое которой легко редактируется."""
    table = slide.shapes.add_table(len(rows) + 1, len(headers),
                                   Inches(x), Inches(y), Inches(w), Inches(h)).table
    for column in table.columns:
        column.width = Inches(w / len(headers))

    for column_index, header in enumerate(headers):
        cell = table.cell(0, column_index)
        cell.fill.solid()
        cell.fill.fore_color.rgb = BLUE
        cell.text = header
        for run in cell.text_frame.paragraphs[0].runs:
            set_run(run, 13, WHITE, True)

    for row_index, row in enumerate(rows, start=1):
        for column_index, value in enumerate(row):
            cell = table.cell(row_index, column_index)
            cell.fill.solid()
            cell.fill.fore_color.rgb = LIGHT_BLUE if row_index % 2 else WHITE
            cell.text = value
            for run in cell.text_frame.paragraphs[0].runs:
                set_run(run, 12, NAVY)


def add_picture_if_present(slide, filename: str, x: float, y: float, width: float) -> None:
    """Вставляет иллюстрацию, если файл из набора ассетов существует."""
    image = ASSETS / filename
    if image.exists():
        slide.shapes.add_picture(str(image), Inches(x), Inches(y), width=Inches(width))


def make_presentation(output: Path) -> None:
    """Собирает презентацию из простых, пригодных для ручного редактирования слайдов."""
    prs = Presentation()
    prs.slide_width = Inches(13.333)
    prs.slide_height = Inches(7.5)
    blank = prs.slide_layouts[6]

    slide = prs.slides.add_slide(blank)
    slide.background.fill.solid()
    slide.background.fill.fore_color.rgb = WHITE
    line = slide.shapes.add_shape(1, 0, 0, Inches(13.333), Inches(0.1))
    line.fill.solid()
    line.fill.fore_color.rgb = BLUE
    line.line.fill.background()
    add_text(slide, 0.9, 1.2, 11.5, 0.65, "Doxygen в Geometry Workbench", 34, NAVY, True)
    add_text(slide, 0.93, 1.95, 10.8, 0.4,
             "Документирование C++-проекта: API, формулы, схемы и статический сайт", 19, SLATE)
    add_text(slide, 0.95, 3.0, 5.5, 1.3,
             "Цель презентации\nПоказать, как комментарии в коде становятся удобной документацией.",
             18, NAVY)
    add_picture_if_present(slide, "doxygen.png", 8.1, 2.55, 1.5)
    add_picture_if_present(slide, "graphviz.png", 9.95, 2.55, 1.5)
    add_picture_if_present(slide, "latex.png", 11.55, 2.55, 1.25)
    add_text(slide, 0.95, 6.55, 11.2, 0.25,
             "C++17 · Qt · Doxygen · Graphviz · MathJax · GitHub Pages", 13, MUTED)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Что документирует Doxygen", "Исходный код остаётся единственным источником правды.", 2)
    add_bullets(slide, 0.85, 1.55, 6.0, 4.8, [
        "классы Qt-интерфейса и библиотечные функции;",
        "параметры, результаты, предусловия и особые случаи;",
        "группы алгоритмов, перекрёстные ссылки и примеры;",
        "диаграммы Graphviz, HTML-сайт и XML для инструментов.",
    ])
    add_code(slide, 7.3, 1.75, 4.7, 2.7,
             "/**\n * @brief Предикат поворота.\n * @param eps Допуск.\n * @return -1, 0 или 1.\n */\nint turn(...);")
    add_text(slide, 7.35, 4.8, 4.7, 0.7,
             "Комментарий в заголовке функции\nпопадает в страницу API автоматически.", 16, SLATE)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Doxyfile: единая настройка", "В проекте конфигурация лежит в корне репозитория.", 3)
    add_code(slide, 0.8, 1.45, 5.8, 4.6,
             "INPUT             = src docs/...\nRECURSIVE         = YES\nGENERATE_HTML     = YES\nGENERATE_XML      = YES\nHAVE_DOT          = YES\nUSE_MATHJAX       = YES\nGENERATE_TODOLIST = YES\nEXAMPLE_PATH      = docs/examples")
    add_table(slide, 7.0, 1.55, 5.3, 3.5,
              ["Параметр", "Результат"], [
                  ["INPUT", "исходники и Markdown-страницы"],
                  ["HAVE_DOT", "графы классов и вызовов"],
                  ["USE_MATHJAX", "математические формулы в HTML"],
                  ["GENERATE_XML", "данные для внешних инструментов"],
              ])

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Контракт функции прямо в коде", "Новые теги описывают не только назначение, но и правила использования API.", 4)
    add_code(slide, 0.75, 1.4, 6.3, 4.95,
             "/**\n * @brief Точно вычисляет знак поворота.\n * @pre sign и error не равны nullptr.\n * @post *sign принадлежит {-1, 0, 1}.\n * @warning Экспонента ограничена 100000.\n * @see geometry::turn()\n * @since 1.0\n */\nbool exactTurn(...);")
    add_bullets(slide, 7.45, 1.65, 4.8, 4.7, [
        "@pre — что обязуется передать вызывающий код;",
        "@post — что гарантирует функция после успеха;",
        "@warning — важное ограничение алгоритма;",
        "@see и @since — навигация и история API.",
    ], 16)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Формулы LaTeX и MathJax", "Математическая запись остаётся рядом с реализацией алгоритма.", 5)
    add_code(slide, 0.9, 1.55, 5.85, 2.75,
             "/**\n * @f[\\operatorname{cross}(a,b,c) =\n * (b_x-a_x)(c_y-a_y) -\n * (b_y-a_y)(c_x-a_x).@f]\n */")
    add_text(slide, 7.25, 1.85, 4.7, 0.4,
             "В HTML формула отображается MathJax-движком.", 18, NAVY, True)
    add_text(slide, 7.25, 2.55, 4.7, 0.5,
             "cross(a, b, c) = (bₓ − aₓ)(cᵧ − aᵧ) − (bᵧ − aᵧ)(cₓ − aₓ)", 17, BLUE, True)
    add_bullets(slide, 1.05, 4.8, 11.0, 1.3, [
        "Формулы полезны для предиката поворота и алгоритма де Кастельжо.",
        "Тот же LaTeX-синтаксис можно использовать в печатном выводе Doxygen.",
    ], 16)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Схемы алгоритмов через @dot", "Graphviz строит диаграммы из короткого текста рядом с API.", 6)
    add_code(slide, 0.8, 1.45, 5.65, 4.5,
             "@dot\ndigraph exact_turn {\n  rankdir=LR;\n  input [label=\"Координаты\"];\n  parse [label=\"Мантисса + степень\"];\n  sign [label=\"Знак BigInteger\"];\n  input -> parse -> sign;\n}\n@enddot")
    add_text(slide, 7.0, 1.65, 4.9, 0.38, "В проекте это применено к exactTurn().", 18, NAVY, True)
    add_bullets(slide, 7.0, 2.4, 4.9, 3.2, [
        "показывает этапы точного вычисления;",
        "масштабируется как SVG;",
        "дополняет автоматически созданные графы классов, вызовов и подключений.",
    ], 17)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "@todo: план развития из исходников", "Doxygen собирает реальные технические задачи в отдельную страницу.", 7)
    add_table(slide, 0.9, 1.55, 11.5, 2.1,
              ["Место", "Комментарий", "Что появится в документации"], [
                  ["BigInteger", "Добавить деление с остатком", "пункт в списке «Необходимо сделать»"],
                  ["Кривые Безье", "Добавить сглаживание стыков C¹", "пункт в списке дальнейших улучшений"],
              ])
    add_code(slide, 2.0, 4.25, 9.2, 1.1,
             "/** @todo Добавить режим сглаживания стыков составной кривой C¹. */")
    add_text(slide, 1.35, 5.9, 10.7, 0.45,
             "Преимущество: задача не теряется в комментариях и видна на одной странице HTML.", 18, SLATE, True, PP_ALIGN.CENTER)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Группы, примеры и собственные метки", "Навигация превращает набор файлов в понятный API.", 8)
    add_table(slide, 0.75, 1.45, 11.9, 3.6,
              ["Возможность", "Пример в проекте", "Польза"], [
                  ["@defgroup / @ingroup", "Алгоритмы геометрии, bigint, Безье", "разделы API по смыслу"],
                  ["@example", "turn_example.cpp", "готовый минимальный пример"],
                  ["@complexity / @invariant", "convexHull", "сложность и свойства результата"],
                  ["@anchor / @see", "конвейер exactTurn", "переходы между разделами"],
              ])
    add_text(slide, 0.95, 5.65, 11.3, 0.45,
             "Все эти элементы генерируются из комментариев и не требуют отдельного сайта вручную.",
             18, SLATE, True, PP_ALIGN.CENTER)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Результаты одной генерации", "Doxygen готовит материалы и для браузера, и для отчёта.", 9)
    add_picture_if_present(slide, "doxygen.png", 1.0, 1.45, 1.35)
    add_picture_if_present(slide, "graphviz.png", 5.95, 1.45, 1.35)
    add_picture_if_present(slide, "latex.png", 10.85, 1.45, 1.15)
    add_table(slide, 0.9, 3.05, 11.6, 2.25,
              ["Формат", "Назначение", "Где находится"], [
                  ["HTML + SVG", "навигация, поиск, диаграммы", "docs/site/html/"],
                  ["XML", "IDE и внешние обработчики", "docs/site/xml/"],
                  ["LaTeX (по настройке)", "печатный PDF-отчёт", "вывод Doxygen"],
              ])

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Публикация на GitHub Pages", "GitHub публикует готовый статический HTML, но не собирает Doxygen.", 10)
    add_code(slide, 0.85, 1.5, 5.9, 2.8,
             "# Локально\ndoxygen Doxyfile\ngit add docs/site\ngit commit -m \"Обновить документацию\"\ngit push")
    add_bullets(slide, 7.15, 1.65, 5.0, 3.3, [
        "workflow загружает docs/site/html как артефакт Pages;",
        "GitHub не устанавливает Doxygen и Graphviz;",
        "сайт повторяет проверенный локальный результат.",
    ], 17)
    add_text(slide, 1.0, 5.55, 11.3, 0.5,
             "Так документация доступна по ссылке и остаётся частью репозитория.", 19, NAVY, True, PP_ALIGN.CENTER)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Как обновить документацию", "Рабочая последовательность для проекта.", 11)
    add_code(slide, 1.0, 1.65, 11.0, 2.05,
             "cmake -S . -B build\ncmake --build build --target docs\nxdg-open docs/site/html/index.html")
    add_bullets(slide, 1.25, 4.45, 10.6, 1.8, [
        "Цель docs запускает Doxygen с Doxyfile из корня проекта.",
        "После изменения комментариев HTML и XML нужно перегенерировать и добавить в коммит.",
    ], 18)

    slide = prs.slides.add_slide(blank)
    add_header(slide, "Итог", "Doxygen помогает объяснить проект, а не только перечислить функции.", 12)
    add_bullets(slide, 1.0, 1.6, 10.9, 4.5, [
        "Комментарии фиксируют назначение, контракт, ограничения и сложность API.",
        "MathJax делает формулы читабельными прямо в HTML-документации.",
        "@dot и Graphviz показывают алгоритмические и архитектурные связи.",
        "@todo превращает планы развития в видимый список.",
        "Готовый HTML публикуется на GitHub Pages без сборки на сервере.",
    ], 20)
    add_text(slide, 1.0, 6.3, 11.0, 0.35, "Спасибо за внимание", 24, BLUE, True, PP_ALIGN.CENTER)

    prs.save(output)


if __name__ == "__main__":
    target = Path(sys.argv[1]).expanduser() if len(sys.argv) > 1 else OUTPUT
    target.parent.mkdir(parents=True, exist_ok=True)
    make_presentation(target)
    print(f"Создан файл: {target}")

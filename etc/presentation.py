"""Генератор презентации о Doxygen для Geometry Workbench.

Запуск: ``.env/bin/python etc/presentation.py``.
"""
from pathlib import Path
import sys

from pptx import Presentation
from pptx.dml.color import RGBColor
from pptx.enum.shapes import MSO_AUTO_SHAPE_TYPE, MSO_CONNECTOR
from pptx.enum.text import PP_ALIGN, MSO_ANCHOR
from pptx.util import Inches, Pt

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "etc" / "doxygen_presentation.pptx"
NAVY = RGBColor(15, 23, 42)
BLUE = RGBColor(37, 99, 235)
PURPLE = RGBColor(124, 58, 237)
GREEN = RGBColor(22, 163, 74)
ORANGE = RGBColor(234, 88, 12)
RED = RGBColor(220, 38, 38)
CYAN = RGBColor(14, 116, 144)
SLATE = RGBColor(71, 85, 105)
MUTED = RGBColor(100, 116, 139)
LIGHT = RGBColor(248, 250, 252)
GRID = RGBColor(226, 232, 240)
WHITE = RGBColor(255, 255, 255)


def font(run, size=18, color=NAVY, bold=False, name="DejaVu Sans"):
    run.font.name = name
    run.font.size = Pt(size)
    run.font.color.rgb = color
    run.font.bold = bold


def text(slide, x, y, w, h, value, size=18, color=NAVY, bold=False,
         align=PP_ALIGN.LEFT, name="DejaVu Sans"):
    box = slide.shapes.add_textbox(Inches(x), Inches(y), Inches(w), Inches(h))
    frame = box.text_frame
    frame.clear()
    frame.word_wrap = True
    frame.margin_left = Inches(.08)
    frame.margin_right = Inches(.08)
    run = frame.paragraphs[0].add_run()
    run.text = value
    frame.paragraphs[0].alignment = align
    font(run, size, color, bold, name)
    return box


def card(slide, x, y, w, h, title, body, color=BLUE):
    shape = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.ROUNDED_RECTANGLE,
                                   Inches(x), Inches(y), Inches(w), Inches(h))
    shape.fill.solid()
    shape.fill.fore_color.rgb = WHITE
    shape.line.color.rgb = color
    shape.line.width = Pt(1.5)
    frame = shape.text_frame
    frame.clear()
    frame.word_wrap = True
    frame.margin_left = Inches(.16)
    frame.margin_right = Inches(.16)
    frame.margin_top = Inches(.12)
    r = frame.paragraphs[0].add_run()
    r.text = title
    font(r, 16, color, True)
    p = frame.add_paragraph()
    p.space_before = Pt(7)
    r = p.add_run()
    r.text = body
    font(r, 12, SLATE)
    return shape


def code(slide, x, y, w, h, value):
    shape = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.ROUNDED_RECTANGLE,
                                   Inches(x), Inches(y), Inches(w), Inches(h))
    shape.fill.solid(); shape.fill.fore_color.rgb = NAVY
    shape.line.color.rgb = RGBColor(30, 41, 59)
    frame = shape.text_frame; frame.clear(); frame.word_wrap = True
    frame.margin_left = Inches(.18); frame.margin_right = Inches(.18)
    frame.margin_top = Inches(.14)
    run = frame.paragraphs[0].add_run(); run.text = value
    font(run, 12, RGBColor(226, 232, 240), False, "DejaVu Sans Mono")


def header(slide, number, title, subtitle, accent=BLUE):
    slide.background.fill.solid(); slide.background.fill.fore_color.rgb = LIGHT
    band = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.RECTANGLE, 0, 0, Inches(13.333), Inches(.12))
    band.fill.solid(); band.fill.fore_color.rgb = accent; band.line.color.rgb = accent
    circle = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.OVAL, Inches(.45), Inches(.42), Inches(.72), Inches(.72))
    circle.fill.solid(); circle.fill.fore_color.rgb = accent; circle.line.color.rgb = accent
    text(slide, .45, .57, .72, .25, str(number), 20, WHITE, True, PP_ALIGN.CENTER)
    text(slide, 1.35, .42, 11.5, .45, title, 28, NAVY, True)
    text(slide, 1.38, .91, 11.2, .3, subtitle, 13, MUTED)
    text(slide, .55, 7.18, 12, .2, "Geometry Workbench • Doxygen", 9, MUTED)


def bullets(slide, x, y, width, values, accent=BLUE, size=18):
    for i, value in enumerate(values):
        yy = y + i * .62
        dot = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.OVAL, Inches(x), Inches(yy + .12), Inches(.13), Inches(.13))
        dot.fill.solid(); dot.fill.fore_color.rgb = accent; dot.line.color.rgb = accent
        text(slide, x + .25, yy, width - .25, .42, value, size, NAVY)


def arrow(slide, x1, y1, x2, y2, color=MUTED):
    line = slide.shapes.add_connector(MSO_CONNECTOR.STRAIGHT, Inches(x1), Inches(y1), Inches(x2), Inches(y2))
    line.line.color.rgb = color; line.line.width = Pt(2.2)


def make_presentation(output):
    prs = Presentation()
    prs.slide_width = Inches(13.333); prs.slide_height = Inches(7.5)
    blank = prs.slide_layouts[6]

    slide = prs.slides.add_slide(blank)
    slide.background.fill.solid(); slide.background.fill.fore_color.rgb = LIGHT
    band = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.RECTANGLE, 0, 0, Inches(13.333), Inches(.16))
    band.fill.solid(); band.fill.fore_color.rgb = PURPLE; band.line.color.rgb = PURPLE
    circle = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.OVAL, Inches(.8), Inches(1.0), Inches(1.2), Inches(1.2))
    circle.fill.solid(); circle.fill.fore_color.rgb = PURPLE; circle.line.color.rgb = PURPLE
    text(slide, .8, 1.31, 1.2, .4, "D", 34, WHITE, True, PP_ALIGN.CENTER)
    text(slide, 2.3, 1.0, 10.2, .55, "Doxygen в Geometry Workbench", 34, NAVY, True)
    text(slide, 2.32, 1.75, 10, .4, "Документация как часть инженерного проекта", 21, PURPLE, True)
    card(slide, 2.32, 2.7, 9.5, 1.25, "Что покажем", "конфигурацию • комментарии API • Graphviz • примеры • HTML и XML", PURPLE)
    text(slide, 2.32, 5.3, 8, .3, "C++17 • Qt 6 • Doxygen • Graphviz", 16, MUTED)
    text(slide, .8, 6.85, 11.8, .25, "Компьютерная геометрия • учебный стенд", 12, MUTED)

    slide = prs.slides.add_slide(blank)
    header(slide, 1, "Зачем нужен Doxygen", "Документация строится из того же кода, который мы компилируем.")
    bullets(slide, .8, 1.55, 6.2, [
        "API видно сразу: классы, функции, параметры и результаты.",
        "Документация не живёт отдельно от исходников.",
        "Графы объясняют зависимости лучше длинного текста.",
        "HTML удобен человеку, XML — инструментам.",
    ])
    card(slide, 8, 1.65, 3.8, 1.0, "Исходный код", "/** @brief ... */", BLUE)
    card(slide, 8, 3.1, 3.8, 1.0, "Doxyfile", "INPUT • EXTRACT • GRAPH", PURPLE)
    card(slide, 8, 4.55, 3.8, 1.0, "Результат", "HTML • SVG • XML", GREEN)
    arrow(slide, 9.9, 2.68, 9.9, 3.05, BLUE); arrow(slide, 9.9, 4.13, 9.9, 4.5, PURPLE)

    slide = prs.slides.add_slide(blank)
    header(slide, 2, "Как работает Doxygen", "Конвейер от комментария до страницы API.", CYAN)
    stages = [("1", "Комментарии", "/** @brief ... */", BLUE), ("2", "Doxyfile", "INPUT • GRAPH", PURPLE),
              ("3", "Парсер", "классы • namespaces", ORANGE), ("4", "Генераторы", "HTML • SVG • XML", GREEN)]
    for i, (num, title, body, color) in enumerate(stages):
        x = .65 + i * 3.1
        circle = slide.shapes.add_shape(MSO_AUTO_SHAPE_TYPE.OVAL, Inches(x + .9), Inches(1.55), Inches(.55), Inches(.55))
        circle.fill.solid(); circle.fill.fore_color.rgb = color; circle.line.color.rgb = color
        text(slide, x + .9, 1.69, .55, .18, num, 13, WHITE, True, PP_ALIGN.CENTER)
        card(slide, x, 2.42, 2.35, 1.35, title, body, color)
        if i < 3: arrow(slide, x + 2.4, 3.1, x + 3.0, 3.1)
    card(slide, 1.25, 5.0, 10.8, .9, "Главная идея", "Комментарий становится частью навигации по проекту, а не просто текстом рядом с кодом.", CYAN)

    slide = prs.slides.add_slide(blank)
    header(slide, 3, "Doxyfile: центр настройки", "Один файл управляет полнотой анализа и форматами вывода.", PURPLE)
    code(slide, .7, 1.45, 6.2, 4.75, "INPUT       = src docs\nRECURSIVE   = YES\nEXTRACT_ALL = YES\nSOURCE_BROWSER = YES\nGENERATE_HTML = YES\nGENERATE_XML  = YES\nHAVE_DOT      = YES\nCALL_GRAPH    = YES\nEXAMPLE_PATH  = docs/examples\nALIASES       = complexity ...")
    bullets(slide, 7.45, 1.55, 5.0, [
        "INPUT — что анализировать.",
        "EXTRACT_* — насколько полно показывать код.",
        "HAVE_DOT — подключить Graphviz.",
        "ALIASES — добавить термины проекта.",
    ], PURPLE, 17)

    slide = prs.slides.add_slide(blank)
    header(slide, 4, "Документирующие комментарии", "Единый блочный стиль с тегами Doxygen.", BLUE)
    code(slide, .7, 1.35, 6.3, 4.9, "/**\n * @brief Проверяет принадлежность точки отрезку.\n * @param a Первый конец.\n * @param b Второй конец.\n * @param p Проверяемая точка.\n * @return true, если точка лежит на отрезке.\n * @complexity O(1)\n */\nbool onSegment(...);")
    card(slide, 7.55, 1.55, 4.65, 1.05, "@brief", "короткое назначение сущности", BLUE)
    card(slide, 7.55, 3.0, 4.65, 1.05, "@param / @return", "контракт входа и результата", GREEN)
    card(slide, 7.55, 4.45, 4.65, 1.05, "\\complexity", "пользовательский alias проекта", PURPLE)

    slide = prs.slides.add_slide(blank)
    header(slide, 5, "Группы и диаграммы", "Документация превращается в карту проекта.", ORANGE)
    card(slide, .75, 1.55, 2.9, 1.0, "geometry_algorithms", "базовые алгоритмы", GREEN)
    card(slide, 4.05, 1.55, 2.75, 1.0, "geometry_predicates", "turn • cross", BLUE)
    card(slide, 7.2, 1.55, 2.4, 1.0, "bigint_api", "точность", ORANGE)
    card(slide, 10, 1.55, 2.4, 1.0, "bezier_api", "кривые", PURPLE)
    arrow(slide, 3.7, 2.05, 4.0, 2.05, GREEN); arrow(slide, 6.9, 2.05, 7.15, 2.05, GREEN); arrow(slide, 9.7, 2.05, 9.95, 2.05, GREEN)
    card(slide, 1.0, 3.35, 11.2, 1.5, "Graphviz-фишки", "CLASS_GRAPH • INCLUDE_GRAPH • CALL_GRAPH • COLLABORATION_GRAPH\nDoxygen генерирует SVG, поэтому диаграммы масштабируются без потери качества.", ORANGE)

    slide = prs.slides.add_slide(blank)
    header(slide, 6, "Как это применено в проекте", "Алгоритмы отделены от Qt и документируются как самостоятельный API.", GREEN)
    card(slide, .65, 1.5, 3.0, 1.05, "GeometryWorkbench", "GeometryPage • Canvas", BLUE)
    card(slide, 4.15, 1.5, 2.5, 1.05, "geometry_qt", "screen ↔ math", CYAN)
    card(slide, 7.2, 1.5, 2.5, 1.05, "geometry", "hull • Delaunay", GREEN)
    card(slide, 10.25, 1.5, 2.4, 1.05, "bigint / bezier", "точность • кривые", PURPLE)
    arrow(slide, 3.7, 2.02, 4.1, 2.02, BLUE); arrow(slide, 6.7, 2.02, 7.15, 2.02, CYAN); arrow(slide, 9.75, 2.02, 10.2, 2.02, GREEN)
    card(slide, .9, 3.15, 5.6, 2.15, "Пример", "convexHull(points)\n\nАлгоритм ничего не знает о Qt. GeometryPage получает результат и передаёт вершины в GeometryCanvas.", GREEN)
    card(slide, 6.9, 3.15, 5.6, 2.15, "Польза", "• алгоритмы тестируются отдельно;\n• HTML показывает контракт API;\n• графы объясняют зависимости.", BLUE)

    slide = prs.slides.add_slide(blank)
    header(slide, 7, "Результаты генерации", "Один запуск создаёт несколько представлений проекта.", CYAN)
    for i, (title, body, color) in enumerate([("HTML", "index.html\nпоиск и дерево", BLUE), ("SVG", "Graphviz\nдиаграммы", PURPLE), ("XML", "для IDE\nи генераторов", GREEN), ("Examples", "примеры API\nотдельный раздел", ORANGE)]):
        card(slide, .75 + i * 3.1, 2.0, 2.55, 1.9, title, body, color)
    text(slide, 1, 5.25, 11.2, .35, "build/docs/html/index.html  •  build/docs/xml/  •  build/docs/html/architecture.html", 16, MUTED, False, PP_ALIGN.CENTER)

    slide = prs.slides.add_slide(blank)
    header(slide, 8, "Запуск: три команды", "Документация встроена в CMake-процесс.", GREEN)
    code(slide, 1, 1.55, 11.3, 2.05, "cmake -S . -B build\ncmake --build build --target docs\nxdg-open build/docs/html/index.html")
    bullets(slide, 1.15, 4.2, 10.8, ["Doxygen читает Doxyfile из корня проекта.", "CMake-цель docs доступна, если Doxygen найден.", "Изменили комментарий → снова target docs → HTML обновился."], GREEN, 18)

    slide = prs.slides.add_slide(blank)
    header(slide, 9, "Итог", "Doxygen — это не только справочник функций.", PURPLE)
    card(slide, .9, 1.45, 11.5, 1.2, "Главная мысль", "В Geometry Workbench документация стала частью сборки и архитектуры проекта.", PURPLE)
    bullets(slide, 1.1, 3.15, 10.8, ["Комментарии фиксируют контракт и сложность алгоритма.", "Группы превращают набор файлов в понятный API.", "Graphviz показывает связи, которые сложно увидеть в тексте.", "HTML, SVG и XML подходят разным читателям и инструментам."], PURPLE, 20)
    text(slide, 1, 6.35, 11.2, .35, "Спасибо! Вопросы?", 24, PURPLE, True, PP_ALIGN.CENTER)

    # 10. External illustrations and ecosystem
    slide = prs.slides.add_slide(blank)
    header(slide, 10, "Экосистема вокруг Doxygen", "Doxygen связывает документацию, Graphviz и LaTeX.", CYAN)
    slide.shapes.add_picture(str(ROOT / "etc/assets/doxygen.png"), Inches(.8), Inches(1.55), width=Inches(3.2))
    slide.shapes.add_picture(str(ROOT / "etc/assets/graphviz.png"), Inches(4.55), Inches(1.55), width=Inches(3.55))
    slide.shapes.add_picture(str(ROOT / "etc/assets/latex.png"), Inches(8.75), Inches(1.55), width=Inches(3.55))
    text(slide, .85, 3.0, 3.0, .35, "Doxygen: анализ API", 16, NAVY, True, PP_ALIGN.CENTER)
    text(slide, 4.7, 3.0, 3.2, .35, "Graphviz: диаграммы", 16, NAVY, True, PP_ALIGN.CENTER)
    text(slide, 8.95, 3.0, 3.1, .35, "LaTeX: печатный PDF", 16, NAVY, True, PP_ALIGN.CENTER)
    card(slide, 1.0, 4.0, 11.3, 1.2, "Как читать слайд", "Doxygen собирает смысл из комментариев, Graphviz визуализирует связи, а LaTeX превращает результат в типографски аккуратный документ.", CYAN)
    text(slide, .9, 6.25, 11.6, .45, "Источники изображений: commons.wikimedia.org (Doxygen, LaTeX), graphviz.org/Gallery (FSM).", 10, MUTED, False, PP_ALIGN.CENTER)

    # 11. LaTeX
    slide = prs.slides.add_slide(blank)
    header(slide, 11, "LaTeX: ещё один формат вывода", "Полезен для отчёта, курсовой или печатной API-справки.", PURPLE)
    slide.shapes.add_picture(str(ROOT / "etc/assets/latex.png"), Inches(8.75), Inches(1.35), width=Inches(3.8))
    code(slide, .75, 1.45, 7.25, 2.2, "GENERATE_LATEX = YES\nLATEX_OUTPUT   = latex\nUSE_PDFLATEX   = YES\n\ndoxygen Doxyfile\ncd build/docs/latex && make")
    bullets(slide, .95, 4.15, 7.6, [
        "Doxygen генерирует refman.tex и файлы разделов.",
        "PDF удобно приложить к отчёту или распечатать.",
        "В документации можно использовать \\f$ ... \\f$ для формул.",
        "Например: \\f$(B-A)\\times(P-A)\\f$ — ориентированная площадь.",
    ], PURPLE, 17)
    text(slide, 8.7, 5.25, 3.9, .75, "Фишка для презентации:\nодин источник → HTML для браузера и PDF для отчёта.", 15, NAVY, True, PP_ALIGN.CENTER)

    # 12. More practical examples
    slide = prs.slides.add_slide(blank)
    header(slide, 12, "Больше практических примеров", "Небольшие теги дают большую пользу при навигации по проекту.", ORANGE)
    code(slide, .65, 1.35, 4.0, 1.95, "/**\n * @defgroup geometry_algorithms\n * Базовые алгоритмы.\n */")
    code(slide, 4.9, 1.35, 3.65, 1.95, "/**\n * @ingroup geometry_algorithms\n * @complexity O(n log n)\n */\nconvexHull(...);")
    code(slide, 8.8, 1.35, 3.85, 1.95, "@dot\ndigraph {\n  Qt -> geometry;\n}\n@enddot")
    card(slide, .75, 3.85, 3.8, 1.45, "@example", "turn_example.cpp появляется в разделе Examples и показывает реальное использование API.", BLUE)
    card(slide, 4.75, 3.85, 3.8, 1.45, "@todo / @bug", "Doxygen может собрать список будущих задач и известных проблем.", RED)
    card(slide, 8.75, 3.85, 3.8, 1.45, "@image / @cite", "Можно подключать схемы, иллюстрации и библиографические ссылки.", GREEN)
    text(slide, .9, 6.05, 11.5, .5, "Практический совет: документируйте не каждую строку, а публичный контракт и важные инварианты.", 18, NAVY, True, PP_ALIGN.CENTER)
    prs.save(output)


if __name__ == "__main__":
    output = Path(sys.argv[1]).expanduser() if len(sys.argv) > 1 else OUTPUT
    output.parent.mkdir(parents=True, exist_ok=True)
    make_presentation(output)
    print(f"Created: {output}")

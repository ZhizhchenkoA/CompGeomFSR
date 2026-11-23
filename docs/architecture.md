@page architecture Архитектура проекта

Приложение отделяет алгоритмы компьютерной геометрии от Qt-интерфейса.
Это позволяет тестировать библиотечный код без запуска графического окна.

@dot
digraph architecture {
  rankdir=LR;
  node [shape=box, style="rounded,filled", fontname="DejaVu Sans"];
  app [label="Приложение Qt GeometryWorkbench", fillcolor="#dbeafe"];
  canvas [label="Холст GeometryCanvas", fillcolor="#e0e7ff"];
  geometry [label="geometry\nпредикаты и операции", fillcolor="#dcfce7"];
  bigint [label="bigint\nточная арифметика", fillcolor="#fef3c7"];
  bezier [label="bezier\nде Кастельжо", fillcolor="#f3e8ff"];
  qt [label="geometry_qt\nY-инверсия", fillcolor="#f1f5f9"];
  app -> canvas [label="события мыши"];
  app -> geometry [label="чистые алгоритмы"];
  app -> bigint [label="задача 3"];
  app -> bezier [label="задача 13"];
  app -> qt [label="координаты"];
  canvas -> qt [label="экран ↔ геометрия"];
}
@enddot

@section flow Поток данных

1. Qt-сигнал передаёт клик или перетаскивание в `GeometryPage`.
2. Страница переводит `QPointF` через `geometry_qt::toGeometry`.
3. Чистая библиотека возвращает число, контур, треугольники или кривую.
4. Результат переводится обратно в экранные координаты и рисуется слоями `GeometryCanvas`.

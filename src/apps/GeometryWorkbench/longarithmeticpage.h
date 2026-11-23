#pragma once

#include <array>

#include <QWidget>

class QLineEdit;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class LongArithmeticPage;
}
QT_END_NAMESPACE

/**
 * @brief Форма задачи 3: точный предикат без графического холста.
 * Поля принимают целые, десятичные и экспоненциальные строки.
 */
class LongArithmeticPage : public QWidget
{
    Q_OBJECT
public:
    explicit LongArithmeticPage(QWidget *parent = nullptr);
    ~LongArithmeticPage() override;

private slots:
    void calculate();
    void setNearLineExample();
    void setLargeExample();
    void setInvalidExample();

private:
    void setCoordinates(const std::array<QString, 6> &values);
    void updateIfOnline();

    Ui::LongArithmeticPage *ui;
    std::array<QLineEdit *, 6> m_fields{};
};

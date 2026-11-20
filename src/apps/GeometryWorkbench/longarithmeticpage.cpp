#include "longarithmeticpage.h"
#include "ui_longarithmeticpage.h"

#include <array>
#include <cmath>

#include "bigint/exact_predicate.h"

namespace {

QString signText(int sign)
{
    return sign > 0 ? "1 - P слева от AB" : sign < 0 ? "−1 - P справа от AB" : "0 - точки на одной прямой";
}

} // namespace

LongArithmeticPage::LongArithmeticPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LongArithmeticPage)
{
    ui->setupUi(this);
    m_fields = {ui->axLineEdit, ui->ayLineEdit, ui->bxLineEdit,
                ui->byLineEdit, ui->pxLineEdit, ui->pyLineEdit};

    for (QLineEdit* field : m_fields) {
        connect(field, &QLineEdit::textChanged, this, &LongArithmeticPage::updateIfOnline);
    }
    connect(ui->calculateButton, &QPushButton::clicked, this, &LongArithmeticPage::calculate);
    connect(ui->nearLineButton, &QPushButton::clicked, this, &LongArithmeticPage::setNearLineExample);
    connect(ui->largeNumbersButton, &QPushButton::clicked, this, &LongArithmeticPage::setLargeExample);
    connect(ui->invalidInputButton, &QPushButton::clicked, this, &LongArithmeticPage::setInvalidExample);
    setNearLineExample();
}

LongArithmeticPage::~LongArithmeticPage()
{
    delete ui;
}

void LongArithmeticPage::calculate()
{
    std::array<std::string, 6> coordinates;
    std::array<QString, 6> text;
    for (int i = 0; i < 6; ++i) {
        text[i] = m_fields[i]->text();
        coordinates[i] = text[i].toStdString();
    }

    int exactSign = 0;
    std::string error;
    if (!bigint::exactTurn(coordinates, &exactSign, &error)) {
        ui->resultLabel->setText("Ошибка ввода: " + QString::fromStdString(error));
        ui->resultLabel->setStyleSheet("background: #fef2f2; border: 1px solid #fca5a5; border-radius: 7px; padding: 10px; color: #991b1b; font-size: 15px;");
        return;
    }

    bool doubleOk = true;
    std::array<double, 6> values{};
    for (int i = 0; i < 6; ++i) {
        values[i] = text[i].replace(',', '.').toDouble(&doubleOk);
        if (!doubleOk) break;
    }

    QString doubleResult = "не удалось преобразовать";
    if (doubleOk) {
        const double determinant = (values[2] - values[0]) * (values[5] - values[1]) -
                                   (values[3] - values[1]) * (values[4] - values[0]);
        const int doubleSign = determinant > 0 ? 1 : determinant < 0 ? -1 : 0;
        doubleResult = signText(doubleSign);
    }

    ui->resultLabel->setText("Точный результат: <b>" + signText(exactSign) + "</b><br>"
                             "Результат double: " + doubleResult);
    ui->resultLabel->setStyleSheet("background: #f0fdf4; border: 1px solid #86efac; border-radius: 7px; padding: 10px; color: #14532d; font-size: 15px;");
}

void LongArithmeticPage::setCoordinates(const std::array<QString, 6>& values)
{
    for (int i = 0; i < 6; ++i) m_fields[i]->setText(values[i]);
    calculate();
}

void LongArithmeticPage::updateIfOnline()
{
    if (ui->onlineCheckBox->isChecked()) calculate();
}

void LongArithmeticPage::setNearLineExample()
{
    setCoordinates({"1", "1", "2", "2", "1", "1.000000000000000000000000000000000000000000000000000000000000000001"});
}

void LongArithmeticPage::setLargeExample()
{
    const QString huge = "1000000000000000000000000000000000000000000000000000000000000";
    setCoordinates({"0", "0", huge, huge, huge,
                    huge + ".0000000000000000000000000000000000000000000000000000000000001"});
}

void LongArithmeticPage::setInvalidExample()
{
    setCoordinates({"1", "1", "два", "2", "1", "3"});
}

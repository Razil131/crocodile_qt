#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "paintwidget.h"
#include <QColorDialog>
#include <QTimer>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);
    pressTimer = new QTimer(this);
    pressTimer->setInterval(100);

    connect(pressTimer, &QTimer::timeout, this, [=]() {
        duration += 100;
        qDebug() << "Кнопка зажата уже " << duration << " мс";
        update();
    });

    ui->DesignFrame->installEventFilter(this);
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::on_StartGameButton_clicked()
{
    ui->StartGameButton->hide();
}


void GameWindow::on_Word1Label_clicked()
{
    ui->Word1Label->hide();
    ui->Word2Label->hide();
    ui->Word3Label->hide();
    ui->TimeToChooseWordLabel->hide();
}


void GameWindow::on_BrushSizeSlider_valueChanged(int value)
{
    ui->Canvas->setWidth(value);
    ui->BrushSizeLabel->setText(QString::number(value));
}


void GameWindow::on_ChooseColorButton_clicked()
{
    QColor selectedColor = QColorDialog::getColor(Qt::black, this, "Выберете цвет");
    if(selectedColor.isValid()){
        ui->Canvas->setColor(selectedColor);
        ui->ChooseColorButton->setStyleSheet(QString("background-color: %1").arg(selectedColor.name()));
    }
}


void GameWindow::on_FillingButton_clicked(bool checked)
{
    if(checked){
        ui->FillingButton->setProperty("state", "active");
        qDebug() << "режим заливки";
    }
    else {
        qDebug() << "режим рисования";
        ui->FillingButton->setProperty("state", "normal");
    }
    ui->Canvas->setFillMode(checked);

    ui->FillingButton->style()->unpolish(ui->FillingButton);
    ui->FillingButton->style()->polish(ui->FillingButton);
}


void GameWindow::on_EraseButton_pressed()
{
    duration = 0;
    pressTimer->start();
}

void GameWindow::on_EraseButton_released()
{
    pressTimer->stop();
    if(duration > 1000){
        ui->Canvas->clearAll();
        qDebug() << "FFFFFF";
        update();
    }

    duration = 0;
    ui->DesignFrame->update();
}

bool GameWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->DesignFrame && event->type() == QEvent::Paint) {
        if (duration > 0) {
            QPainter painter(ui->DesignFrame);
            painter.setRenderHint(QPainter::Antialiasing);

            QRect btnRect = ui->EraseButton->geometry();

            QPen pen(QColor(0, 255, 0), 5);
            pen.setCapStyle(Qt::RoundCap);
            painter.setPen(pen);

            int startAngle = 90 * 16;
            int spanAngle = -(duration * (360 * 16) / 1000);

            painter.drawArc(btnRect.adjusted(-5, -5, 5, 5), startAngle, spanAngle);
        }
        return false;
    }

    return QMainWindow::eventFilter(obj, event);
}

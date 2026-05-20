#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "paintwidget.h"
#include <QColorDialog>
#include <QTimer>


GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow), player("")
{
    ui->setupUi(this);
    connect(ui->InputChat, &QLineEdit::returnPressed, this, &GameWindow::on_EnterChat_released);
    QString ipStr = "IP: " + QString::fromStdString(controller.getIP());
    ui->IPLabel->setText(ipStr);
    pressTimer = new QTimer(this);
    gameTimer = new QTimer(this);
    wordTimer = new QTimer(this);
    pressTimer->setInterval(100);

    connect(pressTimer, &QTimer::timeout, this, [=]() {
        duration += 100;
        qDebug() << "Кнопка зажата уже " << duration << " мс";
        update();
    });


    connect(gameTimer, &QTimer::timeout, this, [this]() {
        ui->TimeLeftLabel->setText( QString::number(controller.getTimeLeft()));
        ui->TimeLeftLabel->update();
        ui->TimeLeftBar->setValue(controller.getTimeLeft());
    });

    connect(wordTimer, &QTimer::timeout, this, [this]() {
        ui->WordLabel->setText(getWordLabelStr(controller.getOpenedLetters()));
    });

    ui->DesignFrame->installEventFilter(this);
    ui->WordLabel->hide();


}

GameWindow::~GameWindow()
{
    delete ui;
}

QString GameWindow::getWordLabelStr(std::vector<std::string> letters){
    std::string wordLabelStr = "";
    for(auto l : letters){
        wordLabelStr += l;
        wordLabelStr += " ";
    }
    QString output = QString::fromStdString(wordLabelStr);
    return output;
}

void GameWindow::on_StartGameButton_clicked()
{
    if(controller.isExplainer(&player)){
        ui->StartGameButton->hide();
    }
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


void GameWindow::on_EnterChat_released()
{
    QString input = ui->InputChat->text();
    ui->InputChat->clear();
    qDebug() << "игрок с ником " << player.name() << " отправил сообщение";
    if(!input.isEmpty()){
        controller.sendMessage(player, input.toStdString());
    }
    chatUpdate();
}

void GameWindow::chatUpdate(){
    ui->ChatList->clear();
    auto chatHistory = controller.getChatHistory();
    for (auto p : chatHistory){
        ui->ChatList->addItem(QString("%1:  %2").arg(p.first, p.second));
    }
}

void GameWindow::startDraw(){
    ui->WordLabel->show();
    ui->Word1Label->hide();
    ui->Word2Label->hide();
    ui->Word3Label->hide();
    ui->TimeToChooseWordLabel->hide();
    ui->TimeLeftBar->setRange(0, controller.getRoundTime());
    ui->TimeLeftBar->setValue(controller.getRoundTime());
    auto word = controller.getOpenedLetters();
    ui->WordLabel->setText(getWordLabelStr(controller.getOpenedLetters()));
    gameTimer->start(1000);
    wordTimer->start(round(controller.getRoundTime()*1000/3));
}

void GameWindow::on_Word2Label_clicked()
{
    startDraw();
}


void GameWindow::on_Word3Label_clicked()
{
    startDraw();
}

void GameWindow::on_Word1Label_clicked()
{
    startDraw();
}

void GameWindow::setPlayer(Player* plr){
    player = *plr;
    delete plr;
}

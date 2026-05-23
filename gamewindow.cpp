#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "paintwidget.h"
#include <QColorDialog>
#include <QTimer>


GameWindow::GameWindow(GameController* ctrl, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
    , player(nullptr)
    , controller(ctrl)
{
    ui->setupUi(this);
    connect(ui->InputChat, &QLineEdit::returnPressed, this, &GameWindow::on_EnterChat_released);
    QString ipStr = "IP: " + QString::fromStdString(controller->getIP());
    ui->IPLabel->setText(ipStr);

    pressTimer = new QTimer(this);
    gameTimer = new QTimer(this);
    wordTimer = new QTimer(this);
    chooseWordTimer = new QTimer(this);
    chooseWordTimer->setInterval(1000);
    pressTimer->setInterval(100);

    connect(pressTimer, &QTimer::timeout, this, [=]() {
        duration += 100;
        qDebug() << "Кнопка зажата уже " << duration << " мс";
        update();
    });

    connect(chooseWordTimer, &QTimer::timeout, this, [this](){
        timeForChooseWord += 1000;
        ui->TimeToChooseWordLabel->setText(QString::number((10000 - timeForChooseWord)/1000));
    if (timeForChooseWord >= 10000) {
            chooseWordTimer->stop();
            std::string word = ui->Word1Label->text().toStdString();
            controller->setWord(word);
            startDraw();
        }
    });


    connect(gameTimer, &QTimer::timeout, this, [this]() {
        ui->TimeLeftLabel->setText( QString::number(controller->getTimeLeft()));
        ui->TimeLeftLabel->update();
        ui->TimeLeftBar->setValue(controller->getTimeLeft());
        playersTableUpdate();
        controller->updateOpenedLetters();
        if(controller->getTimeLeft() <= 0){

        }
    });

    connect(wordTimer, &QTimer::timeout, this, [this]() {
        ui->WordLabel->setText(getWordLabelStr(controller->getOpenedLetters()));
    });

    tableCreate();
    showWordsOnButtons();
}

GameWindow::~GameWindow()
{
    delete ui;
}

QString GameWindow::getWordLabelStr(std::vector<std::string> letters){
    QString output;
    if(!controller->isExplainer(*player)){
        std::string wordLabelStr = "";
        for(auto l : letters){
            wordLabelStr += l;
            wordLabelStr += " ";
        }
        output = QString::fromStdString(wordLabelStr);
    }
    else{
        output = QString::fromStdString(controller->getWord());
    }
    return output;
}

void GameWindow::on_StartGameButton_clicked()
{
    if(controller->isExplainer(*player)){
        ui->StartGameButton->hide();
        chooseWordTimer->start(1000);
    }
}

void GameWindow::on_BrushSizeSlider_valueChanged(int value)
{
    ui->Canvas->setWidth(value);
    ui->BrushSizeLabel->setText(QString::number(value));
}



void GameWindow::on_ChooseColorButton_clicked()
{
    if(controller->isExplainer(*player)){
        QColor selectedColor = QColorDialog::getColor(Qt::black, this, "Выберете цвет");
        if(selectedColor.isValid()){
            ui->Canvas->setColor(selectedColor);
            ui->ChooseColorButton->setStyleSheet(QString("background-color: %1").arg(selectedColor.name()));
    }
    }
}


void GameWindow::on_FillingButton_clicked(bool checked)
{
    if(controller->isExplainer(*player)){
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
}


void GameWindow::on_EraseButton_pressed()
{
    if(controller->isExplainer(*player)){
        duration = 0;
        pressTimer->start();
    }
}

void GameWindow::on_EraseButton_released()
{
    if(controller->isExplainer(*player)){
        pressTimer->stop();
        if(duration > 1000){
            ui->Canvas->clearAll();
            qDebug() << "FFFFFF";
            update();
        }

        duration = 0;
        ui->DesignFrame->update();
    }
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
    qDebug() << "игрок с ником " << player->name() << " отправил сообщение";
    if(!input.isEmpty()){
        controller->sendMessage(*player, input.toStdString());
    }
    chatUpdate();
}

void GameWindow::chatUpdate(){
    ui->ChatList->clear();
    auto chatHistory = controller->getChatHistory();
    for (auto p : chatHistory){
        ui->ChatList->addItem(QString("%1:  %2").arg(p.first, p.second));
    }
}

void GameWindow::startDraw(){
    ui->BrushSizeSlider->setEnabled(controller->isExplainer(*player));
    ui->Canvas->setDrawingEnabled(controller->isExplainer(*player));
    ui->specialFrame->hide();
    ui->WordLabel->show();
    ui->Word1Label->hide();
    ui->Word2Label->hide();
    ui->Word3Label->hide();
    ui->TimeToChooseWordLabel->hide();
    ui->TimeLeftBar->setRange(0, controller->getRoundTime());
    ui->TimeLeftBar->setValue(controller->getRoundTime());
    auto word = controller->getOpenedLetters();
    ui->WordLabel->setText(getWordLabelStr(controller->getOpenedLetters()));
    gameTimer->start(1000);
    wordTimer->start(round(controller->getRoundTime()*1000/3));
    controller->startRound();
    if (controller->getRound()!=1){
        controller->nextExplainer();
    }
}

void GameWindow::on_Word1Label_clicked()
{
    std::string word = ui->Word1Label->text().toStdString();
    controller->setWord(word);
    startDraw();
}

void GameWindow::on_Word2Label_clicked()
{
    std::string word = ui->Word2Label->text().toStdString();
    controller->setWord(word);
    startDraw();
}


void GameWindow::on_Word3Label_clicked()
{
    std::string word = ui->Word3Label->text().toStdString();
    controller->setWord(word);
    startDraw();
}


void GameWindow::setPlayer(Player* plr){
    player = plr;
    controller->addPlayer(*player);
    qDebug() << player->id();
    ui->Canvas->setDrawingEnabled(controller->isExplainer(*player));
    ui->BrushSizeSlider->setEnabled(controller->isExplainer(*player));
    playersTableUpdate();
}

void GameWindow::playersTableUpdate(){
    ui->PlayersTable->setUpdatesEnabled(false);
    ui->PlayersTable->clearContents();
    std::vector<Player> players = controller->getPlayers();
    ui->PlayersTable->setRowCount(players.size());

    for (size_t row = 0; row < players.size(); ++row) {
        const auto& player = players[row];
        QString nick = QString::fromStdString(player.name());
        ui->PlayersTable->setItem(row, 0, new QTableWidgetItem(nick));

        if(controller->isExplainer(player)){
        QString status = QString::fromStdString("Рисующий");
        ui->PlayersTable->setItem(row, 1, new QTableWidgetItem(status));
    }
        else{
            QString status = QString::fromStdString("Игрок");
            ui->PlayersTable->setItem(row, 1, new QTableWidgetItem(status));
        }
        QString scores = QString::number(player.score());
        ui->PlayersTable->setItem(row, 2, new QTableWidgetItem(scores));
        ui->PlayersTable->item(row, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        ui->PlayersTable->item(row, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignTop);
        ui->PlayersTable->item(row, 2)->setTextAlignment(Qt::AlignRight | Qt::AlignTop);
        qDebug() << player.name();
        }
    ui->PlayersTable->setUpdatesEnabled(true);
}

void GameWindow::tableCreate(){
    ui->DesignFrame->installEventFilter(this);
    ui->WordLabel->hide();
    ui->PlayersTable->setColumnCount(3);
    ui->PlayersTable->setShowGrid(false);
    ui->PlayersTable->setFrameShape(QFrame::NoFrame);
    ui->PlayersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->PlayersTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->PlayersTable->verticalHeader()->setVisible(false);
    ui->PlayersTable->horizontalHeader()->setVisible(false);
    ui->PlayersTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PlayersTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PlayersTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->PlayersTable->verticalHeader()->setDefaultSectionSize(35);
    ui->PlayersTable->horizontalHeader()->setStretchLastSection(false);
    ui->PlayersTable->horizontalHeader()->setSectionsMovable(false);
    ui->PlayersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->PlayersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->PlayersTable->setColumnWidth(1, 80);
    ui->PlayersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->PlayersTable->setColumnWidth(2, 60);
}

void GameWindow::showWordsOnButtons(){
    auto words = controller->getWordsForChoose();
    ui->Word1Label->setText(QString::fromStdString(std::get<0>(words)));
    ui->Word2Label->setText(QString::fromStdString(std::get<1>(words)));
    ui->Word3Label->setText(QString::fromStdString(std::get<2>(words)));
}

void GameWindow::showRound(){
    QString round = "Раунд " + QString::number(controller->getRound()) + "/" + QString::number(controller->getRoundCount());
    ui->RoundNumLabel->setText(round);
}


//todo
/*
Сделать раунд +
Сделать таймер на выбор слова +
Статус в таблице +
Когда на таймере 0, выводить старт, выбор слова (Не могу сделать пока нет функции для передачи ведущего)
Запретить выбирать слова и рисовать всем кроме ведущего + (вроде с рисованием должно работать)
Запретить менять цвет, заливку и очищать всем кроме ведущего +
убрать возможность пустой ник оставлять +
Добавить слоты чтобы обновлялось само
*/

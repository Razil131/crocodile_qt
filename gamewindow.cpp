#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QColorDialog>
#include <QDebug>

GameWindow::GameWindow(GameController* ctrl, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
    , controller(ctrl)
{
    ui->setupUi(this);

    connect(ui->InputChat, &QLineEdit::returnPressed, this, &GameWindow::on_EnterChat_released);

    ui->IPLabel->setText("IP: " + controller->getIP());

    pressTimer = new QTimer(this);
    pressTimer->setInterval(100);

    connect(controller, &GameController::chatUpdated, this, &GameWindow::chatUpdate);
    connect(controller, &GameController::playersUpdated, this, &GameWindow::playersTableUpdate);
    connect(controller, &GameController::wordsForChooseReady, this, &GameWindow::onWordChooseStarted);
    connect(controller, &GameController::roundStarted, this, &GameWindow::onRoundStarted);
    connect(controller, &GameController::openedLettersUpdated, this, &GameWindow::onOpenedLettersUpdated);
    connect(controller, &GameController::timerUpdated, this, &GameWindow::onGameTimerUpdated);
    connect(controller, &GameController::wordTimerUpdated, this, &GameWindow::onWordTimerUpdated);
    connect(controller, &GameController::roundEnded, this, &GameWindow::onRoundEnded);
    connect(controller, &GameController::explainerUpdated, this, &GameWindow::onExplainerUpdated);
    connect(controller, &GameController::messageReceived, this, &GameWindow::onMessageReceived);
    connect(controller, &GameController::gameEnded, this, &GameWindow::onGameEnded);

    connect(ui->Canvas, &PaintWidget::commandGenerated, controller->draw(), &DrawController::broadcastCommand);
    connect(controller, &GameController::drawCommandReceived, ui->Canvas, &PaintWidget::executeCommand);

    connect(pressTimer, &QTimer::timeout, this, [=]() {
        duration += 100;
        update();
    });

    tableCreate();
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::setPlayer(int assignedId) {
    playerId_ = assignedId;
    Player& myPlayer = controller->players()->getPlayerById(playerId_);

    bool isMeExplainer = controller->players()->isExplainer(myPlayer);
    ui->Canvas->setDrawingEnabled(isMeExplainer);
    ui->BrushSizeSlider->setEnabled(isMeExplainer);

    playersTableUpdate();
}

QString GameWindow::getWordLabelStr(const QList<QString>& letters) {
    if (!controller->players()->isExplainerByID(playerId_) &&
        !controller->players()->getPlayerById(playerId_).isCurrentWordGuessed())
    {
        QString wordLabelStr = "";
        for (const auto& l : letters) {
            wordLabelStr += l + " ";
        }
        return wordLabelStr.trimmed();
    }

    return controller->getWord();
}

void GameWindow::on_StartGameButton_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        ui->StartGameButton->hide();
        controller->round()->startWordChooseAndRound();
    }
}

void GameWindow::on_BrushSizeSlider_valueChanged(int value)
{
    ui->Canvas->setWidth(value);
    ui->BrushSizeLabel->setText(QString::number(value));
}

void GameWindow::on_ChooseColorButton_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        QColor selectedColor = QColorDialog::getColor(Qt::black, this, "Выберите цвет");
        if (selectedColor.isValid()) {
            ui->Canvas->setColor(selectedColor);
            ui->ChooseColorButton->setStyleSheet(QString("background-color: %1").arg(selectedColor.name()));
        }
    }
}

void GameWindow::on_FillingButton_clicked(bool checked)
{
    if (controller->players()->isExplainerByID(playerId_)) {
        if (checked) {
            ui->FillingButton->setProperty("state", "active");
        } else {
            ui->FillingButton->setProperty("state", "normal");
        }
        ui->Canvas->setFillMode(checked);

        ui->FillingButton->style()->unpolish(ui->FillingButton);
        ui->FillingButton->style()->polish(ui->FillingButton);
    }
}

void GameWindow::on_EraseButton_pressed()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        duration = 0;
        pressTimer->start();
    }
}

void GameWindow::on_EraseButton_released()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        pressTimer->stop();
        if (duration > 1000) {
            ui->Canvas->clearAll();
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
    QString input = ui->InputChat->text().trimmed();
    ui->InputChat->clear();

    if (!input.isEmpty()) {
        Player& myPlayer = controller->players()->getPlayerById(playerId_);
        controller->chat()->sendMessage(myPlayer, input);
    }
}

void GameWindow::chatUpdate() {
    //
}

void GameWindow::onMessageReceived(int senderId, const QString& senderName, const QString& text) {
    ui->ChatList->addItem(QString("%1: %2").arg(senderName, text));
    ui->ChatList->scrollToBottom();
}

void GameWindow::on_Word1Label_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        controller->round()->setWord(ui->Word1Label->text());
    }
}

void GameWindow::on_Word2Label_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        controller->round()->setWord(ui->Word2Label->text());
    }
}

void GameWindow::on_Word3Label_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        controller->round()->setWord(ui->Word3Label->text());
    }
}

void GameWindow::playersTableUpdate() {
    ui->PlayersTable->setUpdatesEnabled(false);
    ui->PlayersTable->clearContents();

    const QList<Player>& players = controller->getPlayers();
    ui->PlayersTable->setRowCount(players.size());

    for (size_t row = 0; row < players.size(); ++row) {
        const auto& player = players[row];

        ui->PlayersTable->setItem(row, 0, new QTableWidgetItem(player.name()));

        QString status = controller->players()->isExplainer(player) ? "Рисующий" : "Игрок";
        ui->PlayersTable->setItem(row, 1, new QTableWidgetItem(status));

        ui->PlayersTable->setItem(row, 2, new QTableWidgetItem(QString::number(player.score())));

        ui->PlayersTable->item(row, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        ui->PlayersTable->item(row, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignTop);
        ui->PlayersTable->item(row, 2)->setTextAlignment(Qt::AlignRight | Qt::AlignTop);
    }
    ui->PlayersTable->setUpdatesEnabled(true);
}

void GameWindow::tableCreate() {
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

void GameWindow::showRound() {
    QString round = QString("Раунд %1/%2").arg(QString::number(controller->getRound()), QString::number(controller->getRoundCount()));
    ui->RoundNumLabel->setText(round);
}

void GameWindow::onWordChooseStarted(const QString& w1, const QString& w2, const QString& w3) {
    ui->StartGameButton->hide();
    ui->WordLabel->hide();
    ui->specialFrame->show();

    if (controller->players()->isExplainerByID(playerId_)) {
        ui->Word1Label->setText(w1);
        ui->Word2Label->setText(w2);
        ui->Word3Label->setText(w3);

        ui->Word1Label->show();
        ui->Word2Label->show();
        ui->Word3Label->show();
        ui->TimeToChooseWordLabel->show();
    } else {
        ui->Word1Label->setText("Ведущий выбирает слово...");
        ui->Word1Label->show();

        ui->Word2Label->hide();
        ui->Word3Label->hide();
        ui->TimeToChooseWordLabel->hide();
    }
}

void GameWindow::onRoundStarted(int roundNum, const QString& wordToDraw) {

    bool isMeExplainer = controller->players()->isExplainerByID(playerId_);
    if (!isMeExplainer) {
        ui->FillingButton->hide();
        ui->BrushSizeSlider->hide();
        ui->BrushSizeLabel->hide();
        ui->BrushSizeLabel_tip->hide();
        ui->ChooseColorButton->hide();
        ui->EraseButton->hide();
    } else {
        ui->FillingButton->show();
        ui->BrushSizeSlider->show();
        ui->BrushSizeLabel->show();
        ui->BrushSizeLabel_tip->show();
        ui->ChooseColorButton->show();
        ui->EraseButton->show();
    }

    ui->BrushSizeSlider->setEnabled(isMeExplainer);
    ui->Canvas->setDrawingEnabled(isMeExplainer);
    ui->Word1Label->hide();
    ui->Word2Label->hide();
    ui->Word3Label->hide();
    ui->TimeToChooseWordLabel->hide();
    ui->specialFrame->hide();
    ui->WordLabel->show();

    ui->TimeLeftBar->setRange(0, controller->getRoundTime());
    ui->TimeLeftBar->setValue(controller->getRoundTime());

    ui->WordLabel->setText(getWordLabelStr(controller->getOpenedLetters()));
    showRound();
}

void GameWindow::onOpenedLettersUpdated(const QList<QString>& openedLetters) {
    ui->WordLabel->setText(getWordLabelStr(openedLetters));
}

void GameWindow::onGameTimerUpdated(std::time_t timeLeft) {
    int timeInt = static_cast<int>(timeLeft);
    ui->TimeLeftLabel->setText(QString::number(timeInt));
    ui->TimeLeftBar->setValue(timeInt);
}

void GameWindow::onWordTimerUpdated(std::time_t timeLeft) {
    ui->TimeToChooseWordLabel->setText(QString::number(static_cast<int>(timeLeft)));
}

void GameWindow::onRoundEnded() {
    ui->Canvas->setDrawingEnabled(false);
    ui->BrushSizeSlider->setEnabled(false);
    ui->Canvas->clearAll();
    ui->ChatList->addItem("Система: Раунд окончен! Подсчет очков...");
    playersTableUpdate();
}

void GameWindow::onExplainerUpdated(int newExplainerId) {

    Player& myPlayer = controller->players()->getPlayerById(playerId_);
    bool isMeExplainer = controller->players()->isExplainer(myPlayer);

    ui->Canvas->setDrawingEnabled(isMeExplainer);
    ui->BrushSizeSlider->setEnabled(isMeExplainer);

    playersTableUpdate();
}

void GameWindow::onGameEnded(){
    tableInChat();
    qDebug() << "onGameEnded вызван!";
    playersTableUpdate();
    ui->StartGameButton->show();
}

void GameWindow::tableInChat(){
    const QList<Player>& players = controller->getPlayers();
    for (size_t i = 0; i < players.size(); ++i) {
        const auto& player = players[i];
        QString output = "Игрок " + player.name() + " заработал: " + QString::number(player.score()) + " очков!";
        controller->chat()->messageReceived(-1, "Система", output);
    }
}

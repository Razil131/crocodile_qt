#include "gamewindow.h"
#include "ui_gamewindow.h"


GameWindow::GameWindow(GameController* ctrl, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
    , controller(ctrl)
{
    ui->setupUi(this);
    setWindowTitle(tr("Крокодил"));

    connect(ui->InputChat, &QLineEdit::returnPressed, this, &GameWindow::on_EnterChat_released);

    pressTimer = new QTimer(this);
    pressTimer->setInterval(100);
    connect(controller, &GameController::playersUpdated, this, &GameWindow::playersTableUpdate);
    connect(controller, &GameController::wordsForChooseReady, this, &GameWindow::onWordChooseStarted);
    connect(controller, &GameController::roundStarted, this, &GameWindow::onRoundStarted);
    connect(controller, &GameController::openedLettersUpdated, this, &GameWindow::onOpenedLettersUpdated);
    connect(controller, &GameController::wordTimerUpdated, this, &GameWindow::onWordTimerUpdated);
    connect(controller, &GameController::roundEnded, this, &GameWindow::onRoundEnded);
    connect(controller, &GameController::explainerUpdated, this, &GameWindow::onExplainerUpdated);
    connect(controller, &GameController::messageReceived, this, &GameWindow::onMessageReceived);
    connect(controller, &GameController::gameEnded, this, &GameWindow::onGameEnded);

    connect(ui->Canvas, &PaintWidget::commandGenerated, controller, &GameController::sendDrawCommand);
    connect(controller, &GameController::drawCommandReceived, ui->Canvas, &PaintWidget::executeCommand);
    connect(controller, &GameController::serverDisconnected, this, [this]() {
        QMessageBox::critical(this, "ERROR", tr("Потеряно соединение с сервером"));
        this->close();
    });
    
    connect(controller, &GameController::connectionFailed, this, [this](const QString& error) {
         QMessageBox::critical(this, "ERROR", error);
        this->close();
    });

    connect(pressTimer, &QTimer::timeout, this, [=]() {
        duration += 100;
        update();
    });

    tableCreate();

    QTimer* uiTimer = new QTimer(this);
    connect(uiTimer, &QTimer::timeout, this, [this]() {
        if (controller->getRound() > 0) {
            this->onGameTimerUpdated(controller->getTimeLeft());
        } else {
            ui->TimeLeftLabel->setText("--");
            ui->TimeLeftBar->setValue(0);
        }
    });
    uiTimer->start(500);
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

    if (!isMeExplainer) {
        ui->StartGameButton->setEnabled(0);
        ui->StartGameButton->setText(tr("Ожидание хоста..."));
    } else if(controller->getRound() == 0) {
         ui->StartGameButton->show();
    } else {
        ui->StartGameButton->hide();
    }

    playersTableUpdate();
}

void GameWindow::setHostMode(bool isHost) {
    ui->IPLabel->setVisible(isHost);
}

void GameWindow::updateIPlabel(){
    ui->IPLabel->setText(controller->getIPandPort());
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

void GameWindow::closeEvent(QCloseEvent *event) {
    if (controller) {
        controller->shutdownNetwork();
    }
    event->accept();
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Undo) ||
        (event->key() == Qt::Key_Z && (event->modifiers() & Qt::ControlModifier)))
    {
        if (controller->players()->isExplainerByID(playerId_)) {
            ui->Canvas->undo();
        }
    }
    QMainWindow::keyPressEvent(event);
}

void GameWindow::on_StartGameButton_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        if (controller->getPlayers().size() >= 2) {
            ui->StartGameButton->hide();
            controller->startGame();
        } else {
            QListWidgetItem* item = new QListWidgetItem(tr("Система: Недостаточно игроков для начала игры (минимум 2)."));
            item->setForeground(QColor("#dc2626"));
            item->setBackground(QColor("#fee2e2"));
            
            QFont font = item->font();
            font.setBold(true);   
            item->setFont(font);
            
            ui->ChatList->addItem(item);
            ui->ChatList->scrollToBottom();
        }
    }
}

void GameWindow::on_Word1Label_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        controller->selectWord(ui->Word1Label->text());
    }
}

void GameWindow::on_Word2Label_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        controller->selectWord(ui->Word2Label->text());
    }
}

void GameWindow::on_Word3Label_clicked()
{
    if (controller->players()->isExplainerByID(playerId_)) {
        controller->selectWord(ui->Word3Label->text());
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
        QColor selectedColor = QColorDialog::getColor(Qt::black, this, tr("Выберите цвет"));
        if (selectedColor.isValid()) {
            ui->Canvas->setColor(selectedColor);
            QString colorStyle = QString(
                "background-color: %1; "
                "border-radius: 20px; "
                "border: 2px solid #e5e7eb;"
            ).arg(selectedColor.name());

            ui->ChooseColorButton->setStyleSheet(colorStyle);
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

void GameWindow::on_EnterChat_released()
{
    QString input = ui->InputChat->text().trimmed();
    ui->InputChat->clear();

    if (!input.isEmpty()) {
        controller->sendChatMessage(input);
    }
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
        ui->Word1Label->setText(tr("Ведущий выбирает слово..."));
        ui->Word1Label->show();

        ui->Word2Label->hide();
        ui->Word3Label->hide();
        ui->TimeToChooseWordLabel->hide();
    }
}

void GameWindow::onRoundStarted(int roundNum, const QString& wordToDraw) {
    ui->Canvas->clearHistory();
    ui->StartGameButton->hide();

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
    ui->Canvas->clearHistory();
    addSystemMessage(tr("Система: Раунд окончен! Подсчет очков..."), MsgType::Info);
    playersTableUpdate();
}

void GameWindow::onExplainerUpdated(int newExplainerId) {

    Player& myPlayer = controller->players()->getPlayerById(playerId_);
    bool isMeExplainer = controller->players()->isExplainer(myPlayer);

    ui->Canvas->setDrawingEnabled(isMeExplainer);
    ui->BrushSizeSlider->setEnabled(isMeExplainer);

    playersTableUpdate();
}

void GameWindow::onMessageReceived(int senderId, const QString& senderName, const QString& text) {
    if (senderId == -1 || senderName == tr("Система")) {
        MsgType type = MsgType::Success;     
        addSystemMessage(QString("%1: %2").arg(senderName, text), type);      
    } else {
        ui->ChatList->addItem(QString("%1: %2").arg(senderName, text));
        ui->ChatList->scrollToBottom();
    }
}

void GameWindow::onGameEnded(){
    tableInChat();
    playersTableUpdate();
    ui->StartGameButton->show();
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

void GameWindow::showRound() {
    QString round = QString(tr("Раунд %1/%2")).arg(QString::number(controller->getRound()), QString::number(controller->getRoundCount()));
    ui->RoundNumLabel->setText(round);
}

void GameWindow::playersTableUpdate() {
    ui->PlayersTable->setUpdatesEnabled(false);
    ui->PlayersTable->clearContents();

    const QList<Player>& players = controller->getPlayers();
    ui->PlayersTable->setRowCount(players.size());

    for (int row = 0; row < players.size(); ++row) {
        const auto& player = players[row];
        bool isExplainer = controller->players()->isExplainer(player);

        QTableWidgetItem* nameItem = new QTableWidgetItem(player.name());
        
        QString status = isExplainer ? tr("Рисующий") : tr("Игрок");
        QTableWidgetItem* statusItem = new QTableWidgetItem(status);
        
        QTableWidgetItem* scoreItem = new QTableWidgetItem(QString::number(player.score()));

        nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        statusItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        scoreItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        if (isExplainer) {
            QColor rowBg("#e0e7ff");
            nameItem->setBackground(rowBg);
            statusItem->setBackground(rowBg);
            scoreItem->setBackground(rowBg);

            statusItem->setForeground(QColor("#4f46e5"));
            QFont font = statusItem->font();
            font.setBold(true);
            statusItem->setFont(font);
            
            nameItem->setFont(font);
            scoreItem->setFont(font);
        } else {
            statusItem->setForeground(QColor("#4b5563"));
        }

        ui->PlayersTable->setItem(row, 0, nameItem);
        ui->PlayersTable->setItem(row, 1, statusItem);
        ui->PlayersTable->setItem(row, 2, scoreItem);
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
    ui->PlayersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->PlayersTable->setColumnWidth(0, 100); 
    ui->PlayersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->PlayersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->PlayersTable->setColumnWidth(2, 60);
}

void GameWindow::tableInChat(){
    const QList<Player>& players = controller->getPlayers();
    for (const auto& player : players) {
        QString output = QString(tr("Система: Игрок %1 заработал: %2 очков!"))
                         .arg(player.name(), QString::number(player.score()));
        
        addSystemMessage(output, MsgType::Success);
    }
    ui->ChatList->scrollToBottom();
}

void GameWindow::addSystemMessage(const QString& text, MsgType type)
{
    QListWidgetItem* item = new QListWidgetItem(text);
    QFont font = item->font();
    
    switch (type) {
        case MsgType::Success:
            item->setForeground(QColor("#16a34a"));
            item->setBackground(QColor("#f0fdf4"));
            font.setBold(true);
            break;
            
        case MsgType::Warning:
            item->setForeground(QColor("#dc2626")); 
            item->setBackground(QColor("#fee2e2")); 
            font.setBold(true);
            break;
            
        case MsgType::Info:
        default:
            item->setForeground(QColor("#4f46e5"));
            item->setBackground(QColor("#e0e7ff"));
            font.setBold(true);
            break;
    }
    
    item->setFont(font);
    ui->ChatList->addItem(item);
    ui->ChatList->scrollToBottom();
}
#include "paintwidget.h"
#include "ui_paintwidget.h"
#include <QStack>

PaintWidget::PaintWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PaintWidget)
{
    ui->setupUi(this);
    canvas = QImage(500, 500, QImage::Format_RGB32);
    canvas.fill(Qt::white);
    isDrawing = false;
    fillMode = false;
    drawingEnabled = false;
}

PaintWidget::~PaintWidget()
{
    delete ui;
}

void PaintWidget::mousePressEvent(QMouseEvent *event) {
    if (!drawingEnabled) return;
    if(event->button() == Qt::LeftButton && fillMode){
        DrawCommand cmd;
        cmd.type = DrawCommand::Fill;
        cmd.x = event->pos().x();
        cmd.y = event->pos().y();
        cmd.color = currentColor;
        emit commandGenerated(cmd);
        return;
    }
    DrawCommand cmd;
    cmd.type = DrawCommand::Start;
    cmd.x = event->pos().x();
    cmd.y = event->pos().y();
    cmd.color = (event->button() == Qt::LeftButton ? currentColor : Qt::white);
    cmd.width = currentWidth;
    emit commandGenerated(cmd);
}


void PaintWidget::drawLineOnCanvas(const QPoint &from, const QPoint &to, const QColor &color, int width){
    QPainter imagePainter(&canvas);
    QPen pen;
    pen = QPen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    imagePainter.setPen(pen);
    if (from == to) {
        imagePainter.drawPoint(from);
    } else {
        imagePainter.drawLine(from, to);
    }
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!drawingEnabled || !isDrawing) return;

    DrawCommand cmd;
    cmd.type = DrawCommand::Move;
    cmd.x = event->pos().x();
    cmd.y = event->pos().y();
    cmd.color = currentStroke.color;
    cmd.width = currentStroke.width;
    emit commandGenerated(cmd);
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (!drawingEnabled) return;
    DrawCommand cmd;
    cmd.type = DrawCommand::End;
    emit commandGenerated(cmd);
}

void PaintWidget::drawStroke(QPainter &p, const Stroke &s){
    QPen pen(s.color, s.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p.setPen(pen);
    for(int j = 1; j < s.point_list.size(); j++){
        p.drawLine(s.point_list[j-1], s.point_list[j]);
    }
}

void PaintWidget::applyFill(QPoint start, QColor fillColor){ //заливка
    QColor startColor;
    if(canvas.rect().contains(start)){
        startColor = canvas.pixelColor(start);}
    else return;
    if(startColor == fillColor){return;}
    QStack<QPoint> stack;
    stack.push(start);

    while(!stack.isEmpty()){
        QPoint p = stack.pop();
        if (!canvas.rect().contains(p)) {
            continue;
        }
        if(canvas.pixelColor(p) == startColor){
            canvas.setPixelColor(p, fillColor);
            stack.push(QPoint(p.x() + 1, p.y()));
            stack.push(QPoint(p.x() - 1, p.y()));
            stack.push(QPoint(p.x(), p.y() + 1));
            stack.push(QPoint(p.x(), p.y() - 1));
        }
    }
    update();
}

void PaintWidget::paintEvent(QPaintEvent *event){ //ивент для рисования
    QPainter painter(this);
    painter.drawImage(0, 0, canvas);
}

void PaintWidget::clearAll() {
    DrawCommand cmd;
    cmd.type = DrawCommand::Clear;
    emit commandGenerated(cmd);
}

void PaintWidget::executeCommand(DrawCommand cmd) {
    switch (cmd.type) {
    case DrawCommand::Start:
        isDrawing = true;
        lastPoint = QPoint(cmd.x, cmd.y);

        currentStroke.color = cmd.color;
        currentStroke.width = cmd.width;
        currentStroke.point_list.clear();
        currentStroke.point_list.append(lastPoint);
        drawLineOnCanvas(lastPoint, lastPoint, cmd.color, cmd.width);
        break;

    case DrawCommand::Move:
        if (isDrawing) {
            QPoint newPoint(cmd.x, cmd.y);
            drawLineOnCanvas(lastPoint, newPoint, cmd.color, cmd.width);
            lastPoint = newPoint;
            currentStroke.point_list.append(newPoint);
        }
        break;

    case DrawCommand::End:
        isDrawing = false;
        history.push_back(currentStroke);
        currentStroke.point_list.clear();
        break;

    case DrawCommand::Clear:
        canvas.fill(Qt::white);
        history.clear();
        currentStroke.point_list.clear();
        isDrawing = false;
        break;

    case DrawCommand::Fill:
        applyFill(QPoint(cmd.x, cmd.y), cmd.color);
        break;
    }
    update();
}

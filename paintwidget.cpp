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

void PaintWidget::mousePressEvent(QMouseEvent *event){
    if (!drawingEnabled) return;
    if(event->button() == Qt::LeftButton && fillMode){
        QPoint start = event->pos();
        applyFill(start, currentColor);
        return;
    }


    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        qDebug() << "лкм нажата по координатам " << event->pos();
        currentStroke.color = (event->button() == Qt::LeftButton ? currentColor : Qt::white);
        currentStroke.width = currentWidth;
        lastPoint = event->pos();
        isDrawing = true;
        currentStroke.point_list.append(event->pos());
        drawLineOnCanvas(lastPoint, lastPoint, currentStroke.color, currentStroke.width);
        update();
    }
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

void PaintWidget::mouseMoveEvent(QMouseEvent *event){
    if (!drawingEnabled) return;
    if(isDrawing && (event->buttons() & (Qt::LeftButton | Qt::RightButton))){
        drawLineOnCanvas(lastPoint, event->pos(), currentStroke.color, currentStroke.width);

        lastPoint = event->pos();
        currentStroke.point_list.append(event->pos());
        update();
    }
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *event){
    if (!drawingEnabled) return;
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        qDebug() << "лкм отпущена по координатам" << event->pos();
        isDrawing = false;
        history.push_back(currentStroke);
        currentStroke.point_list.clear();
    }
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

void PaintWidget::clearAll(){
        qDebug() << " все очищено";
        history.clear();
        currentStroke.point_list.clear();
        canvas.fill(Qt::white);
        update();
        isDrawing = false;
}

//TODO щас заливка работает даже когда игра еще не начата так что исправлять надо потом

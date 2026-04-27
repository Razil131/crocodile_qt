#include "paintwidget.h"
#include "ui_paintwidget.h"

PaintWidget::PaintWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PaintWidget)
{
    ui->setupUi(this);
    canvas = QImage(2000, 2000, QImage::Format_RGB32);
    canvas.fill(Qt::white);
    isDrawing = false;
}

PaintWidget::~PaintWidget()
{
    delete ui;
}

void PaintWidget::mousePressEvent(QMouseEvent *event){
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

    else if(event->button() == Qt::MiddleButton) { //пока этот код тут, потом будет перенесен в отдельную кнопку в ui
        qDebug() << "нажата скм, все очищено";
        history.clear();
        currentStroke.point_list.clear();
        canvas.fill(Qt::white);
        update();
        isDrawing = false;
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
    if(isDrawing && (event->buttons() & (Qt::LeftButton | Qt::RightButton))){
        drawLineOnCanvas(lastPoint, event->pos(), currentStroke.color, currentStroke.width);

        lastPoint = event->pos();
        currentStroke.point_list.append(event->pos());
        update();
    }
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *event){
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

void PaintWidget::paintEvent(QPaintEvent *event){ //ивент для рисования
    QPainter painter(this);
    painter.drawImage(0, 0, canvas);
}



#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

namespace Ui {
class PaintWidget;
}

class PaintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PaintWidget(QWidget *parent = nullptr);
    ~PaintWidget();
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void setColor(const QColor c){currentColor = c;}
    void setWidth(int w) {currentWidth = w;}

private:
    Ui::PaintWidget *ui;

    QColor currentColor = Qt::black;
    int currentWidth = 10;

    struct Stroke{ //структура для хранения последнего мазка
        QList<QPoint> point_list;
        QColor color;
        int width;
    };

    QList<Stroke> history;
    QImage canvas;
    bool isDrawing;
    QPoint lastPoint;
    Stroke currentStroke;
    void paintEvent(QPaintEvent *event) override;
    void drawStroke(QPainter &p, const Stroke &s);
    void drawLineOnCanvas(const QPoint &from, const QPoint &to, const QColor &color, int width);

signals:
    void pointAdded(QPoint pos, QColor color, int width);

};

#endif // PAINTWIDGET_H

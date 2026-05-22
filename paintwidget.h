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
    void setFillMode(bool b){fillMode = b;}
    void applyFill(QPoint start, QColor fillColor);
    void clearAll();
    void setDrawingEnabled(bool enabled) { drawingEnabled = enabled; }
    bool isDrawingEnabled() const { return drawingEnabled; }

private:
    Ui::PaintWidget *ui;
    bool drawingEnabled;
    QColor currentColor = Qt::black;
    int currentWidth = 3;

    struct Stroke{ //структура для хранения последнего мазка
        QList<QPoint> point_list;
        QColor color;
        int width;
    };

    QList<Stroke> history;
    QImage canvas;
    bool isDrawing;
    bool fillMode;
    QPoint lastPoint;
    Stroke currentStroke;
    void paintEvent(QPaintEvent *event) override;
    void drawStroke(QPainter &p, const Stroke &s);
    void drawLineOnCanvas(const QPoint &from, const QPoint &to, const QColor &color, int width);

signals:
    void pointAdded(QPoint pos, QColor color, int width);

};

#endif // PAINTWIDGET_H

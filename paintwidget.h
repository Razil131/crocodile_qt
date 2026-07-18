#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include "drawCommand.hpp"

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
    void setFillMode(bool b){fillMode = b; updateCursor();}
    void applyFill(QPoint start, QColor fillColor);
    void clearAll();
    void setDrawingEnabled(bool enabled) { drawingEnabled = enabled; updateCursor(); }
    bool isDrawingEnabled() const { return drawingEnabled; }
    void executeCommand(DrawCommand cmd);
    void undo();
    void syncUndoToNetwork();

private:
    Ui::PaintWidget *ui;
    bool drawingEnabled;
    bool isReplayingHistory = false;
    QImage currentSnapshotBefore;
    QColor currentColor = Qt::black;
    int currentWidth = 3;

    struct Stroke{ //структура для хранения последнего мазка
        QList<QPoint> point_list;
        QColor color;
        int width;
    };

    struct PaintAction{
        enum Type{
            StrokeAction,
            FillAction,
            ClearAction
        };
        Type type;

        Stroke stroke;
        QImage snapshotBefore;
        DrawCommand originalCommand;
    };

    QList<PaintAction> history;
    QImage canvas;
    bool isDrawing;
    bool fillMode;
    QPoint lastPoint;
    Stroke currentStroke;
    void paintEvent(QPaintEvent *event) override;
    void drawStroke(QPainter &p, const Stroke &s);
    void drawLineOnCanvas(const QPoint &from, const QPoint &to, const QColor &color, int width);
    void resizeEvent(QResizeEvent *event);
    void updateCursor();

protected:
    void enterEvent(QEnterEvent *event) override;

signals:
    void pointAdded(QPoint pos, QColor color, int width);
    void commandGenerated(DrawCommand cmd);

};


#endif // PAINTWIDGET_H

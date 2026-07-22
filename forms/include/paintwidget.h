#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QImage>
#include <QList>
#include <QPoint>
#include <QColor>
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

    void setColor(const QColor c) { currentColor = c; }
    void setWidth(int w) { currentWidth = w; }
    void setFillMode(bool b) { fillMode = b; updateCursor(); }
    void setDrawingEnabled(bool enabled) { drawingEnabled = enabled; updateCursor(); }
    bool isDrawingEnabled() const { return drawingEnabled; }

    void applyFill(QPoint start, QColor fillColor);
    void clearAll();
    void clearHistory();
    void executeCommand(DrawCommand cmd);
    void undo();
    void syncUndoToNetwork();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEnterEvent *event) override;

signals:
    void pointAdded(QPoint pos, QColor color, int width);
    void commandGenerated(DrawCommand cmd);

private:
    struct Stroke {
        QList<QPoint> point_list;
        QColor color;
        int width;
    };

    struct PaintAction {
        enum Type {
            StrokeAction,
            FillAction,
            ClearAction
        };
        Type type;
        Stroke stroke;
        QImage snapshotBefore;
        DrawCommand originalCommand;
    };

    void drawStroke(QPainter &p, const Stroke &s);
    void drawLineOnCanvas(const QPoint &from, const QPoint &to, const QColor &color, int width);
    void updateCursor();

    Ui::PaintWidget *ui;

    bool drawingEnabled;
    bool isDrawing;
    bool fillMode;
    bool isReplayingHistory = false;
    
    int currentWidth = 3;
    QColor currentColor = Qt::black;
    QPoint lastPoint;
    Stroke currentStroke;

    QImage canvas;
    QImage currentSnapshotBefore;
    QList<PaintAction> history;
};

#endif // PAINTWIDGET_H

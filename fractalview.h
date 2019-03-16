#ifndef FRACTALVIEW_H
#define FRACTALVIEW_H

#include <QWidget>

typedef QVector<QLineF> Lines;

enum FractalType { DEFAULT, PI_SHAPED, ISLAND };

class FractalView : public QWidget
{
    Q_OBJECT

    FractalType type = FractalType::DEFAULT;

    Lines lineCoords;
    int iterationCount = 0;

    QTimer *animationTimer;

    qreal scaleFactor;
    QPointF viewOffset;

    QPointF dragStartPos;

    void setInitialState();
    void setInitialViewParams();
public:
    FractalView(QWidget *parent);
    void setFractalType(FractalType type);

public slots:
    void nextIter();
    void resetState();
    void toggleAnimation(bool enabled);
    void savePicture();

private slots:
    void nextFrame();
    void saveAnimation();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // FRACTALVIEW_H

#ifndef FRACTALVIEW_H
#define FRACTALVIEW_H

#include <QWidget>

typedef QVector<QLineF> Lines;

enum FractalType { DEFAULT, PI_SHAPED, ISLAND };

class FractalView : public QWidget
{
    Q_OBJECT

    FractalType type = FractalType::DEFAULT;
    qreal initialWidth = 20;
    qreal viewportWidth = 100;
    Lines lineCoords;
    int iterationCount = 0;

    QTimer *animationTimer;

    void setInitialState();
public:
    FractalView(QWidget *parent);
    void setFractalType(FractalType type);

public slots:
    void nextIter();
    void resetState();
    void setInitialWidth(int width);
    void setSquareWidth(int width);
    void toggleAnimation(bool enabled);

private slots:
    void nextFrame();

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // FRACTALVIEW_H

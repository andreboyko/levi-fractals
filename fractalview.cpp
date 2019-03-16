#include "fractalview.h"
#include <QPainter>
#include <QPen>
#include <QPaintEvent>
#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include "asmOpenCV.h"
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

FractalView::FractalView(QWidget *parent)
{
    Q_UNUSED(parent);
    setInitialState();
    setInitialViewParams();
    animationTimer = new QTimer(this);
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
}

void FractalView::resetState() {
    lineCoords.clear();
    setInitialState();
    setInitialViewParams();
    this->update();
}

void FractalView::setInitialState() {
    this->iterationCount = 0;

    QSize widgetSize = this->size();
    const int widgetSideLength = qMin(widgetSize.width(), widgetSize.height());
    const int initialWidth = qRound(0.4  * widgetSideLength);

    const int w2 = initialWidth / 2;
    switch(type) {
        case FractalType::ISLAND:
        lineCoords.push_back(QLineF(w2, w2, -w2, w2));
        [[clang::fallthrough]]; case FractalType::PI_SHAPED:
            lineCoords.push_back(QLineF(w2, -w2, w2, w2));
            lineCoords.push_back(QLineF(-w2, w2, -w2, -w2));
            lineCoords.push_back(QLineF(-w2, -w2, w2, -w2));
            break;
        case FractalType::DEFAULT:
            lineCoords.push_back(QLineF(-w2, 0, w2, 0));
            break;
    }
}

void FractalView::setInitialViewParams() {
    this->scaleFactor = 1;
    this->viewOffset = QPointF();
}

void FractalView::setFractalType(FractalType type) {
    this->type = type;
    resetState();
}

void FractalView::toggleAnimation(bool enabled) {
    if (enabled) {
        animationTimer->start(300);
    } else {
        animationTimer->stop();
    }
}

void FractalView::nextFrame() {
    if (iterationCount == 16) {
        resetState();
    } else {
        nextIter();
    }
}

void FractalView::nextIter() {
   Lines newLines;

   for (auto it = lineCoords.begin(); it != lineCoords.end(); ++it)
   {
       auto currentLine = *it;

       auto ss = currentLine.normalVector();
       ss.translate(currentLine.center() - currentLine.p1());
       ss.setLength(currentLine.length() / 2);

       auto newCentral = ss.p2();
       newLines.push_back(QLineF(currentLine.p1(), newCentral));
       newLines.push_back(QLineF(newCentral, currentLine.p2()));
   }
   lineCoords = newLines;
   iterationCount++;
   update();
}

void FractalView::saveAnimation() {
    savePicture();
}

void FractalView::savePicture() {
    QString saveFilePath = QFileDialog::getSaveFileName(this, tr("Save animation"), QString(), tr("Video (*.avi)"));

    if (saveFilePath.length() != 0) {
        const QSize size = this->grab().size();;
        const int videoFrameWidth = size.width();
        const int videoFrameHeight = size.height();


        cv::Mat cvMat;
        cv::VideoWriter video(saveFilePath.toStdString(),
                              cv::VideoWriter::fourcc('M','J','P','G'),
                              5,
                              cv::Size(videoFrameWidth, videoFrameHeight),
                              true);

        if (video.isOpened()) {
            resetState();
            for (int i = 0; i <= 16; i++) {
                if (i > 0)
                    nextFrame();

                cvMat = ASM::QPixmapToCvMat(this->grab(), false);
                cv::cvtColor(cvMat,cvMat,cv::COLOR_RGB2BGR);
                video << cvMat;
            }

            video.release();
            resetState();
            QMessageBox::information(this, tr("Animation saving"), tr("Animation successfully saved."));
        } else {
            QMessageBox::warning(this, tr("Animation saving"), tr("Something went wrong..."));
        }
    }
}

void FractalView::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    //set bg to white
    painter.fillRect(rect(), QBrush(Qt::white, Qt::SolidPattern));

    qreal penWidth = 1;
    if(qRound(scaleFactor) != 0)
        penWidth /= qRound(scaleFactor);

    QPen myPen(Qt::black, penWidth, Qt::SolidLine);
    painter.setPen(myPen);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawText(rect().left(), rect().bottom(), QString("Iteration #%1").arg(iterationCount + 1));

    painter.translate(width() / 2 + viewOffset.x(), height() / 2 + viewOffset.y());
    painter.scale(scaleFactor, scaleFactor);

    Lines::iterator it;
    for (it = lineCoords.begin(); it != lineCoords.end(); ++it) {
        painter.drawLine(*it);
    }
}

void FractalView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragStartPos = event->pos() - viewOffset;
    }
}

void FractalView::mouseMoveEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        viewOffset = event->pos() - dragStartPos;
        update();
    }
}

void FractalView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        setInitialViewParams();
        update();
    }
}

void FractalView::wheelEvent(QWheelEvent *event) {
    qreal scale = 1 + static_cast<qreal>(event->angleDelta().y()) / 1000;

    if (qAbs(scale) > 0.01) {
        this->scaleFactor *= scale;
        this->viewOffset = QPointF(viewOffset.x() * scale,
                                   viewOffset.y() * scale);
        update();
    }
}

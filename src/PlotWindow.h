#ifndef _PLOT_WINDOW_H
#define _PLOT_WINDOW_H

#include <QtWidgets/QMainWindow>

class ComtradeRecord;

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ChannelType {
        AnalogChannel = 0,
        DigitalChannel = 1
    };
    Q_ENUM(ChannelType)

    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();

    void setComtradeRecord(ComtradeRecord *comtradeRecord);
    bool loadSamples(QIODevice *samplesFile);

    void setRangeLower(int sample);
    void setRangeUpper(int sample);

    void addPlot(ChannelType type, int index, bool primary);

    bool savePng(const QString &fileName, int width = 0, int height = 0);

private:
    Ui::PlotWindow *ui;

    ComtradeRecord *m_comtradeRecord;
    QList<QVector<double>> m_analogSamples;
    QList<QVector<double>> m_digitalSamples;
    QVector<double> m_timeBaseus;
};

#endif

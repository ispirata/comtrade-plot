#include "PlotWindow.h"
#include "ui_plotwindow.h"

#include "ComtradeAnalogChannel.h"
#include "ComtradeDigitalChannel.h"
#include "ComtradeRecord.h"

#include <QtCore/QDebug>

PlotWindow::PlotWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlotWindow)
{
    ui->setupUi(this);
}

PlotWindow::~PlotWindow()
{
    delete ui;
}

void PlotWindow::setComtradeRecord(ComtradeRecord *comtradeRecord)
{
    m_comtradeRecord = comtradeRecord;
}

bool PlotWindow::loadSamples(QIODevice *samplesFile)
{
    if (!m_comtradeRecord) {
        qWarning() << "m_comtradeRecord is null";
        return false;
    }

    m_analogSamples.clear();
    m_digitalSamples.clear();
    m_timeBaseus.clear();

    int lastRate = m_comtradeRecord->ratesCount() - 1;
    int totalSamples = m_comtradeRecord->lastSample(lastRate);
    int channelsCount = m_comtradeRecord->channelsCount();
    int analogChannelsCount = m_comtradeRecord->analogChannelsCount();
    int digitalChannelsCount = m_comtradeRecord->digitalChannelsCount();

    // Pre-populate empty sample vectors
    for (int i = 0; i < analogChannelsCount; ++i) {
        QVector<double> samples;
        samples.resize(totalSamples);
        m_analogSamples.append(samples);
    }
    for (int i = 0; i < digitalChannelsCount; ++i) {
        QVector<double> samples;
        samples.resize(totalSamples);
        m_digitalSamples.append(samples);
    }

    m_timeBaseus.resize(totalSamples);
    bool hasFixedRate = m_comtradeRecord->ratesCount() > 0;

    // If we have fixed rates, we use this as timebase
    if (hasFixedRate) {
        int currentRate = 0;
        double currentSampling = m_comtradeRecord->sampleRate(currentRate);
        int currentLastSample = m_comtradeRecord->lastSample(currentRate);
        for (int i = 0; i < totalSamples; ++i) {
            if (i > currentLastSample) {
                currentRate++;
                currentSampling = m_comtradeRecord->sampleRate(currentRate);
                currentLastSample = m_comtradeRecord->lastSample(currentRate);
            }
            m_timeBaseus[i] = (1 / currentSampling) * i * 1000 - m_comtradeRecord->triggerOffsetms();
        }
    }

    QByteArray line;
    int lineCount = 0;
    for (line = samplesFile->readLine(); !line.isEmpty() && lineCount < totalSamples; line = samplesFile->readLine()) {
        QList<QByteArray> tokens = line.trimmed().split(',');
        // n, timestamp, 1 sample per channel
        if (tokens.length() != channelsCount + 2) {
            qWarning() << "Invalid data line: " << line << ", skipping it";
            continue;
        }

        if (!hasFixedRate) {
            int timestamp = tokens.value(1).toInt() * m_comtradeRecord->timeMultiplier();
            m_timeBaseus[lineCount] = timestamp;
        }

        for (int i = 0; i < analogChannelsCount; ++i) {
            const ComtradeAnalogChannel *analogChannel = m_comtradeRecord->analogChannel(i);
            double sample = analogChannel->multiplier() * tokens.value(i + 2).toDouble() + analogChannel->offset();
            m_analogSamples[i][lineCount] = sample;
        }

        for (int i = 0; i < digitalChannelsCount; ++i) {
            double sample = tokens.value(i + analogChannelsCount + 2).toDouble();
            m_digitalSamples[i][lineCount] = sample;
        }

        lineCount++;
    }

    if (lineCount != totalSamples) {
        qWarning() << "DAT files has" << lineCount << "lines, but" << totalSamples << "samples were expected";
        return false;
    }

    return true;
}

void PlotWindow::addPlot(ChannelType type, int index, bool primary)
{
    QVector<double> data;
    bool digital = type == DigitalChannel;
    QString label;
    if (digital) {
        if (index >= m_comtradeRecord->digitalChannelsCount()) {
            qWarning() << "Out of range digital channel index: " << index;
            return;
        }

        data = m_digitalSamples.value(index);
        label = m_comtradeRecord->digitalChannel(index)->id();
    } else {
        if (index >= m_comtradeRecord->analogChannelsCount()) {
            qWarning() << "Out of range analog channel index: " << index;
            return;
        }

        data = m_analogSamples.value(index);
        label = QStringLiteral("%1 (%2)").arg(m_comtradeRecord->analogChannel(index)->id())
                    .arg(m_comtradeRecord->analogChannel(index)->unit());
    }

    QCPGraph *graph = ui->customPlot->addGraph();
    if (!graph) {
        qWarning() << "Can't create graph";
        return;
    }

    if (primary) {
        graph->setPen(QPen(Qt::blue));
        ui->customPlot->yAxis->setLabelColor(Qt::blue);
        ui->customPlot->yAxis->setLabel(label);
    } else {
        graph->setPen(QPen(Qt::red));
        graph->setValueAxis(ui->customPlot->yAxis2);
        ui->customPlot->yAxis2->setLabelColor(Qt::red);
        ui->customPlot->yAxis2->setLabel(label);
        ui->customPlot->yAxis2->setVisible(true);
    }

    if (digital) {
        graph->setLineStyle(QCPGraph::lsStepLeft);
    } else {
        graph->setLineStyle(QCPGraph::lsLine);
    }

    graph->setData(m_timeBaseus, data);
    graph->rescaleAxes();
    graph->valueAxis()->scaleRange(1.2);

    ui->customPlot->replot();
}

void PlotWindow::setRangeLower(int sample)
{
    if (sample < 0 || sample >= m_timeBaseus.count()) {
        return;
    }
    ui->customPlot->xAxis->setRangeLower(m_timeBaseus.value(sample));
}

void PlotWindow::setRangeUpper(int sample)
{
    if (sample < 0) {
        return;
    } else if (sample >= m_timeBaseus.count()) {
        sample = m_timeBaseus.count() - 1;
    }
    ui->customPlot->xAxis->setRangeUpper(m_timeBaseus.value(sample));
}

bool PlotWindow::savePng(const QString &fileName, int width, int height)
{
    return ui->customPlot->savePng(fileName, width, height);
}

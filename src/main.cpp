#include <QtWidgets/QApplication>

#include "ComtradeRecord.h"
#include "PlotWindow.h"

#include <QtCore/QCommandLineParser>
#include <QtCore/QDebug>
#include <QtCore/QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PlotWindow w;

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("comtrade plot"));
    parser.addHelpOption();

    QCommandLineOption cfgOpt({QStringLiteral("c"), QStringLiteral("cfgfile")},
                              QStringLiteral("Required, the path to the COMTRADE cfg file"), QStringLiteral("file"));
    parser.addOption(cfgOpt);

    QCommandLineOption datOpt({QStringLiteral("d"), QStringLiteral("datfile")},
                              QStringLiteral("Required, the path to the COMTRADE dat file"), QStringLiteral("file"));
    parser.addOption(datOpt);

    QCommandLineOption primaryChanOpt({QStringLiteral("1"), QStringLiteral("channel-1")},
                                      QStringLiteral("Required, the primary channel to be plotted, the format is a or d for analog or digital "
                                          "followed by the channel index. Examples: a2, d12"), QStringLiteral("channel-id"));
    parser.addOption(primaryChanOpt);

    QCommandLineOption secondaryChanOpt({QStringLiteral("2"), QStringLiteral("channel-2")},
                                        QStringLiteral("The secondary channel to be plotted, the format is a or d for analog or digital "
                                            "followed by the channel index. Examples: a2, d12"), QStringLiteral("channel-id"));
    parser.addOption(secondaryChanOpt);

    QCommandLineOption outOpt({QStringLiteral("o"), QStringLiteral("outfile")},
                              QStringLiteral("The path to the output PNG file, if given the program will run non interactively"), QStringLiteral("file"));
    parser.addOption(outOpt);

    QCommandLineOption dimensionsOpt({QStringLiteral("i"), QStringLiteral("dimensions")},
                              QStringLiteral("The dimensions of the output PNG"), QStringLiteral("widthxheight"));
    parser.addOption(dimensionsOpt);

    QCommandLineOption startOpt({QStringLiteral("s"), QStringLiteral("start")},
                                QStringLiteral("The first sample for the x axis. Defaults to 0."), QStringLiteral("n"));
    parser.addOption(startOpt);

    QCommandLineOption endOpt({QStringLiteral("e"), QStringLiteral("end")},
                              QStringLiteral("The last sample for the x axis. Defaults to the number of samples."), QStringLiteral("n"));
    parser.addOption(endOpt);

    parser.process(app);

    if (!parser.isSet(cfgOpt) || !parser.isSet(datOpt) || !parser.isSet(primaryChanOpt)) {
        qWarning() << "Error: -c, -d and -1 options are required";
        parser.showHelp();
    }

    QString cfgPath = parser.value(cfgOpt);
    QString datPath = parser.value(datOpt);

    QFile cfgFile(cfgPath);
    if (!cfgFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open cfg file: " << cfgPath;
        return 1;
    }

    ComtradeRecord *record = ComtradeRecord::fromCfg(&cfgFile);

    QFile datFile(datPath);
    if (!datFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open dat file: " << datPath;
        return 1;
    }

    w.setComtradeRecord(record);
    w.loadSamples(&datFile);

    bool ok;
    QString primaryChan = parser.value(primaryChanOpt);
    int index = primaryChan.mid(1).toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid channel-1: " << primaryChan;
        return 1;
    }

    if (primaryChan.toLower().startsWith(QStringLiteral("a"))) {
        w.addPlot(PlotWindow::AnalogChannel, index, true);
    } else if (primaryChan.toLower().startsWith(QStringLiteral("d"))) {
        w.addPlot(PlotWindow::DigitalChannel, index, true);
    } else {
        qWarning() << "Invalid channel-1: " << primaryChan;
        return 1;
    }

    if (parser.isSet(secondaryChanOpt)) {
        QString secondaryChan = parser.value(secondaryChanOpt);
        index = secondaryChan.mid(1).toInt(&ok);
        if (!ok) {
            qWarning() << "Invalid channel-2: " << primaryChan;
            return 1;
        }

        if (secondaryChan.toLower().startsWith(QStringLiteral("a"))) {
            w.addPlot(PlotWindow::AnalogChannel, index, false);
        } else if (secondaryChan.toLower().startsWith(QStringLiteral("d"))) {
            w.addPlot(PlotWindow::DigitalChannel, index, false);
        } else {
            qWarning() << "Invalid channel-2: " << primaryChan;
            return 1;
        }
    }

    if (parser.isSet(startOpt)) {
        QString startSampleString = parser.value(startOpt);
        int startSample = startSampleString.toInt(&ok);
        if (!ok) {
            qWarning() << "Invalid start sample: " << startSampleString;
            return 1;
        }

        w.setRangeLower(startSample);
    }

    if (parser.isSet(endOpt)) {
        QString endSampleString = parser.value(endOpt);
        int endSample = endSampleString.toInt(&ok);
        if (!ok) {
            qWarning() << "Invalid end sample: " << endSample;
            return 1;
        }

        w.setRangeUpper(endSample);
    }

    if (parser.isSet(outOpt)) {
        QString outPath = parser.value(outOpt);
        QStringList dimensionsTokens = parser.value(dimensionsOpt).toLower().split(QStringLiteral("x"));
        int width = dimensionsTokens.value(0).toInt();
        int height = dimensionsTokens.value(1).toInt();
        if (w.savePng(outPath, width, height)) {
            return 0;
        } else {
            qWarning() << "Can't open out file: " << outPath;
            return 1;
        }
    } else {
        w.show();
    }

    return app.exec();
}

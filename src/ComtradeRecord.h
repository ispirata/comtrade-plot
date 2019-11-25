#ifndef _COMTRADE_RECORD_H_
#define _COMTRADE_RECORD_H_

#include <QtCore/QObject>

#include <QtCore/QIODevice>

class ComtradeAnalogChannel;
class ComtradeDigitalChannel;

class ComtradeRecord : public QObject
{
    Q_OBJECT

public:
    ComtradeRecord(QObject *parent = nullptr);
    ~ComtradeRecord();

    int channelsCount() const;
    int analogChannelsCount() const;
    int digitalChannelsCount() const;
    const ComtradeAnalogChannel *analogChannel(int index) const;
    const ComtradeDigitalChannel *digitalChannel(int index) const;

    int ratesCount() const;
    double sampleRate(int rateIndex) const;
    int lastSample(int rateIndex) const;

    double timeMultiplier() const;
    double triggerOffsetms() const;

    static ComtradeRecord *fromCfg(QIODevice *cfgFile, QObject *parent = nullptr);

private:
    void appendAnalogChannel(ComtradeAnalogChannel *analogChannel);
    void appendDigitalChannel(ComtradeDigitalChannel *digitalChannels);
    void appendSampling(double sampleRate, int lastSample);
    void setTimeMultiplier(double timeMultiplier);
    void setTriggerOffsetms(double triggerOffsetms);

    QList<double> m_sampleRates;
    QList<int> m_lastSamples;
    QList<ComtradeAnalogChannel *> m_analogChannels;
    QList<ComtradeDigitalChannel *> m_digitalChannels;
    double m_timeMultiplier;
    double m_triggerOffsetms;
};

#endif

#include "ComtradeRecord.h"

#include "ComtradeAnalogChannel.h"
#include "ComtradeDigitalChannel.h"

#include <QtCore/QDebug>
#include <QtCore/QDateTime>

ComtradeRecord::ComtradeRecord(QObject *parent)
    : QObject(parent)
    , m_timeMultiplier(0)
    , m_triggerOffsetms(0)
{
}

ComtradeRecord::~ComtradeRecord()
{
}

int ComtradeRecord::channelsCount() const
{
    return m_analogChannels.count() + m_digitalChannels.count();
}

int ComtradeRecord::analogChannelsCount() const
{
    return m_analogChannels.count();
}

int ComtradeRecord::digitalChannelsCount() const
{
    return m_digitalChannels.count();
}

const ComtradeAnalogChannel *ComtradeRecord::analogChannel(int index) const
{
    return m_analogChannels.value(index);
}

const ComtradeDigitalChannel *ComtradeRecord::digitalChannel(int index) const
{
    return m_digitalChannels.value(index);
}

int ComtradeRecord::ratesCount() const
{
    return m_sampleRates.count();
}

double ComtradeRecord::sampleRate(int rateIndex) const
{
    return m_sampleRates.value(rateIndex);
}

int ComtradeRecord::lastSample(int rateIndex) const
{
    return m_lastSamples.value(rateIndex);
}

double ComtradeRecord::timeMultiplier() const
{
    return m_timeMultiplier;
}

double ComtradeRecord::triggerOffsetms() const
{
    return m_triggerOffsetms;
}

void ComtradeRecord::appendAnalogChannel(ComtradeAnalogChannel *analogChannel)
{
    m_analogChannels.append(analogChannel);
}

void ComtradeRecord::appendDigitalChannel(ComtradeDigitalChannel *digitalChannels)
{
    m_digitalChannels.append(digitalChannels);
}

void ComtradeRecord::appendSampling(double sampleRate, int lastSample)
{
    m_sampleRates.append(sampleRate);
    m_lastSamples.append(lastSample);
}

void ComtradeRecord::setTimeMultiplier(double timeMultiplier)
{
    m_timeMultiplier = timeMultiplier;
}

void ComtradeRecord::setTriggerOffsetms(double triggerOffsetms)
{
    m_triggerOffsetms = triggerOffsetms;
}

ComtradeRecord *ComtradeRecord::fromCfg(QIODevice *cfgFile, QObject *parent)
{
    ComtradeRecord *ret = new ComtradeRecord(parent);

    QList<QByteArray> tokens;
    bool ok = true;

    // station_name,rec_dev_id,rev_year, ignore for now
    QByteArray line = cfgFile->readLine();

    // TT,##A,##D
    line = cfgFile->readLine();
    tokens = line.trimmed().split(',');
    if (tokens.count() != 3) {
        qWarning() << "Invalid CFG line: " << line;
        ret->deleteLater();
        return nullptr;
    }
    int totalChannels = tokens.value(0).toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid channels count: " << tokens.value(0);
    }
    QByteArray analogToken = tokens.value(1);
    analogToken.chop(1);
    int analogChannels = analogToken.toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid analog count: " << tokens.value(1);
        ret->deleteLater();
        return nullptr;
    }
    QByteArray digitalToken = tokens.value(2);
    digitalToken.chop(1);
    int digitalChannels = digitalToken.toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid digital count: " << tokens.value(2);
        ret->deleteLater();
        return nullptr;
    }
    if (totalChannels != digitalChannels + analogChannels) {
        qWarning() << "Total count not equal to analog + digital (" << totalChannels << analogChannels << digitalChannels << ")";
        ret->deleteLater();
        return nullptr;
    }

    // Analog channels lines
    for (int i = 0; i < analogChannels; ++i) {
        line = cfgFile->readLine();
        ComtradeAnalogChannel *analogChannel = ComtradeAnalogChannel::fromCfgLine(line, ret);
        if (!analogChannel) {
            ret->deleteLater();
            return nullptr;
        }
        ret->appendAnalogChannel(analogChannel);
    }

    // Digital channels lines
    for (int i = 0; i < digitalChannels; ++i) {
        line = cfgFile->readLine();
        ComtradeDigitalChannel *digitalChannel = ComtradeDigitalChannel::fromCfgLine(line, ret);
        if (!digitalChannel) {
            ret->deleteLater();
            return nullptr;
        }
        ret->appendDigitalChannel(digitalChannel);
    }

    // lf, ignore for now
    line = cfgFile->readLine();

    // nrates
    line = cfgFile->readLine();
    int nRates = line.trimmed().toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid nrates: " << line;
        ret->deleteLater();
        return nullptr;
    }

    // Rates lines
    for (int i = 0; i < nRates; ++i) {
        line = cfgFile->readLine();
        tokens = line.trimmed().split(',');
        if (tokens.count() != 2) {
            qWarning() << "Invalid rate line: " << line;
            ret->deleteLater();
            return nullptr;
        }
        double sampleRate = tokens.value(0).toDouble(&ok);
        if (!ok) {
            qWarning() << "Invalid sample rate: " << tokens.value(0);
            ret->deleteLater();
            return nullptr;
        }
        int lastSample = tokens.value(1).toInt(&ok);
        if (!ok) {
            qWarning() << "Invalid last sample: " << tokens.value(1);
            ret->deleteLater();
            return nullptr;
        }
        ret->appendSampling(sampleRate, lastSample);
    }

    // initialTimestamp
    line = cfgFile->readLine();
    QDateTime initialTimestamp = QDateTime::fromString(QString::fromLatin1(line.trimmed()), QStringLiteral("dd/MM/yyyy,HH:mm:ss.zzz000"));
    // triggerTimestamp
    line = cfgFile->readLine();
    QDateTime triggerTimestamp = QDateTime::fromString(QString::fromLatin1(line.trimmed()), QStringLiteral("dd/MM/yyyy,HH:mm:ss.zzz000"));
    if (!initialTimestamp.isValid() || !triggerTimestamp.isValid()) {
        qWarning() << "Invalid timestamps, timebase will have no offset";
    } else {
        qint64 offset = initialTimestamp.msecsTo(triggerTimestamp);
        ret->setTriggerOffsetms(offset);
    }
    // ft
    line = cfgFile->readLine();
    if (line.trimmed() != "ASCII") {
        qWarning() << "Unsupported file type: " << line;
        ret->deleteLater();
        return nullptr;
    }

    // timemultiplier
    line = cfgFile->readLine();
    double timeMultiplier = line.trimmed().toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid time multiplier: " << line;
        ret->deleteLater();
        return nullptr;
    }
    ret->setTimeMultiplier(timeMultiplier);

    return ret;
}

#include "ComtradeAnalogChannel.h"

#include <QtCore/QDebug>

#define ANALOG_LINE_TOKENS 13

ComtradeAnalogChannel::ComtradeAnalogChannel(QObject *parent)
    : ComtradeChannel(parent)
    , m_valueScaling(ScalingIdentifier::Invalid)
    , m_multiplier(0)
    , m_offset(0)
    , m_channelSkewus(0)
    , m_primaryRatio(0)
    , m_secondaryRatio(0)
    , m_minValue(0)
    , m_maxValue(0)
{
}

ComtradeAnalogChannel::~ComtradeAnalogChannel()
{
}

QString ComtradeAnalogChannel::unit() const
{
    return m_unit;
}

double ComtradeAnalogChannel::multiplier() const
{
    return m_multiplier;
}

double ComtradeAnalogChannel::offset() const
{
    return m_offset;
}

double ComtradeAnalogChannel::channelSkewus() const
{
    return m_channelSkewus;
}

int ComtradeAnalogChannel::minValue() const
{
    return m_minValue;
}

int ComtradeAnalogChannel::maxValue() const
{
    return m_maxValue;
}

double ComtradeAnalogChannel::primaryRatio() const
{
    return m_primaryRatio;
}

double ComtradeAnalogChannel::secondaryRatio() const
{
    return m_secondaryRatio;
}

ComtradeAnalogChannel::ScalingIdentifier ComtradeAnalogChannel::valueScaling() const
{
    return m_valueScaling;
}

void ComtradeAnalogChannel::setUnit(const QString &unit)
{
    m_unit = unit;
}

void ComtradeAnalogChannel::setMultiplier(double multiplier)
{
    m_multiplier = multiplier;
}

void ComtradeAnalogChannel::setOffset(double offset)
{
    m_offset = offset;
}

void ComtradeAnalogChannel::setChannelSkewus(double channelSkewus)
{
    m_channelSkewus = channelSkewus;
}

void ComtradeAnalogChannel::setMinValue(int minValue)
{
    m_minValue = minValue;
}

void ComtradeAnalogChannel::setMaxValue(int maxValue)
{
    m_maxValue = maxValue;
}

void ComtradeAnalogChannel::setPrimaryRatio(double primaryRatio)
{
    m_primaryRatio = primaryRatio;
}

void ComtradeAnalogChannel::setSecondaryRatio(double secondaryRatio)
{
    m_secondaryRatio = secondaryRatio;
}

void ComtradeAnalogChannel::setValueScaling(ScalingIdentifier scaling)
{
    m_valueScaling = scaling;
}

ComtradeAnalogChannel *ComtradeAnalogChannel::fromCfgLine(const QByteArray &line, QObject *parent)
{
    QList<QByteArray> tokens = line.trimmed().split(',');
    if (tokens.count() != ANALOG_LINE_TOKENS) {
        qWarning() << "Invalid analog channel cfg: " << line;
        return nullptr;
    }

    bool ok;

    int index = tokens.value(0).toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid channel index: " << tokens.value(0);
        return nullptr;
    }

    // Non-critical fields
    QString id = QString::fromLatin1(tokens.value(1));
    QString phase = QString::fromLatin1(tokens.value(2));
    QString ccbm = QString::fromLatin1(tokens.value(3));

    QString unit = QString::fromLatin1(tokens.value(4));
    double multiplier = tokens.value(5).toDouble(&ok);
    if (!ok) {
        qWarning() << "Invalid multiplier: " << tokens.value(5);
        return nullptr;
    }
    double offset = tokens.value(6).toDouble(&ok);
    if (!ok) {
        qWarning() << "Invalid offset: " << tokens.value(6);
        return nullptr;
    }

    // Non critical
    double channelSkewus = tokens.value(7).toDouble(&ok);

    int minValue = tokens.value(8).toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid minValue: " << tokens.value(8);
        return nullptr;
    }

    int maxValue = tokens.value(9).toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid maxValue: " << tokens.value(9);
        return nullptr;
    }

    double primaryRatio = tokens.value(10).toDouble(&ok);
    if (!ok) {
        qWarning() << "Invalid primaryRatio: " << tokens.value(10);
        return nullptr;
    }
    double secondaryRatio = tokens.value(11).toDouble(&ok);
    if (!ok) {
        qWarning() << "Invalid secondaryRatio: " << tokens.value(11);
        return nullptr;
    }

    ScalingIdentifier scaling;
    QByteArray scalingToken = tokens.value(12).toLower();
    if (scalingToken == "p") {
        scaling = ScalingIdentifier::Primary;
    } else if (scalingToken == "s") {
        scaling = ScalingIdentifier::Secondary;
    } else {
        qWarning() << "Invalid scaling identifier: " << tokens.value(12);
        return nullptr;
    }

    ComtradeAnalogChannel *ret = new ComtradeAnalogChannel(parent);
    ret->setIndex(index);
    ret->setId(id);
    ret->setPhase(phase);
    ret->setCCBM(ccbm);
    ret->setUnit(unit);
    ret->setMultiplier(multiplier);
    ret->setOffset(offset);
    ret->setChannelSkewus(channelSkewus);
    ret->setMinValue(minValue);
    ret->setMaxValue(maxValue);
    ret->setPrimaryRatio(primaryRatio);
    ret->setSecondaryRatio(secondaryRatio);
    ret->setValueScaling(scaling);

    return ret;
}

#ifndef _COMTRADE_ANALOG_CHANNEL_H
#define _COMTRADE_ANALOG_CHANNEL_H

#include "ComtradeChannel.h"

class ComtradeAnalogChannel : public ComtradeChannel
{
    Q_OBJECT

public:
    enum class ScalingIdentifier {
        Invalid = 0,
        Primary = 1,
        Secondary = 2
    };
    Q_ENUM(ScalingIdentifier)

    ComtradeAnalogChannel(QObject *parent = nullptr);
    ~ComtradeAnalogChannel();

    QString unit() const;
    double multiplier() const;
    double offset() const;
    double channelSkewus() const;
    int minValue() const;
    int maxValue() const;
    double primaryRatio() const;
    double secondaryRatio() const;
    ScalingIdentifier valueScaling() const;

    void setUnit(const QString &unit);
    void setMultiplier(double multiplier);
    void setOffset(double offset);
    void setChannelSkewus(double channelSkewus);
    void setMinValue(int minValue);
    void setMaxValue(int maxValue);
    void setPrimaryRatio(double primaryRatio);
    void setSecondaryRatio(double secondaryRatio);
    void setValueScaling(ScalingIdentifier scaling);

    static ComtradeAnalogChannel *fromCfgLine(const QByteArray &line, QObject *parent = nullptr);

private:
    QString m_unit;
    ScalingIdentifier m_valueScaling;
    double m_multiplier;
    double m_offset;
    double m_channelSkewus;
    double m_primaryRatio;
    double m_secondaryRatio;
    int m_minValue;
    int m_maxValue;
};

#endif

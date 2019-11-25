#ifndef _COMTRADE_DIGITAL_CHANNEL_H
#define _COMTRADE_DIGITAL_CHANNEL_H

#include "ComtradeChannel.h"

class ComtradeDigitalChannel : public ComtradeChannel
{
    Q_OBJECT

public:
    ComtradeDigitalChannel(QObject *parent = nullptr);
    ~ComtradeDigitalChannel();

    bool normalState() const;

    void setNormalState(bool normalState);

    static ComtradeDigitalChannel *fromCfgLine(const QByteArray &line, QObject *parent = nullptr);

private:
    bool m_normalState;
};

#endif

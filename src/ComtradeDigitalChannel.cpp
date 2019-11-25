#include "ComtradeDigitalChannel.h"

#include <QtCore/QDebug>

#define DIGITAL_LINE_TOKENS 5

ComtradeDigitalChannel::ComtradeDigitalChannel(QObject* parent)
    : ComtradeChannel(parent)
    , m_normalState(false)
{
}

ComtradeDigitalChannel::~ComtradeDigitalChannel()
{
}

bool ComtradeDigitalChannel::normalState() const
{
    return m_normalState;
}

void ComtradeDigitalChannel::setNormalState(bool normalState)
{
    m_normalState = normalState;
}

ComtradeDigitalChannel *ComtradeDigitalChannel::fromCfgLine(const QByteArray &line, QObject *parent)
{
    QList<QByteArray> tokens = line.trimmed().split(',');
    if (tokens.count() != DIGITAL_LINE_TOKENS) {
        qWarning() << "Invalid digital channel cfg: " << line;
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

    bool normalState = tokens.value(4).toInt(&ok);
    if (!ok) {
        qWarning() << "Invalid normal state: " << tokens.value(4);
        return nullptr;
    }

    ComtradeDigitalChannel *ret = new ComtradeDigitalChannel(parent);
    ret->setIndex(index);
    ret->setId(id);
    ret->setPhase(phase);
    ret->setCCBM(ccbm);
    ret->setNormalState(normalState);

    return ret;
}

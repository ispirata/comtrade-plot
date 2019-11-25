#ifndef _COMTRADE_CHANNEL_H
#define _COMTRADE_CHANNEL_H

#include <QtCore/QObject>

class ComtradeChannel : public QObject
{
    Q_OBJECT

public:
    ComtradeChannel(QObject *parent = nullptr);
    ~ComtradeChannel();

    int index() const;
    QString id() const;
    QString phase() const;
    QString ccbm() const;

    void setIndex(int index);
    void setId(const QString &id);
    void setPhase(const QString &phase);
    void setCCBM(const QString &ccbm);

protected:
    int m_index;
    QString m_id;
    QString m_phase;
    QString m_ccbm;
};

#endif

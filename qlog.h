#ifndef QLOG_H
#define QLOG_H

#include <QObject>
#include <QMetaObject>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include "misc.h"

enum Level{
    DBG,
    INFO,
    WARN,
    ERR,
    CRI
};

enum IO{
    QDBG,
    LDOC
};

#define LOG_FIELD_SEP_CHAR ","

class QLog: public QObject
{
Q_OBJECT

public:
    explicit QLog(QObject* parent, const QString name="");
    ~QLog();

    void setIO(IO io);
    void setLevel(Level level);
    void setScale(int size, int rot_len);
    QMetaMethod getJOSlot();
    QList<QMetaMethod> getJOSignals();
    bool connectParentJOSignals();
    // dynamically call the slot or Q_INVOKABLE method
    template<typename RetT, typename... ArgTs>
    inline
    bool invokeParentRetSlot(const char* funcName, RetT& ret, ArgTs... args){
        log(QString("invoke %1 BEFO").arg(funcName), args...);
        bool r = invokeRetSlot(parent(), funcName, ret, args...);
        log(QString("invoke %1 POST").arg(funcName), r, ret);
        return r;
    }

    template<typename... ArgTs>
    inline
    bool invokeParentSlot(const char* funcName, ArgTs... args){
        log(QString("invoke %1 BEFO").arg(funcName), args...);
        bool r = invokeRetSlot(parent(), funcName, args...);
        log(QString("invoke %1 POST").arg(funcName), r);
        return r;
    }

    // dynamically send the signal
    template<typename RetT, typename... ArgTs>
    inline
    bool invokeParentRetSignal(const char* signalName, RetT& ret, ArgTs... args){
        log(QString("emit %1").arg(signalName), args...);
        return invokeRetSignal(parent(), signalName, ret, args...);
    }

    template<typename... ArgTs>
    inline
    bool invokeParentSignal(const char* signalName, ArgTs... args){
        log(QString("emit %1").arg(signalName), args...);
        return invokeSignal(parent(), signalName, args...);
    }

    template<typename... RTs>
    void line(Level level, RTs... args){
        if (level >= m_level){
            log(args...);
        }
    }

    template<typename... RTs>
    void dLine(RTs... args){
        line(Level::DBG, args...);
    }

    template<typename... RTs>
    void iLine(RTs... args){
        line(Level::INFO, args...);
    }

    template<typename... RTs>
    void wLine(RTs... args){
        line(Level::WARN, args...);
    }

    template<typename... RTs>
    void eLine(RTs... args){
        line(Level::ERR, args...);
    }

    template<typename... RTs>
    void cLine(RTs... args){
        line(Level::CRI, args...);
    }

private slots:
    // For joSlot method, QJsonObject {"topic":,"body":} is the suggested data form!
    // And topic can be cascaded like T1/T2/...
    void joSlot(const QJsonObject data);

private:

    bool shift();

    bool appendLine(const QString line);

    template<typename... RTs>
    bool log(QString arg){
        QString curtimeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        if (m_io==IO::QDBG){
            qDebug().noquote().nospace() << curtimeStr << "|" << arg;
            return true;
        }else if (m_io==IO::LDOC){
            shift();
            return appendLine(curtimeStr + "|" + arg);
        }
        return false;
    }

    template<typename... RTs>
    bool log(RTs... args){
        if ((sizeof...(args)) <= 0){
            return log("\n");
        }
        QString s;
        combineStr(s, ',', args...);
        return log(s);
    }

    QString getParentClassName();
    bool setFile(const QString filename);

    QFile m_file;
    int m_size;
    Level m_level;
    IO m_io;
    int m_rot_len;
};

#endif // QLOG_H

#ifndef MISC_H
#define MISC_H

#include <QString>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QSettings>
#include <QMetaProperty>
#include <QFile>


QString dump_jsonobject(const QJsonObject& jo);
QString dump_jsonarray(const QJsonArray& jo);
QJsonObject load_jsonobject(const QString jstr);
QJsonArray load_jsonarray(const QString jstr);
void clear_jsonobject(QJsonObject& jo);
void clear_jsonarray(QJsonArray& ja);
bool read_jsonobject(QJsonValue& val, const QJsonObject& jo, const QString key, const QChar sep='/');
bool read_jsonarray(QJsonValue& val, const QJsonArray& ja, const QString key, const QChar sep='/');
bool upsert_jsonobject(QJsonObject& jo, const QString key, const QJsonValue val, const QChar sep='/');
bool upsert_jsonarray(QJsonArray& ja, const QString key, const QJsonValue val, const QChar sep='/');
bool check_jsonobject(QJsonValue::Type& typ, const QJsonObject jo, QString key, const QChar sep='/');
void parse_inifile(QJsonObject & jo, const QString filename);
void serialize_qobject(const QObject *obj, QJsonObject &jo, bool nested=false);

bool create_textfile(const QString filename);
bool delete_textfile(const QString filename);
bool read_textfile(const QString filename, QString& data);
bool write_textfile(const QString filename, const QString data);
bool append_textfile(const QString filename, const QString data);

bool file_exists(const QString filename);
void write_jsonfile(const QString filename, const QString json_str);
QString read_jsonfile(const QString filename);
void create_jsonfile(const QString filename);

inline QString& combineStr(QString& tot)
{
    return tot;
}

inline QString& combineStr(QString& tot, const QChar sep, const QString& arg){
    if (!tot.isEmpty()){
        tot.append(sep);
    }
    tot.append(arg);
    return combineStr(tot);
}

inline QString& combineStr(QString& tot, const QChar sep, const char* arg){
    if (!tot.isEmpty()){
        tot.append(sep);
    }
    tot.append(arg);
    return combineStr(tot);
}


template<typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, const QString& arg, RTs... r_args){
    if (!tot.isEmpty()){
        tot.append(sep);
    }
    tot.append(arg);
    if ((sizeof...(r_args)) <= 0){
        return tot;
    }
    return combineStr(tot, sep, r_args...);
}

template<typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, const char* arg, RTs... r_args){
    if (!tot.isEmpty()){
        tot.append(sep);
    }
    tot.append(arg);
    if ((sizeof...(r_args)) <= 0){
        return tot;
    }
    return combineStr(tot, sep, r_args...);
}

template<typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, const QByteArray& arg, RTs... r_args){
    return combineStr(tot, sep, arg.data(), r_args...);
}

template<typename... RTs>
inline
    QString& combineStr(QString& tot, const QChar sep, bool arg, RTs... r_args){
    if (arg){
        return combineStr(tot, sep, "true", r_args...);
    }else{
        return combineStr(tot, sep, "false", r_args...);
    }
}

template<typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, int arg, RTs... r_args){
    return combineStr(tot, sep, QString::number(arg), r_args...);
}

template<typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, double arg, RTs... r_args){
    return combineStr(tot, sep, QString::number(arg), r_args...);
}

template<typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, const QJsonObject& arg, RTs... r_args){
    return combineStr(tot, sep, dump_jsonobject(arg), r_args...);
}

template<typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, const QJsonArray& arg, RTs... r_args){
    return combineStr(tot, sep, dump_jsonarray(arg), r_args...);
}

// handle QObject*, but covered by the next default handler!
template<typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, QObject* arg, RTs... r_args){
    QJsonObject obj;
    serialize_qobject(arg, obj);
    return combineStr(tot, sep, obj, r_args...);
}

// handle else unclear class
template<typename T, typename... RTs>
inline QString& combineStr(QString& tot, const QChar sep, T arg, RTs... r_args){
    QString simple_display =  QString("<%1>").arg(typeid(arg).name());
    return combineStr(tot, sep, simple_display, r_args...);
}

///////////////////////////////////////////////////////////////////
template<typename T>
QReturnArgument<T> getRetArg(T& x)
{
    return QReturnArgument<T>(typeid(x).name(), x);
}

template<typename T>
QArgument<T> getArg(T& x)
{
    return QArgument<T>(typeid(x).name(), x);
}

// dynamically call the slot or Q_INVOKABLE method
template<typename RetT, typename... ArgTs>
inline
bool invokeRetSlot(QObject *obj, const char* funcName, RetT& ret, ArgTs... args){
    return QMetaObject::invokeMethod(
        obj,
        funcName,
        Qt::AutoConnection,
        getRetArg<RetT>(ret),
        getArg<ArgTs>(args)...
        );
}

template<typename... ArgTs>
inline
bool invokeSlot(QObject *obj, const char* funcName, ArgTs... args){
    return QMetaObject::invokeMethod(
        obj,
        funcName,
        Qt::AutoConnection,
        getArg<ArgTs>(args)...
        );
}

// dynamically send the signal
template<typename RetT, typename... ArgTs>
inline
bool invokeRetSignal(QObject *obj, const char* signalName, RetT& ret, ArgTs... args){
    return QMetaObject::invokeMethod(
        obj,
        signalName,
        Qt::AutoConnection,
        getRetArg<RetT>(ret),
        getArg<ArgTs>(args)...
        );
}

template<typename... ArgTs>
inline
bool invokeSignal(QObject *obj, const char* signalName, ArgTs... args){
    return QMetaObject::invokeMethod(
        obj,
        signalName,
        Qt::AutoConnection,
        getArg<ArgTs>(args)...
        );
}

#endif // MISC_H

#include "qlog.h"
#include "qapplication.h"
#include <QMetaMethod>

QLog::QLog(QObject* parent, const QString name)
{
    setParent(parent);
    setObjectName(name);

    m_level = Level::DBG;
    m_io = IO::QDBG;
    m_size = 512;
    m_rot_len = 7;

    QString filename;
    if(!objectName().isNull()){
        filename = objectName() + ".log";
    }else if(parent){
        filename = getParentClassName() + ".log";
    }
    else{
        filename = QCoreApplication::applicationName() + ".log";
    }
    setFile(filename);

    connectParentJOSignals();
}

QLog::~QLog()
{
}

QString QLog::getParentClassName()
{
    if (parent()){
        return parent()->metaObject()->className();
    }
    return "";
}

void QLog::setIO(IO io){
    m_io = io;
}

void QLog::setLevel(Level level){
    m_level = level;
}

void QLog::setScale(int size, int rot_len){
    m_size = size;
    m_rot_len = rot_len;
}

bool QLog::shift(){
    if ((m_file.size() / 1024) >= m_size){
        // remove old backup
        if(m_rot_len > 0){
            QString originfile, backupfile;
            backupfile = QString("%1.%2").arg(m_file.fileName()).arg(m_rot_len);
            if (QFile::exists(backupfile))
                QFile::remove(backupfile);
            for (int i=m_rot_len-1; i>=0; --i){
                if(i==0){
                    originfile = m_file.fileName();
                }else{
                    originfile = QString("%1.%2").arg(m_file.fileName()).arg(i);
                }
                if (QFile::exists(originfile)){
                    backupfile = QString("%1.%2").arg(m_file.fileName()).arg(i+1);
                    QFile::rename(originfile, backupfile);
                }
            }
            return true;
        }
    }
    return false;
}

bool QLog::appendLine(const QString line){
    if (m_file.open(QFile::Append | QFile::Text | QFile::Unbuffered)){
        QTextStream out(&m_file);
        out << line << "\n";
        m_file.close();
        return true;
    }
    return false;
}


bool QLog::setFile(QString filename){
    m_file.setFileName(filename);
    if(!m_file.exists()){
        try{
            // create empty log file
            create_textfile(filename);
        }catch(...){
            return false;
        }
    }
    return true;
}


void QLog::joSlot(const QJsonObject data)
{
    iLine("joSignal", data);
}

QMetaMethod QLog::getJOSlot(){
    const QMetaObject* metaObject = this->metaObject();
    // Iterate custemed methods in range [methodOffset(), methodCount())
    for (int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
        QMetaMethod method = metaObject->method(i);
        if (method.methodType() == QMetaMethod::Slot){
            if (QString(method.name()).compare("joSlot") == 0){
                // return metaObject->method(metaObject->indexOfMethod(method.methodSignature()));
                if ((method.parameterCount() == 1) && (method.parameterType(0) == 46))
                {
                    return method;
                }
            }
        }
    }
    return QMetaMethod();
}

QList<QMetaMethod> QLog::getJOSignals(){
    QObject *parent = this->parent();
    const QMetaObject* metaObject = parent->metaObject();
    QList<QMetaMethod> mmds;
    for (int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
        QMetaMethod method = metaObject->method(i);
        if (method.methodType() == QMetaMethod::Signal) {
            // jo signal must has not less than 1 parameter which type is QJsonObject (46)
            if ((method.parameterCount() >= 1) && (method.parameterType(0) == 46))
            {
                mmds.append(method);
            }
        }
    }
    return mmds;
}

bool QLog::connectParentJOSignals()
{
    // parameterType map is next
    // void: 0 =No
    // const char*:0
    // QByteArray:12
    // QString:10
    // bool:1
    // int:2
    // char:34
    // float:38
    // QJsonObject:46
    // QJsonArray:47
    // ...

    QObject *parent = this->parent();

    if (parent == nullptr){
        return false;
    }

    QMetaMethod jo_slot = this->getJOSlot();
    if (!jo_slot.isValid()){
        return false;
    }

    QList<QMetaMethod> parent_jo_signals = this->getJOSignals();
    for (const QMetaMethod& mmd: parent_jo_signals){
        QObject::connect(
            parent,
            mmd,
            (QObject*)this,
            jo_slot
            );
    }

    return true;
}

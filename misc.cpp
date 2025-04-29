#include "misc.h"


void seperate_key(const QString key, const QChar sep, QString& top_key, QString& rem_key)
{
    QStringList sub_keys = key.split(sep);
    top_key = sub_keys.at(0);
    rem_key = key.right(key.length()-top_key.length()-1);
}

bool isInteger(const QString str)
{
    bool ok;
    str.toInt(&ok);
    return ok;
}

bool isReal(const QString str)
{
    bool ok;
    str.toDouble(&ok);
    return ok;
}

bool isBoolean(const QString str)
{
    return (str == "true"|| str == "false");
}

bool read_jsonobject(QJsonValue& val, const QJsonObject& jo, const QString key, const QChar sep)
{
    if (key.contains(sep)){
        QString top_key, rem_key;
        seperate_key(key, sep, top_key,rem_key);
        if(jo.value(top_key).isObject()){
            QJsonObject jo_top = jo.value(top_key).toObject();
            return read_jsonobject(val, jo_top, rem_key, sep);
        }
        if(jo.value(top_key).isArray()){
            QJsonArray jo_top = jo.value(top_key).toArray();
            return read_jsonarray(val, jo_top, rem_key, sep);
        }
        return false;
    }

    if (jo.contains(key)){
        val = jo.value(key);
        return true;
    }
    return false;
}

bool read_jsonarray(QJsonValue& val, const QJsonArray& ja, const QString key, const QChar sep)
{
    if (key.contains(sep)){
        QString top_key, rem_key;
        seperate_key(key, sep, top_key,rem_key);
        if (isInteger(top_key)){
            int idx = top_key.toInt();
            if (ja.at(idx).isArray()) {
                QJsonArray ja_top = ja.at(idx).toArray();
                return read_jsonarray(val, ja_top, rem_key, sep);
            }
            if (ja.at(idx).isObject()) {
                QJsonObject ja_top = ja.at(idx).toObject();
                return read_jsonobject(val, ja_top, rem_key, sep);
            }
        }else{
            return false;
        }
    }

    if (isInteger(key)){
        if (ja.size() > key.toInt()){
            val = ja.at(key.toInt());
            return true;
        }
    }
    return false;
}

bool check_jsonobject(QJsonValue::Type& typ, const QJsonObject jo, const QString key, const QChar sep)
{
    QJsonValue val;
    if (read_jsonobject(val, jo, key, sep)){
        typ = val.type();
        return true;
    }
    return false;
}

void parse_inifile(QJsonObject & jo, const QString filename)
{
    QSettings settings(filename, QSettings::IniFormat);

    QStringList keys = settings.allKeys();
    for (const QString& key_expr: keys){
        if (key_expr.contains("/")){
            QVariant val = settings.value(key_expr);
            QStringList key_parts = key_expr.split("/");
            if (key_parts.length() == 2){
                QString group = key_parts[0];
                QString key = key_parts[1];
                QJsonObject group_obj;
                if (jo.contains(group)){
                    group_obj = jo[group].toObject();
                }

                if (isBoolean(val.toJsonValue().toString())){
                    group_obj[key] = val.toBool();
                }else if (isInteger(val.toString())){
                    group_obj[key] = val.toInt();
                }else if (isReal(val.toString())){
                    group_obj[key] = val.toDouble();
                }else{
                    group_obj[key] = val.toString();
                }
                jo[group] = group_obj;

            }else if (key_parts.length() == 3){
                QString group = key_parts[0];
                QString subgroup = key_parts[1];
                QString subkey = key_parts[2];
                QJsonObject group_obj;
                QJsonObject subgroup_obj;
                if (jo.contains(group)){
                    group_obj = jo[group].toObject();
                    if (group_obj.contains(subgroup)){
                        subgroup_obj = group_obj[subgroup].toObject();
                    }
                }
                if (isBoolean(val.toJsonValue().toString())){
                    subgroup_obj[subkey] = val.toBool();
                }else if (isInteger(val.toString())){
                    subgroup_obj[subkey] = val.toInt();
                }else if (isReal(val.toString())){
                    subgroup_obj[subkey] = val.toDouble();
                }else{
                    subgroup_obj[subkey] = val.toString();
                }
                group_obj[subgroup] = subgroup_obj;

                jo[group] = group_obj;
            }
        }
    }
}

QString dump_jsonobject(const QJsonObject& jo)
{
    QJsonDocument jdoc;
    jdoc.setObject(jo);
    return QString::fromUtf8(jdoc.toJson(QJsonDocument::Compact));
}

QString dump_jsonarray(const QJsonArray& ja)
{
    QJsonDocument jdoc;
    jdoc.setArray(ja);
    return QString::fromUtf8(jdoc.toJson(QJsonDocument::Compact));
}

QJsonObject load_jsonobject(const QString jstr)
{
    QJsonParseError json_error;
    QJsonDocument jdoc = QJsonDocument::fromJson(jstr.toUtf8(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(jdoc.isObject())
        {
            return jdoc.object();
        }
    }
    return QJsonObject();
}

QJsonArray load_jsonarray(const QString jstr)
{
    QJsonParseError json_error;
    QJsonDocument jdoc = QJsonDocument::fromJson(jstr.toUtf8(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(jdoc.isArray())
        {
            return jdoc.array();
        }
    }
    return QJsonArray();
}

void clear_jsonobject(QJsonObject& jo)
{
    QJsonObject::iterator i = jo.begin();
    while (i != jo.end()) {
        i = jo.erase(i);
    }
}

void clear_jsonarray(QJsonArray& ja)
{
    QJsonArray::iterator i = ja.begin();
    while (i != ja.end()) {
        i = ja.erase(i);
    }
}


bool upsert_jsonobject(QJsonObject& jo, const QString key, const QJsonValue val, const QChar sep)
{
    if (key.contains(sep)){
        QString top_key, rem_key;
        seperate_key(key, sep, top_key,rem_key);
        if (isInteger(top_key)){
            return false;
        }
        if (jo.value(top_key).isObject()){
            QJsonObject jo_top;
            jo_top = jo.value(top_key).toObject();
            bool rt = upsert_jsonobject(jo_top, rem_key, val, sep);
            jo[top_key] = jo_top;
            return rt;
        }
        else if (jo.value(top_key).isArray()){
            QJsonArray jo_top = jo.value(top_key).toArray();
            bool rt = upsert_jsonarray(jo_top, rem_key, val, sep);
            jo[top_key] = jo_top;
            return rt;
        }else {
            return false;
        }
    }
    if (val.isUndefined()){
        jo.remove(key);
    }else{
        jo[key] = val;
    }
    return true;
}

bool upsert_jsonarray(QJsonArray& ja, const QString key, const QJsonValue val, const QChar sep)
{
    if (key.contains(sep)){
        QString top_key, rem_key;
        seperate_key(key, sep, top_key,rem_key);
        if (isInteger(top_key)){
            if (ja.size() > top_key.toInt() && top_key.toInt() >= 0){
                if (ja.at(top_key.toInt()).isObject()) {
                    QJsonObject ja_top = ja.at(top_key.toInt()).toObject();
                    bool rt = upsert_jsonobject(ja_top, rem_key, val, sep);
                    ja.replace(top_key.toInt(), ja_top);
                    return rt;
                }
                if (ja.at(top_key.toInt()).isArray()) {
                    QJsonArray ja_top = ja.at(top_key.toInt()).toArray();
                    bool rt = upsert_jsonarray(ja_top, rem_key, val, sep);
                    ja.replace(top_key.toInt(), ja_top);
                    return rt;
                }
            }
        }
        return false;
    }
    if (isInteger(key)){
        if (val.isUndefined()){
            ja.removeAt(key.toInt());
            return true;
        }else{
            if(ja.size() > key.toInt() && key.toInt() >= 0){
                ja.replace(key.toInt(), val);
                return true;
            }
            if(ja.size() == key.toInt()){
                ja.append(val);
                return true;
            }
        }
    }
    return false;
}

void serialize_qobject(const QObject *obj, QJsonObject &jo, bool nested)
{

    const QMetaObject* pMetaObj = obj->metaObject();
    QString objName = obj->objectName();
    jo["_name_"] = objName;

    for (int i = 0; i < pMetaObj->propertyCount(); ++i) {
        QMetaProperty prop = pMetaObj->property(i);

        // jo[prop.name()] = prop.read(obj).toJsonValue();
        //  output like:
        //      QJsonValue(undefined)
        //      QJsonValue(double, 6)
        //      QJsonValue(double, 1.67772e+07)
        //      QJsonValue(string, "")
        //      QJsonValue(bool, true)
        //      QJsonValue(undefined)

        // jo[prop.name()] = prop.read(obj).toByteArray().data();
        // output like:
        //      "6"
        //      "16777215"
        //      ""
        //      "true"
        //      ""
        //      ""

        jo[prop.name()] = QJsonValue::fromVariant(prop.read(obj));
        //  output like:
        //      QJsonValue(null)
        //      QJsonValue(double, 6)
        //      QJsonValue(double, 1.67772e+07)
        //      QJsonValue(string, "")
        //      QJsonValue(bool, true)
        //      QJsonValue(string, "SimSun,13.5,-1,5,50,0,0,0,0,0")
    }

    if (nested&&(obj->children().count() > 0)){
        QJsonArray child_objs = QJsonArray();
        for (int i = 0; i < obj->children().count(); ++i) {
            auto *child = obj->children().at(i);
            QJsonObject child_obj;
            serialize_qobject(child, child_obj);
            child_objs.append(child_obj);
        }
        jo["_children_"] = child_objs;
    }

}

bool check_textfile(const QString filename){
    QFile f(filename);
    return f.exists();
}

bool create_textfile(const QString filename){
    QFile f(filename);
    if(!f.exists()){
        f.open(QFile::WriteOnly | QFile::Text);
        f.close();
        return true;
    }
    return false;
}

bool delete_textfile(const QString filename){
    QFile f(filename);
    if(!f.exists()){
        return false;
    }
    return f.remove();
}

bool read_textfile(const QString filename, QString& data) {
    QFile f(filename);
    if (!f.exists()) {
        return false;
    }
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }

    QTextStream stream(&f);
    data = stream.readAll();
    f.close();

    return true;
}

bool write_textfile(const QString filename, const QString data){
    QFile f(filename);
    if (!f.open(QFile::WriteOnly | QFile::Text)){
        return false;
    }
    QTextStream out(&f);
    out << data;
    f.close();
    return true;
}

bool append_textfile(const QString filename, const QString data){
    QFile f(filename);
    if (!f.open(QFile::Append | QFile::Text)){
        return false;
    }
    QTextStream out(&f);
    out << data;
    f.close();
    return true;
}

bool file_exists(const QString filename)
{
    QFile file(filename);
    return file.exists();
}

QString read_jsonfile(const QString filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&file);
    file.close();
    return ts.readAll();
}

void write_jsonfile(const QString filename, const QString json_str)
{
    QFile file(filename);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&file);
    out << json_str;
    file.close();
}

void create_jsonfile(const QString filename)
{
    write_jsonfile(filename, "{}");
}

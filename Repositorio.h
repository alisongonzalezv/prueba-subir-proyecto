#ifndef REPOSITORIOJSON_H
#define REPOSITORIOJSON_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

template <typename T>
class RepositorioJson {
protected:
    QString rutaArchivo;

public:
    explicit RepositorioJson(const QString& ruta) : rutaArchivo(ruta) {}
    virtual ~RepositorioJson() = default;

    virtual bool cargar() = 0;
    virtual bool guardar() = 0;
};

#endif // REPOSITORIOJSON_H
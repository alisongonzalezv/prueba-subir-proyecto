#ifndef REPOSITORIOPRODUCTOS_H
#define REPOSITORIOPRODUCTOS_H

#include "RepositorioJson.h"
#include "Producto.h"
#include <vector>
#include <memory>

class RepositorioProductos : public RepositorioJson<Producto> {
private:
    std::vector<Producto> listaProductos;

public:
    explicit RepositorioProductos(const QString& ruta = "productos.json")
        : RepositorioJson(ruta) {}

    bool cargar() override {
        QFile archivo(rutaArchivo);
        if (!archivo.open(QIODevice::ReadOnly)) return false;

        QByteArray data = archivo.readAll();
        archivo.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray()) return false;

        QJsonArray array = doc.array();
        listaProductos.clear();

        for (const auto& valor : array) {
            QJsonObject obj = valor.toObject();
            Producto p(
                obj["id"].toString(),
                obj["codigo"].toString(),
                obj["nombre"].toString(),
                obj["precio"].toDouble(),
                obj["existencias"].toInt(),
                obj["stockMinimo"].toInt(),
                obj["activo"].toBool()
            );
            listaProductos.push_back(p);
        }
        return true;
    }

    bool guardar() override {
        QJsonArray array;
        for (const auto& p : listaProductos) {
            QJsonObject obj;
            obj["id"] = p.getId();
            obj["codigo"] = p.getCodigo();
            obj["nombre"] = p.getNombre();
            obj["precio"] = p.getPrecio();
            obj["existencias"] = p.getExistencias();
            obj["stockMinimo"] = p.getStockMinimo();
            obj["activo"] = p.esActivo();
            array.append(obj);
        }

        QJsonDocument doc(array);
        QFile archivo(rutaArchivo);
        if (!archivo.open(QIODevice::WriteOnly)) return false;

        archivo.write(doc.toJson());
        archivo.close();
        return true;
    }

    std::vector<Producto>& obtenerTodos() { return listaProductos; }
};

#endif // REPOSITORIOPRODUCTOS_H
#ifndef REPOSITORIOVENTAS_H
#define REPOSITORIOVENTAS_H

#include "RepositorioJson.h"
#include "Venta.h"
#include <vector>

class RepositorioVentas : public RepositorioJson<Venta> {
private:
    std::vector<Venta> listaVentas;

public:
    explicit RepositorioVentas(const QString& ruta = "ventas.json")
        : RepositorioJson(ruta) {}

    bool cargar() override {
        QFile archivo(rutaArchivo);
        if (!archivo.open(QIODevice::ReadOnly)) return false;

        QByteArray data = archivo.readAll();
        archivo.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray()) return false;

        QJsonArray array = doc.array();
        listaVentas.clear();

        for (const auto& valor : array) {
            QJsonObject obj = valor.toObject();

            QJsonArray arrLineas = obj["lineas"].toArray();
            std::vector<LineaVenta> lineas;
            for (const auto& lVal : arrLineas) {
                QJsonObject lObj = lVal.toObject();
                lineas.push_back({
                    lObj["idProducto"].toString(),
                    lObj["nombreProducto"].toString(),
                    lObj["precioHistorico"].toDouble(),
                    lObj["cantidad"].toInt(),
                    lObj["subtotal"].toDouble()
                });
            }

            Venta v(
                obj["folio"].toString(),
                QDateTime::fromString(obj["fecha"].toString(), Qt::ISODate),
                obj["idCajero"].toString(),
                obj["nombreCajero"].toString(),
                obj["subtotalBruto"].toDouble(),
                obj["montoDescuento"].toDouble(),
                obj["montoIVA"].toDouble(),
                obj["totalNeto"].toDouble(),
                obj["efectivoRecibido"].toDouble(),
                obj["cambio"].toDouble(),
                lineas,
                obj["estado"].toString()
            );
            listaVentas.push_back(v);
        }
        return true;
    }

    bool guardar() override {
        QJsonArray array;
        for (const auto& v : listaVentas) {
            QJsonObject obj;
            obj["folio"] = v.getFolio();
            obj["fecha"] = v.getFecha().toString(Qt::ISODate);
            obj["idCajero"] = v.getIdCajero();
            obj["nombreCajero"] = v.getNombreCajero();
            obj["subtotalBruto"] = v.getSubtotalBruto();
            obj["montoDescuento"] = v.getMontoDescuento();
            obj["montoIVA"] = v.getMontoIVA();
            obj["totalNeto"] = v.getTotalNeto();
            obj["efectivoRecibido"] = v.getEfectivoRecibido();
            obj["cambio"] = v.getCambio();
            obj["estado"] = v.getEstado();

            QJsonArray arrLineas;
            for (const auto& l : v.getLineas()) {
                QJsonObject lObj;
                lObj["idProducto"] = l.idProducto;
                lObj["nombreProducto"] = l.nombreProducto;
                lObj["precioHistorico"] = l.precioHistorico;
                lObj["cantidad"] = l.cantidad;
                lObj["subtotal"] = l.subtotal;
                arrLineas.append(lObj);
            }
            obj["lineas"] = arrLineas;
            array.append(obj);
        }

        QJsonDocument doc(array);
        QFile archivo(rutaArchivo);
        if (!archivo.open(QIODevice::WriteOnly)) return false;

        archivo.write(doc.toJson());
        archivo.close();
        return true;
    }

    std::vector<Venta>& obtenerTodas() { return listaVentas; }
};

#endif // REPOSITORIOVENTAS_H
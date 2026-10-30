#ifndef GESTORJSON_H
#define GESTORJSON_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <vector>
#include "VistaVentas.h"
#include "IGeneradorTicket.h"
#include "Usuario.h"

class GestorJSON {
private:
    QString rutaProductos;
    QString rutaVentas;

public:
    explicit GestorJSON(const QString& rutaProds = "productos.json", const QString& rutaVnts = "ventas.json");

    // Productos
    std::vector<ProductoItem> cargarProductos();
    bool guardarProductos(const std::vector<ProductoItem>& productos);

    // Ventas
    std::vector<DatosVentaTicket> cargarVentas();
    bool guardarVentas(const std::vector<DatosVentaTicket>& ventas);
    bool registrarVenta(const DatosVentaTicket& venta);

    // Usuarios
    std::vector<std::shared_ptr<Usuario>> cargarUsuarios();
    bool guardarUsuarios(const std::vector<std::shared_ptr<Usuario>>& usuarios);
};

#endif // GESTORJSON_H
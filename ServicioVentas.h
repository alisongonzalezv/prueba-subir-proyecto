#ifndef SERVICIOVENTAS_H
#define SERVICIOVENTAS_H

#include "Carrito.h"
#include "Venta.h"
#include "ServicioInventario.h"
#include "RepositorioVentas.h"
#include <memory>
#include <QUuid>

class ServicioVentas {
private:
    std::shared_ptr<ServicioInventario> servicioInventario;
    std::shared_ptr<RepositorioVentas> repoVentas;

public:
    ServicioVentas(std::shared_ptr<ServicioInventario> inv, std::shared_ptr<RepositorioVentas> ven)
        : servicioInventario(inv), repoVentas(ven) {
        repoVentas->cargar();
    }

    bool procesarVenta(const Carrito& carrito, double efectivoRecibido,
                       const QString& idCajero, const QString& nombreCajero,
                       Venta& ventaGenerada, QString& mensajeError) {

        if (carrito.getItems().empty()) {
            mensajeError = "El carrito de compras esta vacio.";
            return false;
        }

        double totalNeto = carrito.calcularTotalNeto();
        if (efectivoRecibido < totalNeto) {
            mensajeError = "El efectivo recibido es insuficiente para cubrir el total.";
            return false;
        }

        // Validar existencias
        for (const auto& item : carrito.getItems()) {
            if (item.producto.getExistencias() < item.cantidad) {
                mensajeError = "Stock insuficiente para el producto: " + item.producto.getNombre();
                return false;
            }
        }

        // Descontar inventario
        for (const auto& item : carrito.getItems()) {
            servicioInventario->descontarStock(item.producto.getId(), item.cantidad);
        }

        // Construir líneas de venta históricas
        std::vector<LineaVenta> lineas;
        for (const auto& item : carrito.getItems()) {
            lineas.push_back({
                item.producto.getId(),
                item.producto.getNombre(),
                item.producto.getPrecio(),
                item.cantidad,
                item.obtenerSubtotal()
            });
        }

        QString folio = "V-" + QUuid::createUuid().toString(QUuid::WithoutBraces).left(6).toUpper();
        double cambio = efectivoRecibido - totalNeto;

        ventaGenerada = Venta(
            folio,
            QDateTime::currentDateTime(),
            idCajero,
            nombreCajero,
            carrito.calcularSubtotalBruto(),
            carrito.calcularMontoDescuento(),
            carrito.calcularMontoIVA(),
            totalNeto,
            efectivoRecibido,
            cambio,
            lineas,
            "COMPLETADA"
        );

        repoVentas->obtenerTodas().push_back(ventaGenerada);
        repoVentas->guardar();

        return true;
    }
};

#endif // SERVICIOVENTAS_H